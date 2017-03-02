
#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;    
//const std::string hal_experimental::name("hal_experimental");
hal_experimental* hal_experimental::instance;
hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        shm(open_or_create, "PineappleJuice", 65536), /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
        dcm_proxy(NULL),
        nao_memory_proxy(NULL)
{
    setModuleDescription("Communicates between the Naoqi process and our Pineapple");
    last_reading_actuator = 255; ///<This is just initially set to an impossible actuator number
    actuator_update_fails = 0;
    std::cout << "Starting hal_experimental!" << std::endl;



    try
    {
        // Establish Communication with NaoQi
        dcm_proxy = new DCMProxy(pBroker);
        nao_memory_proxy = new ALMemoryProxy(pBroker);
        body_ID = static_cast<std::string>(nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0));
        // head_ID = (std::string) nao_memory_proxy->getData("RobotConfig/Head/FullHeadId", 0);
        // body_version = (std::string) nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0);
        // head_version = (std::string) nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0);
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


        ///BEGIN LED REQUEST ALIAS INITIALIZATION

        led_request_alias.arraySetSize(3);
        led_request_alias[1] = std::string("ClearAll");
        led_request_alias[2].arraySetSize(2);
        led_request_alias[2][0][1] = 0; //As you can see, LEDS are suuuper easy to work with

        ///END

        //Initialize Sensor Pointers
        for(int i = 0; i < NumOfSensorIds; ++i)
            sensor_ptrs[i] = static_cast<float *>(nao_memory_proxy->getDataPtr(sensorNames[i]));
        
        ///Initialize Requested Actuators
        std::memset(last_requested_actuators, 0, sizeof(last_reading_actuator)); //Allocate memory
        for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
            last_requested_actuators[i] = -1.f;
        
        dcm_proxy->getGenericProxy()->getModule()->atPreProcess(boost::bind(&hal_experimental::preCallBack, this));
        dcm_proxy->getGenericProxy()->getModule()->atPostProcess(boost::bind(&hal_experimental::postCallBack, this));
        
        



    
	} catch(AL::ALError &e) {
        std::cerr << "Fatal Error: Could Not Initialize Interface with NaoQi due to: "<< e.what() << std::endl;
    }
	catch (std::exception &e) { std::cout << "Error: " << e.what() << std::endl; }

    //Boost Interprocess Shared Memory Initialization:

    try
    {
        //boost::shared_memory_object::remove("juicyData"); // When the hal module is initialized, remove any pre-existing shared memory objects
        std::cout << "[Construction] Shared Memory Object juicyData removed!" << std::endl;
        pineappleJuice = shm.construct<hal_data>("juicyData")(/*Paramers for struct's initial values can go here*/); 
        std::cout << "pineappleJuice created in shared memory" << std::cout;
    } 
    catch(boost::interprocess::interprocess_exception &e) 
    {
        std::cout << "Interprocess error: " << e.what() << std::endl;
    }
    //set up the callbacks for ALBroker

}
/**
* The method is called by NaoQi immediately before it communicates with the chest board.
* It sets all the actuators.
*/
void hal_experimental::preCallBack() 
{
    instance->set_actuators();
}
/**
* The method is called by NaoQi immediately after it communicates with the chest board.
* It reads all sensors.
*/
void hal_experimental::postCallBack()
{
   instance->read_sensors();
}



hal_experimental::~hal_experimental()
{
    std::cout << "Destructing hal_experimental" << std::endl;
    /**
      * Note: If naoqi crashes, by default all data stored in shared memory will (probably)
      * be destroyed. This is why NAOInterface keeps a local copy of the data.
      **/
        
    shm.destroy<hal_data>("juicyData");
    std::cout << "[Destruction] Shared Memory Object juicyData removed!" << std::endl;
    delete dcm_proxy;
    delete nao_memory_proxy;
}

void hal_experimental::set_actuators()
{
    std::cout << "Setting actuators" << std::endl;

	try
	{
        pineappleJuice->semaphore.wait(); //Wait for anything using Interproc
		dcm_time = dcm_proxy->getTime(0); ///< Get's current time on NAO
		pineappleJuice->actuators_current_read = pineappleJuice->actuators_newest_update;
		if (pineappleJuice->actuators_newest_update != last_reading_actuator)
		{
			std::cout << "NaoQi has failed at updating the actuator value. Bad NaoQi. Bad." << std::endl;
			actuator_update_fails++;
		}
		else actuator_update_fails = 0;

		last_reading_actuator = pineappleJuice->actuators_newest_update;

		float* read_actuators = pineappleJuice->actuators[pineappleJuice->actuators_current_read];
        pineappleJuice->semaphore.post();
        std::cout << "Actuator values set" << std::endl;
	}
    catch(std::exception &e) //uh...this could yield an Boost.Interprocess exception or a AL::Exception...I wonder if I can throw it as a boost variant?
    {
        std::cout << "set_actuators exception: " << e.what() << std::endl;
    }
}

void hal_experimental::read_sensors()
{
    std::cout << "reading sensors" << std::endl;
    try
    {
        pineappleJuice->semaphore.wait();
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

        pineappleJuice->semaphore.post();

    }
    catch (const std::exception &e)
    {
        std::cout << "An error occured while trying to read the sensor data: " << e.what() << std::endl;   
    }
}

extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
    AL::ALModule::createModule<hal_experimental>(pBroker, "hal_experimental");
    return 0;
}

extern "C" int _closeModule()
{
    return 0;
}
