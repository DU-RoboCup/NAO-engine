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
        init_test(false)
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
    std::cout << "[Destruction] Shared Memory Object juicyData removed!" << std::endl;
    delete dcm_proxy;
    delete nao_memory_proxy;
}

void hal_experimental::set_actuators()
{
    std::cout << "Set Actuators called" << std::endl;
    
    // if(dcm_proxy == NULL) std::cout << "dcm_proxy is null\n" << std::endl; else std::cout << "dcm_proxy is not null" << std::endl; 
    // dcm_time = dcm_proxy->getTime(0); ///< Get's current time on NAO
    // std::cout << "NO SEGFAULT: DCM Time = " << dcm_time << std::endl;
    // pineappleJuice->actuator_semaphore.post();
    // std::cout << "0. [set_actuators()] Setting actuators" << std::endl;

	try
	{
        pineappleJuice->actuator_semaphore.wait();
        std::cout << "1. [set_actuators()] semaphore wait called" << std::endl;
        dcm_time = dcm_proxy->getTime(0);
        std::cout << "[test]: DCMTime = " << dcm_time << std::endl;
        std::cout << "2. [set_actuators()] current dcm time called: " << dcm_time << std::endl;
        pineappleJuice->actuators_current_read = pineappleJuice->actuators_newest_update;
        std::cout << "3. [set_actuators()] fetched new read vals" << std::endl;
        if (pineappleJuice->actuators_newest_update != last_reading_actuator)
        {
            std::cout << "NaoQi has failed at updating the actuator value. Bad NaoQi. Bad." << std::endl;
            actuator_update_fails++;
        }
        else actuator_update_fails = 0;
        last_reading_actuator = pineappleJuice->actuators_newest_update;
        std::cout << "4. [set_actuators()] updated last reading vals." << std::endl;
        float* read_actuators = pineappleJuice->actuators[pineappleJuice->actuators_current_read];
        std::cout << "5. [set_actuators()] placed read_actuators in a float*" << std::endl;
        pineappleJuice->actuator_semaphore.post();
        std::cout << "6. [set_actuators()] Semaphore posted" << std::endl;
	}
    catch(AL::ALError &e) //uh...this could yield an Boost.Interprocess exception or a AL::Exception...I wonder if I can throw it as a boost variant?
    {
        std::cout << "set_actuators exception: " << e.what() << std::endl;
    }
    std::cout << "7. [set_actuators()] Finished setting Actuators." << std::endl; 
    
}


void hal_experimental::read_sensors()
{
     std::cout << "reading sensors: " << std::endl;
    // //std::cout << "bool val: " << this->init_test << std::endl;
    // if(this->init_test == NULL) std::cout << "The bool is null" << std::endl; else std::cout << "The Bool is NOT NULL" << std::endl;
    // if(instance->init_test)
    // {
    //     instance->pineappleJuice->sensor_semaphore.wait();
    //     //...
    //     std::cout << "Sensor Semaphore Test!" << std::endl;
    //     instance->pineappleJuice->sensor_semaphore.post();
    //     std::cout << "READ SEM TEST passed" << std::endl;
    // } else {
    //     std::cout << "Changing init_test values" << std::endl;
    //     instance->init_test = true;
    // }
    try
    {
        pineappleJuice->sensor_semaphore.wait();
        std::cout << "[read_sensors()] Try wait was succesful" << std::endl;
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

        pineappleJuice->sensors_newest_update = sensor_data_written;
        std::cout << "[read_sensors()] sensor data read...posting semaphore." << std::endl;
        
        pineappleJuice->sensor_semaphore.post();
        std::cout << "[read_sensors()] semaphore posted!" << std::endl;
        
    }
    catch (const std::exception &e)
    {
        std::cout << "An error occured while trying to read the sensor data: " << e.what() << std::endl;   
    }
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
