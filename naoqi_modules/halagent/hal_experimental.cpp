#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;    
//const std::string hal_experimental::name("hal_experimental");
hal_experimental* hal_experimental::instance = NULL;
hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        dcm_proxy(NULL),
        nao_memory_proxy(NULL),
        ledIndex(0)
{
    setModuleDescription("Communicates between the Naoqi process and our Pineapple");
    last_reading_actuator = 255; ///<This is just initially set to an impossible actuator number
    actuator_update_fails = 0;
    std::cout << "Starting hal_experimental!" << std::endl;

    try
    {
        std::cout << "Initializing Shared Memory with Boost.Interprocess. [PineappleJuice]" << std::endl;
        shm = boost::interprocess::managed_shared_memory(open_or_create, "PineappleJuice", 65536); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
        if(shm.get_size() != 65536) std::cout << "ERROR: Did not allocate enough memory. SHM size: " << shm.get_size() << std::endl;
        
        pineappleJuice = shm.find_or_construct<hal_data>("juicyData")(/*Constructor, assuming use of default constructor*/);
        shared_data_ptr = shm.find<hal_data>("juicyData");
        if(!shared_data_ptr.first)
        {
            std::cout << "[FATAL] juicyData failed to be found" << std::endl;
            return;
        }
        pineappleJuice = shared_data_ptr.first; ///< Dumb hack to stop boost interprocess from causing segfaults
        std::cout << "pineappleJuice created in shared memory" << std::endl;
    } 
    catch(boost::interprocess::interprocess_exception &e) 
    {
        std::cout << "[FATAL] An Interprocess error: " << e.what() << std::endl;
        std::cout << "Cleaning up shared memory..." << std::endl;
        shm.destroy<hal_data>("juicyData");
        boost::interprocess::shared_memory_object::remove("PineappleJuice");
    }
    try
    {
        // Establish Communication with NaoQi
        dcm_proxy = new DCMProxy(pBroker);
        nao_memory_proxy = new ALMemoryProxy(pBroker);
        body_ID = static_cast<std::string>(nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0));
        // head_ID = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Head/FullHeadId", 0));
        // body_version = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0));
        // head_version = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0));
        std::cout << "HeadID proxy call value: " << body_ID << std::endl;
        commands.arraySetSize(2); //< Two dimensional array.

        ///BEGIN POSITION ACTUATOR ALIAS INITIALIZATION

        commands[0] = std::string("positionActuators");
        commands[1].arraySetSize(NumOfPositionActuatorIds); //< Ugly data structure
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        {
            commands[1][i] = std::string(actuatorNames[i]);
        }
        // Create Memory Proxy aliases for actuator values
        commandsAlias = dcm_proxy->createAlias(commands);
        
        ///END

        std::cout << "1. Actuator Alias Initialized." << std::endl;


        ///BEGIN STIFFNESS ACTUATOR ALIAS INITIALIZATION
       
        commands[0] = std::string("stiffnessActuators");
        commands[1].arraySetSize(NumOfStiffnessActuatorIds); //< Ugly data structure
        for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        {
            commands[1][i] = std::string(actuatorNames[headYawStiffnessActuator + i]);
        }
        commandsAlias = dcm_proxy->createAlias(commands);
       
        ///END


        ///BEGIN POSITION REQUEST ALIAS INITIALIZATION

        position_request_alias.arraySetSize(6);
        position_request_alias[0] = std::string("positionActuators");
        position_request_alias[1] = std::string("ClearAll"); //Clear any set values in the alias
        position_request_alias[2] = std::string("time-seperate"); //Timing Paramers
        position_request_alias[3] = 0; //idk
        position_request_alias[4].arraySetSize(1);
        position_request_alias[5].arraySetSize(NumOfPositionActuatorIds);
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        {
            position_request_alias[5][i].arraySetSize(1);
        }

        ///END

        std::cout << "2. Position Alias Initialized." << std::endl;


        ///BEGIN STIFFNESS REQUEST ALIAS INITIALIZATION

        stiffness_request_alias.arraySetSize(6);
        stiffness_request_alias[0] = std::string("stiffnessActuators");
        stiffness_request_alias[1] = std::string("ClearAll"); //Clear any set values in the alias
        stiffness_request_alias[2] = std::string("time-seperate"); //Timing Paramers
        stiffness_request_alias[3] = 0; //idk
        stiffness_request_alias[4].arraySetSize(1);
        stiffness_request_alias[5].arraySetSize(NumOfStiffnessActuatorIds);
        for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        {
            stiffness_request_alias[5][i].arraySetSize(1);
        }

        ///END

        std::cout << "3. Stiffness Alias Initialized." << std::endl;


        ///BEGIN LED REQUEST ALIAS INITIALIZATION

        led_request_alias.arraySetSize(3);
        led_request_alias[1] = std::string("ClearAll");
        led_request_alias[2].arraySetSize(1);
        led_request_alias[2][0].arraySetSize(2);
        led_request_alias[2][0][1] = 0; //As you can see, LEDS are suuuper easy to work with

        ///END

        std::cout << "4. LED Alias Initialized." << std::endl;

        //Initialize Sensor Pointers
        for(int i = 0; i < NumOfSensorIds; ++i)
            sensor_ptrs[i] = static_cast<float *>(nao_memory_proxy->getDataPtr(sensorNames[i]));
        
        std::cout << "5. Sensor Pointers initialized" << std::endl;

        ///Initialize Requested Actuators
        void *ms_ret = std::memset(last_requested_actuators, 0, sizeof(last_reading_actuator)); //Allocate memory
        if(ms_ret == NULL)
        {
            std::cout << "A catastrophic error has occured! Could not set memory for actuator values." << std::endl;
            return;
        }
        for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
            last_requested_actuators[i] = -1.f;
        
        std::cout << "6. Requested Actuators Initialized." << std::endl;


        instance = this; ///< Remove this to cause fun segfaults...
        dcm_proxy->getGenericProxy()->getModule()->atPreProcess(boost::bind(&hal_experimental::preCallBack, this));
        dcm_proxy->getGenericProxy()->getModule()->atPostProcess(boost::bind(&hal_experimental::postCallBack, this));
        
        std::cout << "7. Proxy call backs established." << std::endl;
	} catch(AL::ALError &e) {
        std::cerr << "Fatal Error: Could Not Initialize Interface with NaoQi due to: "<< e.what() << std::endl;
    }
    //Boost Interprocess Shared Memory Initialization:
            std::cout << "SEMAPHORE TEST" << std::endl;
        pineappleJuice->actuator_semaphore.wait();
        std::cout << "still segfaulting?" << std::endl;
        if(dcm_proxy == NULL) std::cout << "dcm_proxy is null\n" << std::endl; else std::cout << "dcm_proxy is not null" << std::endl; 
        dcm_time = dcm_proxy->getTime(0); ///< Get's current time on NAO
        std::cout << "NO SEGFAULT: DCM Time = " << dcm_time << std::endl;
        pineappleJuice->actuator_semaphore.post();

    std::cout << "hal_experimental Fully Initialized!" << std::endl;

}
/**
* The method is called by NaoQi immediately before it communicates with the chest board.
* It sets all the actuators.
*/
void hal_experimental::preCallBack() 
{
    std::cout << "Pre-Callback called" << std::endl;
    instance->set_actuators();
}
/**
* The method is called by NaoQi immediately after it communicates with the chest board.
* It reads all sensors.
*/
void hal_experimental::postCallBack()
{
    std::cout << "Post-Callback called" << std::endl;  
    instance->read_sensors();
}



hal_experimental::~hal_experimental()
{
    std::cout << "Destructing hal_experimental" << std::endl;
    /**
      * Note: If naoqi crashes, by default all data stored in shared memory will (probably)
      * be destroyed. This is why NAOInterface keeps a local copy of the data.
      **/
        
    try
    {
        shm.destroy<hal_data>("juicyData");
        boost::interprocess::shared_memory_object::remove("PineappleJuice");
    } 
    catch(boost::interprocess::interprocess_exception &e)
    {
        std::cout << "[CRITICAL][DESTRUCTOR] Could not clean up shared memory!" << std::endl;
    }
    delete dcm_proxy;
    delete nao_memory_proxy;
}

void hal_experimental::set_LEDS()
{
    for(int i = faceLedRedLeft0DegActuator; i <= faceLedBlueRight315DegActuator; ++i)
        actuators[i] = 0.f;
    // LED Debug stuff can go here
    float blink_val = float(dcm_time / 500 & 1);
    actuators[faceLedGreenRight180DegActuator] =  blink_val;
    actuators[faceLedGreenLeft180DegActuator] = blink_val;
}

float* hal_experimental::robot_state_handler(float *actuator_vals)
{
    return actuator_vals;
}

/**
  * \brief: Helper function to set actuator positions 
  **/
void hal_experimental::set_actuators_positions()
{
    try
    {
        //Set positions
        position_request_alias[4][0] = dcm_time; 
        for(int i = 0; i < NumOfActuatorIds; ++i)
            position_request_alias[5][i][0] = actuators[i];
        dcm_proxy->setAlias(position_request_alias); //Assign the alias for the naoqi DCM
    } 
    catch (const AL::ALError &e)
    {
        std::cout << "An exception has occured while setting actuator positions: " << e.what() << std::endl;
    }
}

/**
  * \brief: Tries to set the stiffness values requested from NAOInterface for the servos.
  **/
bool hal_experimental::set_actuators_stiffness()
{
    std::cout << "Stiffness IDs to be set: " << headYawStiffnessActuator + NumOfStiffnessActuatorIds << std::endl;
    //std::cout << "Array Size: " << 
    try{
        for(int i = headYawStiffnessActuator; i < headYawStiffnessActuator + NumOfStiffnessActuatorIds; ++i)
        {
            if(actuators[i] != last_requested_actuators[i])
            {
                stiffness_request_alias[4][0] = dcm_time;
                for(int j = 0; j < NumOfActuatorIds; ++j)
                    stiffness_request_alias[5][j][0] = last_requested_actuators[headYawStiffnessActuator + j] = actuators[headYawStiffnessActuator + j];
                dcm_proxy->setAlias(stiffness_request_alias);
                return true; //Stiffness values succesfully set
            }
        }
    }
    catch(const AL::ALError &e)
    {
        std::cout << "An Exception has occured while setting stiffness actuators: " << e.what() << std::endl;
    }
    return false; //Requested stiffness values failed to be set
}

/**
  * \brief: Helper function to set LED values on the NAO
  **/
void hal_experimental::set_actuators_leds(bool &requested_stiffness_set)
{
    if(!requested_stiffness_set)
    {
        try
        {
            for(int i = 0; i < NumOfLedActuatorIds; ++i)
            {
                //Local LED index. Don't confuse with ledIndex variable
                int led_index = faceLedBlueLeft180DegActuator + ledIndex;
                //Ensure our ledIndex isnt > actual number of leds
                if(NumOfLedActuatorIds == ledIndex++) ledIndex = 0;
                if(actuators[led_index] != last_requested_actuators[led_index])
                {
                    led_request_alias[0] = std::string(actuatorNames[led_index]);
                    //Update last_requested_actuators val to actuators val
                    last_requested_actuators[led_index] = actuators[led_index];
                    //Finish setting value and tell naoqi
                    led_request_alias[2][0][0] = last_requested_actuators[led_index];
                    led_request_alias[2][0][1] = dcm_time;
                    dcm_proxy->set(led_request_alias);
                    return;
                }
            }
        } 
        catch (const AL::ALError &e)
        {
            std::cout << "An Exception has occured while setting LEDS: " << e.what() << std::endl;   
        }
    }
}
void hal_experimental::set_actuators()
{
	try
	{
        pineappleJuice->actuator_semaphore.wait();
        dcm_time = dcm_proxy->getTime(0);
        pineappleJuice->actuators_current_read = pineappleJuice->actuators_newest_update;
        if (pineappleJuice->actuators_newest_update != last_reading_actuator)
        {
            if(actuator_update_fails == 0)
                std::cout << "NaoQi has failed at updating the actuator value. Bad NaoQi. Bad." << std::endl;
            actuator_update_fails++;
        }
        else actuator_update_fails = 0;
        last_reading_actuator = pineappleJuice->actuators_newest_update;
        read_actuators = pineappleJuice->actuators[pineappleJuice->actuators_current_read];
        //Get actual actuator vals to be assigned. These values won't exactly correspond to
        //what NAOInterface passed due to the various states the robot may be in.
        actuators = robot_state_handler(read_actuators);
        pineappleJuice->actuator_semaphore.post();

        //Set Actuator values
        set_actuators_positions();
        std::cout << "set_actuators_positions was called\n";
        bool was_set = set_actuators_stiffness();
        std::cout << "set_actuators_stiffness was called\n";
        set_actuators_leds(was_set);
        std::cout << "set_actuators_leds was called" << std::endl;
        //TODO: Gamecontroller stuff (team info)
        
	}
    catch(AL::ALError &e) //uh...this could yield an Boost.Interprocess exception or a AL::Exception...I wonder if I can throw it as a boost variant?
    {
        std::cout << "set_actuators exception: " << e.what() << std::endl;
    }
    print_actuators();    
}



void hal_experimental::read_sensors()
{
    try
    {
        pineappleJuice->sensor_semaphore.wait();
        int sensor_data_written = 0;
        if(sensor_data_written == pineappleJuice->sensors_newest_update)
        {
            sensor_data_written++;
        }
        if(sensor_data_written == pineappleJuice->sensors_newest_read && sensor_data_written == pineappleJuice->sensors_newest_update )
        {
            sensor_data_written++;
        }

        //Safety Test:
        if(sensor_data_written == pineappleJuice->sensors_newest_read || sensor_data_written == pineappleJuice->sensors_newest_update)
        {
            std::cerr << "Sensor data being overwritten to shared memory!" << std::endl; 
        }
        float *current_sensor = pineappleJuice->sensors[sensor_data_written];
        for(int i = 0; i < NumOfSensorIds; ++i)
        {
            current_sensor[i] = *sensor_ptrs[i];
        }
        //Update sensor values in shared memory
        pineappleJuice->sensors_newest_update = sensor_data_written;
        std::cout << "[read_sensors()] sensor data read...posting semaphore." << std::endl;
        
        pineappleJuice->sensor_semaphore.post();
        std::cout << "[read_sensors()] semaphore posted!" << std::endl;
        
    }
    catch (const std::exception &e)
    {
        std::cout << "An error occured while trying to read the sensor data: " << e.what() << std::endl;   
    }
    print_sensors();
}

void hal_experimental::print_sensors()
{
    std::cout << "================= SENSOR VALUES! =================" << std::endl;
    pineappleJuice->sensor_semaphore.wait();
    for(int i = 0; i < NumOfSensorIds; ++i)
        std::cout << sensorNames[i] << " = " << pineappleJuice->sensors[pineappleJuice->sensors_newest_update][i] << std::endl;
    pineappleJuice->sensor_semaphore.post(); 
}
void hal_experimental::print_actuators()
{
    std::cout << "================= ACTUATOR VALUES! =================" << std::endl;
    pineappleJuice->actuator_semaphore.wait();
    for(int i = 0; i < NumOfActuatorIds; ++i)
        std::cout << sensorNames[i] << " = " << pineappleJuice->actuators[pineappleJuice->actuators_newest_update][i] << std::endl;
    pineappleJuice->sensor_semaphore.post(); 
}

extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
    AL::ALModule::createModule<hal_experimental>(pBroker, "hal_experimental");
    std::cout << "_createModule<hal_experimental>... Called!" << std::endl;
    return 0;
}

extern "C" int _closeModule()
{
    std::cout << "_closeModule Called!" << std::endl;
    return 0;
}
