#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;


// Macro to make the NAO say stuff. Use carefully (if spammed it'll crash the DCM also its really damn annoying)
#define SAY(text) speak_proxy->post.say(text);
//Macro for logging
#define LOG2(msg) std::cout << "[" << __func__ << "]: " << msg << std::endl;
//Macro for semaphore name
#ifndef PINEAPPLE_SEMAPHORE
    #define PINEAPPLE_SEMAPHORE "pineapple_semaphore"
#endif

//const std::string hal_experimental::name("hal_experimental");
hal_experimental* hal_experimental::instance = NULL;
hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        fMemoryFastAccess(boost::shared_ptr<AL::ALMemoryFastAccess>(new AL::ALMemoryFastAccess())),
        nao_memory_proxy(NULL),
        speak_proxy(NULL),
        /*actuator_semaphore(open_or_create, "actuator_semaphore", 1),
        sensor_semaphore(open_or_create, "sensor_semaphore", 1),*/
        ////DEBUG STUFF
        testLEDInitialized(false),
        testLEDRot(0)
{
    setModuleDescription("Communicates between the Naoqi process and our Pineapple");
    last_reading_actuator = 255; ///<This is just initially set to an impossible actuator number
    actuator_update_fails = 0;
    std::cout << "Starting hal_experimental!" << std::endl;
    try
    {
        //Debug
       log_file.open("/home/nao/NAOQI-LOG2.log", std::ios::out);
    } catch(...) {
        SAY("I could't use the LOG File");
    }
    std::cout << "log file opened\n";
    create_interprocess_memory();
    //open_interprocess_memory();

    //HardwareMap hm;
    try
    {
        std::cout << "Connecting to speech and normal memory proxies" << std::endl;
        // Establish Communication with NaoQi
        //dcm_proxy = new DCMProxy(pBroker);
        //dcm_proxy = getParentBroker()->getDcmProxy();
        nao_memory_proxy = new ALMemoryProxy(pBroker);
        speak_proxy = new ALTextToSpeechProxy("localhost", 9559);
        //body_ID = static_cast<std::string>(nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0));
        //head_ID = static_cast<std::string>(nao_memory_proxy->getData("Device/DeviceList/Head/FullHeadId", 0));
        // body_version = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0));
        // head_version = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0));
        //log_file << "HeadID proxy call value: " << head_ID << "\n" << "BodyID: " << body_ID;
        //SAY("You better have fixed me this time!");
        

        instance = this; ///< Remove this to cause fun segfaults...

        std::cout << "Almost fully contructed!" << std::endl;
	} catch(AL::ALError &e) {
        log_file << "[ERROR] Fatal Error: Could Not Initialize Interface with NaoQi due to: "<< e.what() << "\n";
    }
        
    for(int i = 0; i < NumOfActuatorIds; ++i)
        actuators[i] = 0.f;

    // SAY("I will say my IP Address in a moment. It's just, well Linux hates me.");
    // get_ip_address();

    pBrokerCopy = pBroker;

    SAY("Watch out I'm going to jump!");

    //Stuff for NAOQi/Python binding generation
    setModuleDescription("Direct access to DCM/Memory for hardware IO with interprocess communication");
    functionName("startLoop", getName() , "start");
    BIND_METHOD(hal_experimental::startLoop);

    functionName("stopLoop", getName() , "stop");
    BIND_METHOD(hal_experimental::stopLoop);

    functionName("testLEDS", getName(), "test chest LEDS");
    BIND_METHOD(hal_experimental::testLEDS);

    functionName("set_all_actuator_stiffnesses" , getName(), "change stiffness of all joint");
    addParam("value", "new stiffness value from 0.0 to 1.0");
    BIND_METHOD(hal_experimental::set_all_actuator_stiffnesses);

    functionName("update_actuator_values", getName(), "interprocess sync of actuators");
    BIND_METHOD(hal_experimental::update_actuator_values);

    functionName("update_sensor_value", getName(), "interprocess sync of sensor values");
    BIND_METHOD(hal_experimental::update_sensor_values);

    functionName("update_text_to_speak", getName(), "interprocess sync of text to speak");
    BIND_METHOD(hal_experimental::update_text_to_speak);

    startLoop();
    std::cout << "hal_experimental Fully Initialized!" << std::endl;

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
        sem_close(semaphore);
        // named_semaphore::remove("actuator_semaphore");
        // named_semaphore::remove("sensor_semaphore");
    } 
    catch(boost::interprocess::interprocess_exception &e)
    {
        std::cout << "[CRITICAL][DESTRUCTOR] Could not clean up shared memory!" << std::endl;
    }
    log_file << "My journey is over. **Kills self**\n";
    log_file << "Narator: And that is how the tragic life of the dumb robot ended." << std::endl;
    log_file.close();
    //delete dcm_proxy;
    delete nao_memory_proxy;
    delete speak_proxy;
}
/**
  * create_interprocess_memory: Creates shared memory with unrestricted access using boost interprocess.
  **/
void hal_experimental::create_interprocess_memory()
{
    // Initialize the interprocess shared memory
    try
    {
        permissions shm_permissions;
        shm_permissions.set_unrestricted();
        log_file << "Initializing Shared Memory with Boost.Interprocess. [PineappleJuice]\n";
        shm = boost::interprocess::managed_shared_memory(open_or_create, "PineappleJuice", 65536, 0, shm_permissions); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
        std::cout << "Opened pineapple juice" << std::endl;
        if(shm.get_size() != 65536)
        {
            log_file << "ERROR: Did not allocate enough memory. SHM size: " << shm.get_size() << "\n";
            std::cout << "ERROR: Not enough memory was allocated! " << shm.get_size() << std::endl;
        } else {
            std::cout << "PineappleJuice created with size of: " << shm.get_size() << std::endl;
        }
        pineappleJuice = shm.find_or_construct<hal_data>("juicyData")(/*Constructor, assuming use of default constructor*/);
        shared_data_ptr = shm.find<hal_data>("juicyData");
        if(!shared_data_ptr.first)
        {
            LOG("[Constructor]","[FATAL] juicyData failed to be found!");
            return;
        } else {
            LOG2("juicyData created!");
            pineappleJuice = shared_data_ptr.first; 
            log_file << "[Constructor]: Connected to Interprocess Shared Memory!" << std::endl;
        }
        ///< Dumb hack to stop boost interprocess from causing segfaults
       LOG("[Constructor]","pineappleJuice created in shared memory!");
       std::cout << "Interprocess memory setup" << std::endl;
    }
    catch(const boost::interprocess::interprocess_exception &e) 
    {
        log_file << "[FATAL] An Interprocess error: " << e.what() << ". Cleaning up shared memory..." << std::endl;
        shm.destroy<hal_data>("juicyData");
        boost::interprocess::shared_memory_object::remove("PineappleJuice");
    }
    std::cout << "Interprocess memory created" << std::endl;
    /* Create our named semaphore for syncing data between NAOQi and the NAO-Engine
       We switched this from a boost::interprocess semaphore to a POSIX one due to
       some blocking issues. If you'd like to use boost, create the named semaphore
       and use the try_wait() call
    */
    semaphore = sem_open(PINEAPPLE_SEMAPHORE, O_RDWR);
    if(semaphore == SEM_FAILED)
	{
		LOG2("Named semaphore does not exist, creating now...");
        //Create the semaphore with [!DIRECTORY, READ, WRITE, EXECUTE] permissions and an initial count of 1
		semaphore = sem_open(PINEAPPLE_SEMAPHORE, O_CREAT, 0666, 1);
	} 
    else
		LOG2("Named semaphore has already been created and was found in /dev/shm!!!");
    
}
void hal_experimental::open_interprocess_memory()
{
    LOG2("Accessing shared memory...");
	try
	{
        shm = managed_shared_memory(open_only, "PineappleJuice"); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
		if(shm.get_size() < 65536)
		{
			LOG2("Error, there managed memory regions size is incorrect!");
		} else {
			LOG2("Found share memory region PineappleJuice!");
		}
		std::pair<hal_data *, std::size_t> shmem_region_ptr = shm.find_no_lock<hal_data>("juicyData");
		if(!shmem_region_ptr.first)
		{
			LOG2("Error, could not find shared memory object juicyData!!!");
		} else {
			LOG2("Found shared memory object juicyData!");
			pineappleJuice = shmem_region_ptr.first;
		}
		LOG2("Shared Memory should be setup!");
	}
	catch (const interprocess_exception &e)
	{
		std::cout << "NAOInterface could not open required shared memory objects. Error Code: " << e.what() << ".";
	}
}
/**
  * \brief: Sets up all of the aliases for reading sensor values and setting hardware values
  **/
void hal_experimental::initialize_everything()
{
    LOG("[init_everything]","Creating Aliases...");
    create_fast_sensor_access();
    create_actuator_position_aliases();
    create_actuator_stiffness_aliases();
    LOG("[init_everything]","All aliases created!");

    set_all_actuator_stiffnesses(0.2f);
    init_actuator_position_aliases();
    LOG("[init_everything]","Done!");
    SAY("I should have done things.");
}

/////////////////////////////////////////////////////////////////////
///////////////////     REAL TIME HOOKS     /////////////////////////
/////////////////////////////////////////////////////////////////////
/** DANGER: There be dragons ahead.
  If you have any blocking/slow calls that occur below, you WILL kill
  the NaoQI real time thread. You can visually tell that you've killed it
  if the NAO's chestboard blinks red. Blocking calls induce some sort of
  priority inversion I believe. 
  **/
void hal_experimental::startLoop()
{
    try
    {
        dcm_proxy = getParentBroker()->getDcmProxy();
        lastTime = dcm_proxy->getTime(0);
        LOG("[startloop]","DCM proxy connection established!");
    }
    catch(const AL::ALError &e)
    {
        LOG("[startloop]","[ERROR]: Error getting dcmProxy: " + e.toString());
    }

    signed long isDCMRunning;
    try
    {
        isDCMRunning = getParentBroker()->getProxy("ALLauncher")->call<bool>("isModulePresent", std::string("DCM"));
        LOG("[startloop]","DCM is running!");
    }
    catch (AL::ALError& e)
    {
        LOG("[startLoop]","[ERROR] Error when connecting to DCM : " + e.toString());
    }

    if (!isDCMRunning)
    {
        LOG("[startLoop]","[ERROR]: Error no DCM running!");
    }

    std::cout << "Starting main loops!" << std::endl;
    initialize_everything();
    connectToDCMLoop();
}
void hal_experimental::stopLoop()
{
    set_all_actuator_stiffnesses(0.0f);
    fDCMPreProcessConnection.disconnect();
    fDCMPostProcessConnection.disconnect();
    LOG("[stopLoop]", "Real time hooks disconnected!");
    try
    {
        shm.destroy<hal_data>("juicyData");
        boost::interprocess::shared_memory_object::remove("PineappleJuice");
        // named_semaphore::remove("actuator_semaphore");
        // named_semaphore::remove("sensor_semaphore");
    } 
    catch(boost::interprocess::interprocess_exception &e)
    {
        std::cout << "[CRITICAL][DESTRUCTOR] Could not clean up shared memory!" << std::endl;
    }
}
/** 
* The method is called by NaoQi immediately before it communicates with the chest board.
* It sets all the actuators.
*/
void hal_experimental::connectToDCMLoop()
{
    std::cout << "connecting to dcm loop" << std::endl;
    LOG("[connectToDCMLoop]", "beginning fastMemoryAccess stuff");
    try
    {

        fMemoryFastAccess->GetValues(sensorValues);
        LOG("[connectToDCMLoop]","Values Got with FastAccess: " + sensorValues.size());
        //DEBUG

        try
        {
            log_file << "fSensorKeys size = " << fSensorKeys.size() << "\tTrying to print sensorValues: " << sensorValues.size() << std::endl;
            if(fSensorKeys.size() != 0)
            {
                for(int i = 0; i < sensorValues.size(); ++i)
                    std::cout << fSensorKeys[i] << " = " << sensorValues[i] << std::endl;
            }
            else
                log_file << "[connectToDCMLoop][ERROR]: fSensorKeys.size = 0" << std::endl;
        } 
        catch(const std::exception &e)
        {
            std::cout << "Error printing sensor values: " << e.what() << std::endl;
        }
        LOG("[connectToDCMLoop]","Got sensor values with fast access! ");
        try
        {
       // for(int i = 0; i < NumOfPositionActuatorIds; ++i)
            LOG("[connectToDCMLoop]", "Assigning inital vals to vector AND sharedMemory");
            //pineappleJuice->actuator_semaphore.wait();
            sem_wait(semaphore);
            for(int i = 0; i < 25; ++i)
            {
                initialJointSensorValues.push_back(sensorValues[i]);
                pineappleJuice->sensor_values[i] = sensorValues[i];
                log_file << "[connectToDCMLoop]: initialSensorValues[" << i << "] = " << sensorValues[i] << std::endl;
            }
            //pineappleJuice->actuator_semaphore.post();
            LOG("[connectToDCMLoop]","Done assigning initial sensor value!");
        } 
        catch(const boost::interprocess::interprocess_exception &e)
        {
            std::cout << "Error assigning inital values: " << e.what() << std::endl;
            log_file << "Error assigning inital values: " << e.what() << std::endl;

        }
        sem_post(semaphore);

        try
        {
            LOG("[connectToDCMLoop]","setting up pre/post process hooks...");

            //Connect our real time functions to the DCM
            fDCMPostProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(boost::bind(&hal_experimental::onPreProcess, this));
            fDCMPreProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPreProcess(boost::bind(&hal_experimental::onPostProcess, this));
            LOG("[connectToDCMLoop]", "pre/postProcess hook connected!");
        }
        catch(const AL::ALError &e)
        {
            LOG("[connectToDCMLoop][ERROR]", "Couldn't connect to real time hooks: " + e.toString());
        }
    }
    catch(const AL::ALError &e)
    {
        LOG("[connectToDCMLoop]","[ERROR]: Error while getting initial sensor values: " + e.toString());
    }
    LOG("[connectToDCMLoop]", "Done!");
    std::cout << "Done Connecting to DCM Loop" << std::endl;
}

/**
  * Real time function called before DCM I/O.
  * Primary use is for setting actuator
  **/
void hal_experimental::onPreProcess()
{
    update_actuator_values();
    //actuator_joint_test();
    //update_text_to_speak();
}

/**
  * Real time function called after DCM I/O.
  * Primary use for reading sensor values.
  **/

void hal_experimental::onPostProcess()
{
    update_sensor_values();
    testLEDS();
}
////////////////////////////////////////////////////////////
///////////////// END REAL TIME HOOKS //////////////////////
////////////////////////////////////////////////////////////


/**
* get_ip_address: Executes a shell command to retrieve the NAO's IP Address
* NOTE: There is a blocking call that lasts like 10s before the python script is run or anything else
*       in NAOQi can be run so please avoid using this.
*/
void hal_experimental::get_ip_address()
{
    //Listen up kids, this here is how you can easily introduce a 
    //buffer overflow and use an exploit to make the NAO part of your botnet.
    log_file << "get_ip_address: Performing Sanity Check\n";
    try {
        system("python /home/nao/say_ip_address.py");
    } 
    catch(const std::exception &e)
    {
        log_file << "An error occured while trying to run the python file\n";
    }
}

/////////////////////////////////////////////////////
///////// Alias Creation and Initialization  ///////
////////////////////////////////////////////////////

/**
 * create_fast_sensor_access
 * \brief: Following the Aldabaran FastGetSet module, we can read and write sensor
 *         and actuator vals in 10ms. This method sets up the fast DCM memory access
 **/
void hal_experimental::create_fast_sensor_access()
{
    LOG("[fast_mem_init]", "Fast Sensor Access initializing...");
    fSensorKeys.clear();
    //  Here as an example inertial + joints + FSR are read
    fSensorKeys.resize(25 + 7 + 6);
    for(int i = 0; i < NumOfPositionActuatorIds; ++i)
    {
        fSensorKeys[i] = sensorNames[i];
    }
    // fSensorKeys.resize(7 + 25 + 6);
    // // Joints Sensor list
    // fSensorKeys[HEAD_PITCH]       = std::string("Device/SubDeviceList/HeadPitch/Position/Sensor/Value");
    // fSensorKeys[HEAD_YAW]         = std::string("Device/SubDeviceList/HeadYaw/Position/Sensor/Value");
    // fSensorKeys[L_ANKLE_PITCH]    = std::string("Device/SubDeviceList/LAnklePitch/Position/Sensor/Value");
    // fSensorKeys[L_ANKLE_ROLL]     = std::string("Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value");
    // fSensorKeys[L_ELBOW_ROLL]     = std::string("Device/SubDeviceList/LElbowRoll/Position/Sensor/Value");
    // fSensorKeys[L_ELBOW_YAW]      = std::string("Device/SubDeviceList/LElbowYaw/Position/Sensor/Value");
    // fSensorKeys[L_HAND]           = std::string("Device/SubDeviceList/LHand/Position/Sensor/Value");
    // fSensorKeys[L_HIP_PITCH]      = std::string("Device/SubDeviceList/LHipPitch/Position/Sensor/Value");
    // fSensorKeys[L_HIP_ROLL]       = std::string("Device/SubDeviceList/LHipRoll/Position/Sensor/Value");
    // fSensorKeys[L_HIP_YAW_PITCH]  = std::string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value");
    // fSensorKeys[L_KNEE_PITCH]     = std::string("Device/SubDeviceList/LKneePitch/Position/Sensor/Value");
    // fSensorKeys[L_SHOULDER_PITCH] = std::string("Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value");
    // fSensorKeys[L_SHOULDER_ROLL]  = std::string("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value");
    // fSensorKeys[L_WRIST_YAW]      = std::string("Device/SubDeviceList/LWristYaw/Position/Sensor/Value");
    // fSensorKeys[R_ANKLE_PITCH]    = std::string("Device/SubDeviceList/RAnklePitch/Position/Sensor/Value");
    // fSensorKeys[R_ANKLE_ROLL]     = std::string("Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value");
    // fSensorKeys[R_ELBOW_ROLL]     = std::string("Device/SubDeviceList/RElbowRoll/Position/Sensor/Value");
    // fSensorKeys[R_ELBOW_YAW]      = std::string("Device/SubDeviceList/RElbowYaw/Position/Sensor/Value");
    // fSensorKeys[R_HAND]           = std::string("Device/SubDeviceList/RHand/Position/Sensor/Value");
    // fSensorKeys[R_HIP_PITCH]      = std::string("Device/SubDeviceList/RHipPitch/Position/Sensor/Value");
    // fSensorKeys[R_HIP_ROLL]       = std::string("Device/SubDeviceList/RHipRoll/Position/Sensor/Value");
    // fSensorKeys[R_KNEE_PITCH]     = std::string("Device/SubDeviceList/RKneePitch/Position/Sensor/Value");
    // fSensorKeys[R_SHOULDER_PITCH] = std::string("Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value");
    // fSensorKeys[R_SHOULDER_ROLL]  = std::string("Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value");
    // fSensorKeys[R_WRIST_YAW]      = std::string("Device/SubDeviceList/RWristYaw/Position/Sensor/Value");


    //TODO: Update this to use our enum
    // // Inertial sensors
    fSensorKeys[ACC_X]   = std::string("Device/SubDeviceList/InertialSensor/AccX/Sensor/Value");
    fSensorKeys[ACC_Y]   = std::string("Device/SubDeviceList/InertialSensor/AccY/Sensor/Value");
    fSensorKeys[ACC_Z]   = std::string("Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value");
    fSensorKeys[GYR_X]   = std::string("Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value");
    fSensorKeys[GYR_Y]   = std::string("Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value");
    fSensorKeys[ANGLE_X] = std::string("Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value");
    fSensorKeys[ANGLE_Y] = std::string("Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value");

    // Some FSR sensors
    fSensorKeys[L_COP_X]        = std::string("Device/SubDeviceList/LFoot/FSR/CenterOfPressure/X/Sensor/Value");
    fSensorKeys[L_COP_Y]        = std::string("Device/SubDeviceList/LFoot/FSR/CenterOfPressure/Y/Sensor/Value");
    fSensorKeys[L_TOTAL_WEIGHT] = std::string("Device/SubDeviceList/LFoot/FSR/TotalWeight/Sensor/Value");
    fSensorKeys[R_COP_X]        = std::string("Device/SubDeviceList/RFoot/FSR/CenterOfPressure/X/Sensor/Value");
    fSensorKeys[R_COP_Y]        = std::string("Device/SubDeviceList/RFoot/FSR/CenterOfPressure/Y/Sensor/Value");
    fSensorKeys[R_TOTAL_WEIGHT] = std::string("Device/SubDeviceList/RFoot/FSR/TotalWeight/Sensor/Value");

    // Create the fast memory access
    fMemoryFastAccess->ConnectToVariables(getParentBroker(), fSensorKeys, false);

    LOG("[fast_mem_init]","Fast Sensor Access initialized");
}

/**
  * \brief: Creates an alias for all the position actuators.
  **/
void hal_experimental::create_actuator_position_aliases()
{
    LOG("[create_actuator_pos]","create actuator position initializing...");

    AL::ALValue jointAliases;

    jointAliases.arraySetSize(2);
    jointAliases[0] = std::string("jointActuators");
    jointAliases[1].arraySetSize(NumOfPositionActuatorIds);
    for(int i = 0; i < NumOfPositionActuatorIds; ++i)
    {
        jointAliases[1][i] = std::string(actuatorNames[i]);
    }


    // Joints actuator list
    //  jointAliases[1].arraySetSize(25);
    // jointAliases[1][HEAD_PITCH]       = std::string("Device/SubDeviceList/HeadPitch/Position/Actuator/Value");
    // jointAliases[1][HEAD_YAW]         = std::string("Device/SubDeviceList/HeadYaw/Position/Actuator/Value");
    // jointAliases[1][L_ANKLE_PITCH]    = std::string("Device/SubDeviceList/LAnklePitch/Position/Actuator/Value");
    // jointAliases[1][L_ANKLE_ROLL]     = std::string("Device/SubDeviceList/LAnkleRoll/Position/Actuator/Value");
    // jointAliases[1][L_ELBOW_ROLL]     = std::string("Device/SubDeviceList/LElbowRoll/Position/Actuator/Value");
    // jointAliases[1][L_ELBOW_YAW]      = std::string("Device/SubDeviceList/LElbowYaw/Position/Actuator/Value");
    // jointAliases[1][L_HAND]           = std::string("Device/SubDeviceList/LHand/Position/Actuator/Value");
    // jointAliases[1][L_HIP_PITCH]      = std::string("Device/SubDeviceList/LHipPitch/Position/Actuator/Value");
    // jointAliases[1][L_HIP_ROLL]       = std::string("Device/SubDeviceList/LHipRoll/Position/Actuator/Value");
    // jointAliases[1][L_HIP_YAW_PITCH]  = std::string("Device/SubDeviceList/LHipYawPitch/Position/Actuator/Value");
    // jointAliases[1][L_KNEE_PITCH]     = std::string("Device/SubDeviceList/LKneePitch/Position/Actuator/Value");
    // jointAliases[1][L_SHOULDER_PITCH] = std::string("Device/SubDeviceList/LShoulderPitch/Position/Actuator/Value");
    // jointAliases[1][L_SHOULDER_ROLL]  = std::string("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Value");
    // jointAliases[1][L_WRIST_YAW]      = std::string("Device/SubDeviceList/LWristYaw/Position/Actuator/Value");
    // jointAliases[1][R_ANKLE_PITCH]    = std::string("Device/SubDeviceList/RAnklePitch/Position/Actuator/Value");
    // jointAliases[1][R_ANKLE_ROLL]     = std::string("Device/SubDeviceList/RAnkleRoll/Position/Actuator/Value");
    // jointAliases[1][R_ELBOW_ROLL]     = std::string("Device/SubDeviceList/RElbowRoll/Position/Actuator/Value");
    // jointAliases[1][R_ELBOW_YAW]      = std::string("Device/SubDeviceList/RElbowYaw/Position/Actuator/Value");
    // jointAliases[1][R_HAND]           = std::string("Device/SubDeviceList/RHand/Position/Actuator/Value");
    // jointAliases[1][R_HIP_PITCH]      = std::string("Device/SubDeviceList/RHipPitch/Position/Actuator/Value");
    // jointAliases[1][R_HIP_ROLL]       = std::string("Device/SubDeviceList/RHipRoll/Position/Actuator/Value");
    // jointAliases[1][R_KNEE_PITCH]     = std::string("Device/SubDeviceList/RKneePitch/Position/Actuator/Value");
    // jointAliases[1][R_SHOULDER_PITCH] = std::string("Device/SubDeviceList/RShoulderPitch/Position/Actuator/Value");
    // jointAliases[1][R_SHOULDER_ROLL]  = std::string("Device/SubDeviceList/RShoulderRoll/Position/Actuator/Value");
    // jointAliases[1][R_WRIST_YAW]      = std::string("Device/SubDeviceList/RWristYaw/Position/Actuator/Value");
    try
    {
        dcm_proxy->createAlias(jointAliases);
        log_file << "position alias added to DCM!\n";

    }
    catch(const AL::ALError &e)
    {
        LOG("[create_actuaor_pos]","[ERROR]: Error creating actuator position aliases: " + e.toString());
    }

    LOG("[create_actuator_pos]","create actuator position initialized");
}

/**
  * \brief: Initialize the previously created joint position aliases
  **/
void hal_experimental::init_actuator_position_aliases()
{
    LOG("[init_actuator_pos]", "Initializing the Alias for actuator positions...");

    commands.arraySetSize(6);
    commands[0] = std::string("jointActuators");
    commands[1] = std::string("ClearAll");
    commands[2] = std::string("time-separate");
    commands[3] = 0; //Aldabaran never implemented this
    commands[4].arraySetSize(1);
    commands[5].arraySetSize(25);
    for(int i =0; i < 25; ++i)
    {
        commands[5][i].arraySetSize(1);
    }
    LOG("[init_actuator_pos]","Done!");
}
/**
  * \brief: Create the aliases for controlling the joint stiffnesses.
  **/
void hal_experimental::create_actuator_stiffness_aliases()
{
    LOG("[create_actuator_stiffness]", "create actuator stiffness initializing...");

    AL::ALValue jointAliases;
    try
    {
        jointAliases.clear();
        jointAliases.arraySetSize(2);
        jointAliases[0] = std::string("jointStiffness");
        jointAliases[1].arraySetSize(25);
        int j = 0;
        for(int i = NumOfPositionActuatorIds; i <= rAnkleRollStiffnessActuator; ++i)
        {
            jointAliases[1][j] = actuatorNames[i];
            ++j;
        }
        // jointAliases[1][HEAD_PITCH]        = std::string("Device/SubDeviceList/HeadPitch/Hardness/Actuator/Value");
        // jointAliases[1][HEAD_YAW]          = std::string("Device/SubDeviceList/HeadYaw/Hardness/Actuator/Value");
        // jointAliases[1][L_ANKLE_PITCH]     = std::string("Device/SubDeviceList/LAnklePitch/Hardness/Actuator/Value");
        // jointAliases[1][L_ANKLE_ROLL]      = std::string("Device/SubDeviceList/LAnkleRoll/Hardness/Actuator/Value");
        // jointAliases[1][L_ELBOW_ROLL]      = std::string("Device/SubDeviceList/LElbowRoll/Hardness/Actuator/Value");
        // jointAliases[1][L_ELBOW_YAW]       = std::string("Device/SubDeviceList/LElbowYaw/Hardness/Actuator/Value");
        // jointAliases[1][L_HAND]            = std::string("Device/SubDeviceList/LHand/Hardness/Actuator/Value");
        // jointAliases[1][L_HIP_PITCH]       = std::string("Device/SubDeviceList/LHipPitch/Hardness/Actuator/Value");
        // jointAliases[1][L_HIP_ROLL]        = std::string("Device/SubDeviceList/LHipRoll/Hardness/Actuator/Value");
        // jointAliases[1][L_HIP_YAW_PITCH]   = std::string("Device/SubDeviceList/LHipYawPitch/Hardness/Actuator/Value");
        // jointAliases[1][L_KNEE_PITCH]      = std::string("Device/SubDeviceList/LKneePitch/Hardness/Actuator/Value");
        // jointAliases[1][L_SHOULDER_PITCH]  = std::string("Device/SubDeviceList/LShoulderPitch/Hardness/Actuator/Value");
        // jointAliases[1][L_SHOULDER_ROLL]   = std::string("Device/SubDeviceList/LShoulderRoll/Hardness/Actuator/Value");
        // jointAliases[1][L_WRIST_YAW]       = std::string("Device/SubDeviceList/LWristYaw/Hardness/Actuator/Value");
        // jointAliases[1][R_ANKLE_PITCH]     = std::string("Device/SubDeviceList/RAnklePitch/Hardness/Actuator/Value");
        // jointAliases[1][R_ANKLE_ROLL]      = std::string("Device/SubDeviceList/RAnkleRoll/Hardness/Actuator/Value");
        // jointAliases[1][R_ELBOW_ROLL]      = std::string("Device/SubDeviceList/RElbowRoll/Hardness/Actuator/Value");
        // jointAliases[1][R_ELBOW_YAW]       = std::string("Device/SubDeviceList/RElbowYaw/Hardness/Actuator/Value");
        // jointAliases[1][R_HAND]            = std::string("Device/SubDeviceList/RHand/Hardness/Actuator/Value");
        // jointAliases[1][R_HIP_PITCH]       = std::string("Device/SubDeviceList/RHipPitch/Hardness/Actuator/Value");
        // jointAliases[1][R_HIP_ROLL]        = std::string("Device/SubDeviceList/RHipRoll/Hardness/Actuator/Value");
        // jointAliases[1][R_KNEE_PITCH]      = std::string("Device/SubDeviceList/RKneePitch/Hardness/Actuator/Value");
        // jointAliases[1][R_SHOULDER_PITCH]  = std::string("Device/SubDeviceList/RShoulderPitch/Hardness/Actuator/Value");
        // jointAliases[1][R_SHOULDER_ROLL]   = std::string("Device/SubDeviceList/RShoulderRoll/Hardness/Actuator/Value");
        // jointAliases[1][R_WRIST_YAW]       = std::string("Device/SubDeviceList/RWristYaw/Hardness/Actuator/Value");

    }
    catch(const AL::ALError &e)
    {
        std::cout << "[create_actuator_stiffness][ERROR]: Error setting up the aliase: " << e.toString() << std::endl;
    }

    try
    {
        dcm_proxy->createAlias(jointAliases);
        log_file << "stiffness alias added to DCM!\n";
    }
    catch(const AL::ALError &e)
    {
        LOG("[create_actuator_stiffness]","[ERROR]: An error occured while creating stiffness actuator aliases: " + e.toString());
    }
    LOG("[create_actuator_stiffness]","create actuator stiffness initialized");
}

/**
  * \brief: Set the stiffness value for every joint on the robot to a single value
  **/
void hal_experimental::set_all_actuator_stiffnesses(const float &stiffnessVal)
{
    log_file << "[set_all_stiffness]: << Setting All stiffness values to: " << stiffnessVal << std::endl;

    AL::ALValue stiffnessCommands;
    int DCMTime;

    try
    {
        DCMTime = dcm_proxy->getTime(1000); //Time NAOQi has been running + 1000ms
    }
    catch(const AL::ALError &e)
    {
        LOG("[set_all_stiffness]","[ERROR] An error occured while setting stiffness value: " + e.toString());
    }

    stiffnessCommands.arraySetSize(3);
    stiffnessCommands[0] = std::string("jointStiffness");
    stiffnessCommands[1] = std::string("Merge");
    stiffnessCommands[2].arraySetSize(1);
    stiffnessCommands[2][0].arraySetSize(2);
    stiffnessCommands[2][0][0] = stiffnessVal;
    stiffnessCommands[2][0][1] = DCMTime;

    log_file << "[set_all_stiffness] Alias set up..trying to set it now." << std::endl;
    std::cout << "[set_all_stiffness] Alias set up..trying to set it now." << std::endl;
    try
    {
        dcm_proxy->set(stiffnessCommands);
        LOG("[set_all_stiffness]","Stiffness Values Set!");
    }
    catch(const AL::ALError &e)
    {
        LOG("[set_all_stiffness]","[ERROR] An error occured while setting stiffness value: " + e.toString());
    }

    log_file << "[set_all_stiffness]: Done Setting all stiffness values to: " << stiffnessVal << std::endl;
}

///////////////////////////////////////////////////
/////////// End Alias Initialization //////////////
///////////////////////////////////////////////////




/////////////////////////////////////////////////////////
/////////// Interprocess Sync and Hardware IO ///////////
/////////////////////////////////////////////////////////

/**!!!! NOTE !!!!!
* If any of the interprocess semaphore wait/post calls take
* more than 10ms, the DCM will crash and the robot will probably 
* fall over or have a seizure or something. I honestly dont know but 
* it'll be funny (not in competition though)...
**/

/**
  * \brief: Reads new values from interprocess memory and attempts to set them
  **/
void hal_experimental::update_actuator_values()
{
    // First get the current time since NAOqi was started from the real time clock
    int current_dcm_time;
    try
    {
        current_dcm_time = dcm_proxy->getTime(0);
    }
    catch(const AL::ALError &e)
    {
        LOG("[actuator_joint_test]", "[ERROR]: Error accessing DCM: " + e.toString());
    }
    try
    {
        // Real time value for setting the actuators immediately.
        commands[4][0] = current_dcm_time;
        //Ensure every joint has a value, the obvious choice is its current sensor value.
        for(int i = 0; i < 25; ++i)
        {
            commands[5][i][0] = initialJointSensorValues[i];
        }

        // Use our semaphore for propery interprocess syncing of shared memory
        // while(!pineappleJuice->actuator_semaphore.try_wait());
        //TODO: Switch this to NumOfActuatorIds when thats all implemented
        sem_wait(semaphore);
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        {
            commands[5][i][0] = pineappleJuice->actuator_values[i];
        }
        //pineappleJuice->actuator_semaphore.post();
    }
    catch(const AL::ALError &e)
    {
        LOG("[actuator_joint_test]", "[DCM POST LOOP][ERROR] An error occurred in the post DCM callback loop: " + e.toString());
    }
    sem_post(semaphore);


    //Tell the DCM the values we want to set the actuators to
    try
    {
        dcm_proxy->setAlias(commands);
    } 
    catch(const AL::ALError &e)
    {
        LOG("[actuator_joint_test]", "[ERROR]: Error setting dcm alias: " + e.toString());
    }
}

/**
  * \brief: Checks shared memory for text to speak. If the last text was already spoken, if not it says it.
  **/
void hal_experimental::update_text_to_speak()
{
    try
    {
        // pineappleJuice->speak_semaphore.wait();
        // if(std::strcmp(last_spoken_text, pineappleJuice->text_to_speak_unsafe) == 0)
        // {
        //     SAY(pineappleJuice->text_to_speak_unsafe);
        //     //PLZ DO NOT EXPLOIT ROBOT
        //     std::strncpy(last_spoken_text, pineappleJuice->text_to_speak_unsafe, 34);
        // }
        // pineappleJuice->speak_semaphore.post();
    }
    catch(const std::exception &e)
    {
        log_file << "[update_text_to_speak][ERROR]: An error has occured while updating text to speak! " << e.what() << std::endl;
    }
}

/**
  * \brief: Updates the sensor values in shared memory for NAOInterface
  **/
void hal_experimental::update_sensor_values()
{
    try
    {
        fMemoryFastAccess->GetValues(sensorValues);
    }
    catch(const AL::ALError &e)
    {
        LOG("update_sensor_vals][ERROR]","Error, could not read sensor values: " + e.toString());
    }

    try
    {
        //pineappleJuice->sensor_semaphore.wait();
        //sensor_semaphore.wait();
        sem_wait(semaphore);
        for(int i = 0; i < sensorValues.size(); ++i)
        {
            pineappleJuice->sensor_values[i] = sensorValues[i];
        }
        //sensor_semaphore.post();
        //pineappleJuice->sensor_semaphore.post();
    }
    catch(const boost::interprocess::interprocess_exception &e)
    {
        log_file << "[update_sensor_vals][ERROR]: An interprocess error has occured: " << e.what() << std::endl;
    }
    sem_post(semaphore);
}


/////////////////////////////////////////////////////////
/////////////////////  End Sync /////////////////////////
////////////////////////////////////////////////////////




///////////////////////////////////////////////////
/////////////////  Tests   ////////////////////////
///////////////////////////////////////////////////

void hal_experimental::set_LEDS()
{
    //This is causing a segfault

    for(int i = faceLedRedLeft0DegActuator; i <= faceLedBlueRight315DegActuator; ++i)
        actuators[i] = 0.f;
    // LED Debug stuff can go here. Blinky eyes from rUNSWIFt
    float blink_val = float(dcm_proxy->getTime(0) / 500 & 1);
    actuators[faceLedGreenRight180DegActuator] =  blink_val;
    actuators[faceLedGreenLeft180DegActuator] = blink_val;
    //log_file << "LEDS were set" << std::endl;
}

//The outline for a great state machine.
float* hal_experimental::robot_state_handler(float *actuator_vals)
{
    log_file << "Robot state handler called \n";
    return actuator_vals;
}

/** \brief: actuator_joint_test: An example of directly reading sensor values and writing actuator values
  *  In this example, any movement on the left arm is mimicked by the right arm
  */

void hal_experimental::actuator_joint_test()
{
    // First get the current time since NAOqi was started from the real time clock
    int current_dcm_time;
    try
    {
        current_dcm_time = dcm_proxy->getTime(0);
    }
    catch(const AL::ALError &e)
    {
        LOG("[actuator_joint_test]", "[ERROR]: Error accessing DCM: " + e.toString());
    }
    //Time for the next cycle. Note there needs to be 10ms between sets,
    //however this ammount of time should happen naturally.
    try
    {
        // Real time value for setting the actuators immediately.
        commands[4][0] = current_dcm_time;
        // Ensure that we have the initial joint values. This should only occur once, 
        // and will probably be moved elsewhere.
        if(initialJointSensorValues.size() == 0)
        {
            for(int i = 0; i < 25; ++i)
                initialJointSensorValues.push_back(sensorValues[i]);
        }

        // Set stiffness of all joints to a low value before attempting 
        // to set a new actuator value 
        for(int i = 0; i < 25; ++i)
        {
            commands[5][i][0] = initialJointSensorValues[i];
        }
        fMemoryFastAccess->GetValues(sensorValues);
        //Test: Sensor values...
        // std::cout << "[ShoulderPitchPositionActuator] RIGHT =  " <<  sensorValues[rShoulderPitchPositionSensor] << " | LEFT = " << sensorValues[lShoulderPitchPositionSensor]  << std::endl;
        // std::cout << "[ShoulderRollPositionActuator]: RIGHT = " << sensorValues[rShoulderRollPositionSensor] << " | LEFT = " << sensorValues[lShoulderRollPositionSensor] << std::endl;
        // std::cout << "[ElbowYawPositionActuator]: RIGHT = " << sensorValues[rElbowRollPositionSensor] << " | LEFT = " <<  sensorValues[lElbowRollPositionSensor] << std::endl;
        // std::cout << "[ElbowRollPositionActuator]: RIGHT = " << sensorValues[rElbowRollPositionSensor] << " | LEFT = " << sensorValues[lElbowRollPositionSensor] <<  std::endl;
        
        //!!IMPORTANT!! commands[5][..PositionACTUATOR][0] = sensorValues[..PositionSENSOR];
        commands[5][lShoulderPitchPositionActuator][0] =  sensorValues[rShoulderPitchPositionSensor];
        commands[5][lShoulderRollPositionActuator][0]  = -sensorValues[rShoulderRollPositionSensor];
        commands[5][lElbowYawPositionActuator][0]      = -sensorValues[rElbowYawPositionSensor];
        commands[5][lElbowRollPositionActuator][0]     = -sensorValues[rElbowRollPositionSensor];
    }
    catch(const AL::ALError &e)
    {
        LOG("[actuator_joint_test]", "[DCM POST LOOP][ERROR] An error occurred in the post DCM callback loop: " + e.toString());
    }

    //Tell the DCM the values we want to set the actuators to
    try
    {
        dcm_proxy->setAlias(commands);
    } 
    catch(const AL::ALError &e)
    {
        LOG("[actuator_joint_test]", "[ERROR]: Error setting dcm alias: " + e.toString());
    }
}


// testLEDS: A simple (not really) DCM write test to change the colors of the chestLEDS
void hal_experimental::testLEDS()
{
    if(!testLEDInitialized)
    {
        try
        {
            commandsTestAlias.arraySetSize(2);
            commandsTestAlias[0] = std::string("ChestLeds");
            commandsTestAlias[1].arraySetSize(3);
            commandsTestAlias[1][0] = std::string("ChestBoard/Led/Red/Actuator/Value");
            commandsTestAlias[1][1] = std::string("ChestBoard/Led/Green/Actuator/Value");
            commandsTestAlias[1][2] = std::string("ChestBoard/Led/Blue/Actuator/Value");
            dcm_proxy->createAlias(commandsTestAlias);
            std::cout << "test LEDs aliass created" << std::endl;
            commandsTest.arraySetSize(4);
            commandsTest[0] = std::string("ChestLeds");
            commandsTest[1] = std::string("ClearAll");
            commandsTest[2] = std::string("time-mixed");
            //commandsTest[3].arraySetSize(3);
        }
        catch (const AL::ALError &e) {
            log_file << "Oh damn there was an error with the testLEDS: " << e.what() << "\n";
        }
        testLEDInitialized = true;
    }
       
    try
    {
        // std::cout << "Setting chest LEDS" << std::endl;
		commandsTest[3].arraySetSize(3);
		if (testLEDRot % 3 == 0 && (dcm_proxy->getTime(0) - lastTime > 2000)) {
            //ChestBoard/Led/Red/Actuator/Value
            commandsTest[3][0].arraySetSize(2);
            commandsTest[3][0][0].arraySetSize(2);

            commandsTest[3][0][0][0] = 1.0;
            commandsTest[3][0][0][1] = dcm_proxy->getTime(0);
            
            commandsTest[3][0][1].arraySetSize(2);
            commandsTest[3][0][1][0] = 0.0;
            commandsTest[3][0][1][1] = dcm_proxy->getTime(2000);

            lastTime = dcm_proxy->getTime(0);
            // std::cout << "Red value should be set\n";
        } else if (testLEDRot % 3 == 1 && (dcm_proxy->getTime(0) - lastTime > 2000)) {
            //ChestBoard/Led/Green/Actuator/Value
            commandsTest[3][1].arraySetSize(2);
            commandsTest[3][1][0].arraySetSize(2);

            commandsTest[3][1][0][0] = 1.0;
            commandsTest[3][1][0][1] = dcm_proxy->getTime(300);

            commandsTest[3][1][1].arraySetSize(2);
            commandsTest[3][1][1][0] = 0.0;
            commandsTest[3][1][1][1] = dcm_proxy->getTime(2500);
            
            // commandsTest[3][1][0].arraySetSize(2);
            // commandsTest[3][1][1][0] = 0.0;
            // commandsTest[3][1][1][0] = dcm_proxy->getTime(2200);


            lastTime = dcm_proxy->getTime(0);
            // std::cout << "Green value should be set\n";
        } else if (testLEDRot % 3 == 2 && (dcm_proxy->getTime(0) - lastTime > 2000)) { 
            //ChestBoard/Led/Blue/Actuator/Value
            commandsTest[3][2].arraySetSize(2);
            commandsTest[3][2][0].arraySetSize(2);

            commandsTest[3][2][0][0] = 1.0;
            commandsTest[3][2][0][1] = dcm_proxy->getTime(600);

            commandsTest[3][2][1].arraySetSize(2);
            commandsTest[3][2][1][0] = 0.0;
            commandsTest[3][2][1][1] = dcm_proxy->getTime(3000);

            // commandsTest[3][2][0].arraySetSize(2);
            // commandsTest[3][2][1][0] = 0.0;
            // commandsTest[3][2][1][0] = dcm_proxy->getTime(2400);

            lastTime = dcm_proxy->getTime(0);
            // std::cout << "Blue value should be set\n";
    //     SAY("My chest LEDS should be doing stuff");
        }
        dcm_proxy->setAlias(commandsTest);
        testLEDRot++;
        // std::cout << "Chest LEDS should have been set" << std::endl;
       } catch(const AL::ALError &e) {
           std::cout << "LED setting error occured: " << e.what() << std::endl;
           log_file << "LED setting error occured: " << e.what() << "\n";
       }
}

////////////////////////////////////
///////// End Tests ////////////////
////////////////////////////////////


/**
  * \brief: A silly helper function that both prints strings to iostream and a log file because I'm lazy
  **/
void hal_experimental::LOG(const std::string function, const std::string message)
{
    std::cout << function << ": " << message << std::endl;
    log_file << function << ": " << message << std::endl; 
}

/**
  * \brief: Returns an approximated value for degrees to radians conversion
  **/
float hal_experimental::deg2rad(float degrees)
{
    return degrees * 0.0174533;
}

//Hooks for NaoQI
extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
    AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(pBroker);
    AL::ALModule::createModule<hal_experimental>(pBroker, "hal_experimental");
    return 0;
}

extern "C" int _closeModule()
{
    return 0;
}
