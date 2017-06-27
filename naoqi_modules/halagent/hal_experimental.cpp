#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;


// Macro to make the NAO say stuff. Use carefully (if spammed it'll crash the DCM also its really damn annoying)
#define SAY(text) speak_proxy->post.say(text); 
//const std::string hal_experimental::name("hal_experimental");
hal_experimental* hal_experimental::instance = NULL;
hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        fMemoryFastAccess(boost::shared_ptr<AL::ALMemoryFastAccess>(new AL::ALMemoryFastAccess())),
        nao_memory_proxy(NULL),
        speak_proxy(NULL),
        ledIndex(0),
        ////DEBUG STUFF
        cout_debug(true),
        testLEDInitialized(false),
        testAliasesInitialized(false),
        testLEDRot(0),
        mult(1)
{
    setModuleDescription("Communicates between the Naoqi process and our Pineapple");
    last_reading_actuator = 255; ///<This is just initially set to an impossible actuator number
    actuator_update_fails = 0;
    std::cout << "Starting hal_experimental!" << std::endl;
    try
    {
        //Debug
       log_file.open("/home/nao/NAOQI-LOG2.log", std::ios::out);
       log_file << "Initializing NAOQI" << '\n';
       log_file << "Another log file test" << '\n';
    } catch(...) {
        SAY("I could't use the LOG File");
    }

    try
    {
        log_file << "Initializing Shared Memory with Boost.Interprocess. [PineappleJuice]\n";
        shm = boost::interprocess::managed_shared_memory(open_or_create, "PineappleJuice", 65536); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
        if(shm.get_size() != 65536) 
            log_file << "ERROR: Did not allocate enough memory. SHM size: " << shm.get_size() << "\n";
        
        pineappleJuice = shm.find_or_construct<hal_data>("juicyData")(/*Constructor, assuming use of default constructor*/);
        shared_data_ptr = shm.find<hal_data>("juicyData");
        if(!shared_data_ptr.first)
        {
            log_file << "[FATAL] juicyData failed to be found" << '\n';
            return;
        }
        pineappleJuice = shared_data_ptr.first; ///< Dumb hack to stop boost interprocess from causing segfaults
        std::cout << "pineappleJuice created in shared memory" << std::endl;
        log_file << "pineappleJuice created in shared memory" << '\n';
    } 
    catch(boost::interprocess::interprocess_exception &e) 
    {
        log_file << "[FATAL] An Interprocess error: " << e.what() << '\n';
        std::cout << "Cleaning up shared memory..." << std::endl;
        log_file << "Shared Memory failed...cleaning up" << std::endl;
        shm.destroy<hal_data>("juicyData");
        boost::interprocess::shared_memory_object::remove("PineappleJuice");
    }


    //HardwareMap hm;
    /** \Brief: This part is complicated and you really need to shouldn't care about it.
      * 
      * Inside this try block we are doing quite a few things. To summarize, we establish communication
      * proxies with NAOQi, then we begin creating AL::Aliases to (questionably) more effeciently
      * send values to the DCM.
      **/
    try
    {
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


	} catch(AL::ALError &e) {
        log_file << "[ERROR] Fatal Error: Could Not Initialize Interface with NaoQi due to: "<< e.what() << "\n";
    }
    //Boost Interprocess Shared Memory Initialization:
        // log_file << "Testing actuator semaphore wait...\n";
        // pineappleJuice->actuator_semaphore.wait();
        // //dcm_time = dcm_proxy->getTime(0); ///< Get's current time on NAO
        // log_file << "NO SEGFAULT: DCM Time = " << dcm_time << "\n";
        // log_file << "woohoo I'm in a semaphore" << '\n';
        // pineappleJuice->actuator_semaphore.post();
        

        for(int i = 0; i < NumOfActuatorIds; ++i)
            actuators[i] = 0.f;

    // SAY("I will say my IP Address in a moment. It's just, well Linux hates me.");
    // get_ip_address();
    //lastTime = dcm_proxy->getTime(0);


    pBrokerCopy = pBroker;
    log_file << "[connectToDCMLoop] connecting to DCM Loop" << std::endl;
    std::cout << "[connectToDCMLoop] begconnecting to DCM Loop" << std::endl;
    try {
        //dcm_proxy->getGenericProxy()->getModule()->atPreProcess(boost::bind(&hal_experimental::preCallBack, this));
        //dcm_proxy->getGenericProxy()->getModule()->atPostProcess(boost::bind(&hal_experimental::postCallBack, this));
//        fDCMPostProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(boost::bind(&FastGetSetDCM::synchronisedDCMcallback, this));
        std::cout << "Connected to dcm! Trying to connect fast memory..." << std::endl;
       // fMemoryFastAccess->ConnectToVariables(pBroker, fSensorKeys, false);
        // std::cout << "Fast Memory Connection Established!" << std::endl;
        //SAY("Wooho I am ready");
        std::cout << "Woohoo I am ready!" << std::endl;
    } catch (std::exception &e) {
        log_file << "ERROR: dcm proxy error: " << e.what() << "\n";
    }
    log_file << "7. Proxy call backs established.\n";

    std::cout << "Everything Initialized!" << std::endl;


    //Stuff for NAOQi
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
    startLoop();
    std::cout << "hal_experimental Fully Initialized!" << std::endl;

}
hal_experimental::~hal_experimental()
{
    SAY("Time to die.");
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
    log_file << "My journey is over. **Kills self**\n";
    log_file << "Narator: And that is how the tragic life of the dumb robot ended." << std::endl;
    log_file.close();
    //delete dcm_proxy;
    delete nao_memory_proxy;
    delete speak_proxy;
}

void hal_experimental::initialize_everything()
{
    log_file << "[init_everything]: Creating Aliases..." << std::endl;
    std::cout << "[init_everything]: Creating Aliases..." << std::endl;
    create_fast_sensor_access();
    create_actuator_position_aliases();
    create_actuator_stiffness_aliases();
    log_file << "[init_everything]: Creating Aliases..." << std::endl;
    std::cout << "[init_everything]: All aliases created!" << std::endl;
    set_all_actuator_stiffnesses(0.2f);
    init_actuator_position_aliases();
    log_file << "[init_everything]: Done!" << std::endl;
    std::cout << "[init_everything]: Done!" << std::endl;
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
        std::cout << "[startloop]: DCM proxy connection established!" << std::endl;
        log_file << "[startloop]: DCM proxy connection established!" << std::endl;
    }
    catch(const AL::ALError &e)
    {
        std::cout << "[startloop][ERROR]: Error getting dcmProxy: " << e.what() << std::endl;
        log_file << "[startloop][ERROR]: Error getting dcmProxy: " << e.what() << std::endl;
    }

    signed long isDCMRunning;
    try
    {
        isDCMRunning = getParentBroker()->getProxy("ALLauncher")->call<bool>("isModulePresent", std::string("DCM"));
        std::cout << "[startloop]: DCM is running!" << std::endl;
        log_file << "[startloop]: DCM is running!" << std::endl;
    }
    catch (AL::ALError& e)
    {
        std::cout << "[startLoop][ERROR] Error when connecting to DCM : " << e.what() << std::endl;
        log_file << "[startLoop][ERROR] Error when connecting to DCM : " << e.what() << std::endl;

    }

    if (!isDCMRunning)
    {
        std::cout << "[startLoop][ERROR]: Error no DCM running!" << std::endl;
        log_file << "[startLoop][ERROR]: Error no DCM running!" << std::endl;
    }

    initialize_everything();
    connectToDCMLoop();
}
void hal_experimental::stopLoop()
{
    set_all_actuator_stiffnesses(0.0f);
    fDCMPreProcessConnection.disconnect();
    fDCMPostProcessConnection.disconnect();
    std::cout << "[stopLoop]: Real time hooks disconnected!" << std::endl;
    log_file << "[startLoop]: Real time hooks disconnected!" << std::endl;
}
/** 
* The method is called by NaoQi immediately before it communicates with the chest board.
* It sets all the actuators.
*/
void hal_experimental::connectToDCMLoop()
{

    //UNCOMMENT THIS 
    log_file << "[connectToDCMLoop] beginning fastMemoryAccess stuff" << std::endl;
    std::cout << "[connectToDCMLoop] beginning fastMemoryAccess stuff" << std::endl;
    try
    {
        // fMemoryFastAccess->ConnectToVariables(pBrokerCopy, fSensorKeys, false);
        // std::cout << "Fast Memory Connection Established!" << std::endl;

        fMemoryFastAccess->GetValues(sensorValues);
        log_file << "[connectToDCMLoop] Values Got with FastAccess: " << sensorValues.size() << std::endl;
        std::cout << "[connectToDCMLoop] Values Got with FastAccess: " << sensorValues.size() << std::endl;
        //DEBUG

        try
        {
            log_file << "[connectToDCMLoop] fSensorKeys size = " << fSensorKeys.size() << "Trying to print sensorValues: " << sensorValues.size() << std::endl;
            std::cout << "[connectToDCMLoop] fSensorKeys size = " << fSensorKeys.size() << "Trying to print sensorValues: " << sensorValues.size() << std::endl;
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
        std::cout << "Got sensor values with fast access! " << std::endl;
       // for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        for(int i = 0; i < 25; ++i)
        {
            initialJointSensorValues.push_back(sensorValues[i]);
            log_file << "[connectToDCMLoop] initialSensorValues[" << i << "] = " << sensorValues[i] << '\n';
            std::cout << "[connectToDCMLoop] initialSensorValues[" << i << "] = " << sensorValues[i] << '\n';
        }

        try
        {
            log_file << "[connectToDCMLoop] setting up pre/post process hooks..." << std::endl;
            std::cout << "[connectToDCMLoop] setting up pre/post process hooks..." << std::endl;

            //Connect our real time functions to the DCM
            fDCMPostProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(boost::bind(&hal_experimental::actuator_joint_test, this));
            fDCMPreProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPreProcess(boost::bind(&hal_experimental::testLEDS, this));
            
            log_file << "[connectToDCMLoop] pre/postProcess hook connected!" << std::endl;
            std::cout << "[connectToDCMLoop] pre/postProcess hook connected!" << std::endl;
        }
        catch(const AL::ALError &e)
        {

        }
    }
    catch(const AL::ALError &e)
    {
        std::cout << "Error while getting initial sensor values: " << e.toString() << std::endl;
    }
    log_file << "[connectToDCMLoop] Done!" << std::endl;
    std::cout << "[connectToDCMLoop] Done!" << std::endl;


}

/**
  * Real time function called before DCM I/O.
  * Primary use is for setting actuator
  **/
void hal_experimental::onPreProcess()
{

}

/**
  * Real time function called after DCM I/O.
  * Primary use for reading sensor values.
  **/

void hal_experimental::onPostProcess()
{

}

///////////////// END REAL TIME HOOKS //////////////////////

/**
* speak: Make the NAO say things (textToSpeech)
* This method is currently not really really race condition safe.
* Not my fault if the NAO starts speaking giberish (well actually 100% my fault but its halarious)
**/
void hal_experimental::speak()
{
    log_file << "Horrible unsafe speak method called\n";
    pineappleJuice->speak_semaphore.post();
    //See if there's anything new to talk about
    SAY("I've been asked to speak. I am mute.");
    if(pineappleJuice->text_to_speak_unsafe[0] != ' ' || pineappleJuice->text_to_speak_unsafe[0] != '0')
    {
        SAY(pineappleJuice->text_to_speak_unsafe);
         //Mark as already have talked. Prevents parrots from attacking our pineapples.
        pineappleJuice->text_to_speak_unsafe[0] = ' ';
    }
    log_file << "I have left the terrible unsafe, widely spoken lands" << std::endl;
   // pineappleJuice->speak_semaphore.post();
}
/**
* get_ip_address: Executes a shell command to retrieve the NAO's IP Address
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

    //SAY("I should have said my IP Address");
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
    std::cout << "[fast_mem_init]: Fast Sensor Access initializing..." << std::endl;
    log_file << "[fast_mem_init]: Fast Sensor Access initializing..." << std::endl;
    // fSensorKeys.clear();
    // fSensorKeys.resize(NumOfSensorIds);
    // for(int i = 0; i < NumOfSensorIds; ++i)
    // {
    //     fSensorKeys[i] = sensorNames[i];
    //     std::cout << "[fast_mem_init]: fSensorKeys[" << i << "] = " << sensorNames[i] << '\n';
    //     log_file << "[fast_mem_init]: fSensorKeys[" << i << "] = " << sensorNames[i] << '\n';

    // }
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

    std::cout << "[fast_mem_init]: Fast Sensor Access initialized" << std::endl;
    log_file << "[fast_mem_init]: Fast Sensor Access initialized" << std::endl;
}

/**
  * \brief: Creates an alias for all the position actuators.
  **/
void hal_experimental::create_actuator_position_aliases()
{
    std::cout << "[create_actuator_pos]: create actuator position initializing..." << std::endl;
    log_file << "[create_actuator_pos]: create actuator position initializing..." << std::endl;

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
        log_file << "[create_actuaor_pos][ERROR]: Error creating actuator position aliases: " << e.toString() << std::endl;
        std::cout << "[create_actuaor_pos][ERROR]: Error creating actuator position aliases: " << e.toString() << std::endl;
    }

    std::cout << "[create_actuator_pos]: create actuator position initialized" << std::endl;
    log_file << "[create_actuator_pos]: create actuator position initialized" << std::endl;
}

/**
  * \brief: Initialize the previously created joint position aliases
  **/
void hal_experimental::init_actuator_position_aliases()
{
    std::cout << "[init_actuator_pos]: Initializing the Alias for actuator positions..." << std::endl;
    log_file << "[init_actuator_pos]: Initializing the Alias for actuator positions..." << std::endl;
    commands.arraySetSize(6);
    LOG("init_actuator_pos", "Array size set to 6");
    commands[0] = std::string("jointActuators");
    commands[1] = std::string("ClearAll");
    commands[2] = std::string("time-separate");
    commands[3] = 0; //Aldabaran never implemented this

    commands[4].arraySetSize(1);
    
    // commands[5].arraySetSize(NumOfPositionActuatorIds);
    // for(int i = 0; i < NumOfPositionActuatorIds; ++i)
    // {
    //     commands[5][i].arraySetSize(1);
    // }
    commands[5].arraySetSize(25);
    for(int i =0; i < 25; ++i)
    {
        commands[5][i].arraySetSize(1);
    }
    std::cout << "[init_actuator_pos]: Done!" << std::endl;
    log_file << "[init_actuator_pos]: Done!" << std::endl;

}
/**
  * \brief: Create the aliases for controlling the joint stiffnesses.
  **/
void hal_experimental::create_actuator_stiffness_aliases()
{
    std::cout << "[create_actuator_stiffness]: create actuator stiffness initializing..." << std::endl;
    log_file << "[create_actuator_pos]: create actuator stiffness initializing..." << std::endl;

    // const int num_stiffness_actuators = rAnkleRollStiffnessActuator - headYawStiffnessActuator;
    // Recycle our last used alias
    AL::ALValue jointAliases;
    try
    {
        jointAliases.clear();
        jointAliases.arraySetSize(2);
        jointAliases[0] = std::string("jointStiffness");

        // jointAliases[1].arraySetSize(NumOfStiffnessActuatorIds);
        // for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        // {
        //     jointAliases[1][i] = actuatorNames[i];
        // }
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
        log_file << "[create_actuator_stiffness][ERROR]: An error occured while creating stiffness actuator aliases: " << e.toString() << std::endl;
        std::cout << "[create_actuator_stiffness][ERROR]: An error occured while creating stiffness actuator aliases: " << e.toString() << std::endl;
    }
    std::cout << "[create_actuator_stiffness]: create actuator stiffness initialized" << std::endl;
    log_file << "[create_actuator_stiffness]: create actuator stiffness initialized" << std::endl;
}

/**
  * \brief: Set the stiffness value for every joint on the robot to a single value
  **/
void hal_experimental::set_all_actuator_stiffnesses(const float &stiffnessVal)
{
    log_file << "[set_all_stiffness] Setting All stiffness values to: " << stiffnessVal << "\n";
    std::cout << "[set_all_stiffness] Setting All stiffness values to: " << stiffnessVal << "\n";
    AL::ALValue stiffnessCommands;
    int DCMTime;

    try
    {
        DCMTime = dcm_proxy->getTime(1000); //Time NAOQi has been running + 1000ms
    }
    catch(const AL::ALError &e)
    {
        log_file << "[set_all_stiffness][ERROR] An error occured while setting stiffness value: " << e.toString() << std::endl;
        std::cout << "[set_all_stiffness][ERROR] An error occured while setting stiffness value: " << e.toString() << std::endl;
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
        log_file << "[set_all_stiffness] Stiffness Values Set!" << std::endl;
        std::cout << "[set_all_stiffness] Stiffness Values Set!" << std::endl;
    }
    catch(const AL::ALError &e)
    {
        log_file << "[set_all_stiffness][ERROR] An error occured while setting stiffness value: " << e.toString() << std::endl;
        std::cout << "[set_all_stiffness][ERROR] An error occured while setting stiffness value: " << e.toString() << std::endl;   
    }

    log_file << "[set_all_stiffness] Done Setting all stiffness values to: " << stiffnessVal << std::endl;
    std::cout << "[set_all_stiffness] Done Setting All stiffness values to: " << stiffnessVal << std::endl;
}


/////////// End Alias Initialization /////////////




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
//Stupid Debug thing. Will be removed when things work.
void hal_experimental::debug_alvalue(AL::ALValue &v, std::string name="NULL")
{
    if(cout_debug)
        log_file << "DEBUG_ALVALUE_INFO [" << name << "]: \n\t" << v.serializeToText() << "\n\t" << v.toString() << std::endl; 
}

/**
  * \brief: Helper function to set actuator positions 
  **/
void hal_experimental::set_actuators_positions()
{
    try
    {
        //Set positions
        position_request_alias.arraySetSize(5);
        position_request_alias[4][0] = dcm_proxy->getTime(0); 
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
            position_request_alias[5][i][0] = 0.2;//actuators[i];
            //position_request_alias[5][i][0] = actuators[?][i];
        log_file << "dcm time: " << dcm_proxy->getTime(0) <<  " positions were set.\n";
        dcm_proxy->setAlias(position_request_alias); //Assign the alias for the naoqi DCM
        log_file << "Alias set for position request\n";
    } 
    catch (const AL::ALError &e)
    {
        log_file << "An exception has occured while setting actuator positions: " << e.what() << std::endl;
        log_file << "An exception has occured while setting actuator positions: " << e.what() << std::endl;

    }
}

/**
  * \brief: Tries to set the stiffness values requested from NAOInterface for the servos.
  **/
bool hal_experimental::set_actuators_stiffness()
{
    log_file << "Stiffness IDs to be set: " << headYawStiffnessActuator + NumOfStiffnessActuatorIds << "\n";
    stiffness_request_alias.arraySetSize(5);
    try
    {
        for(int i = headYawStiffnessActuator; i < headYawStiffnessActuator + NumOfStiffnessActuatorIds; ++i)
        {
            if(actuators[i] != last_requested_actuators[i])
            {
                stiffness_request_alias[4][0] = dcm_proxy->getTime(0); //First Time Value (based on internal clock)
                for(int j = 0; j < NumOfStiffnessActuatorIds; ++j)
                    //First command for the jth stiffness actuator
                    stiffness_request_alias[5][j][0] = last_requested_actuators[headYawStiffnessActuator + j] = actuators[headYawStiffnessActuator + j];
                    //stiffness_request_alias[5][j][0] = last_requested_actuators[headYawStiffnessActuator + j] = actuators[headYawStiffnessActuator + j];
                dcm_proxy->setAlias(stiffness_request_alias);
                log_file << "dcm proxy alias set \n";
            }
        }
        log_file << "Position Stiffness was set properly" << std::endl;;
        return true; //Stiffness values succesfully set
    }
    catch(const AL::ALError &e)
    {
        log_file << "An Exception has occured while setting stiffness actuators: " << e.what() << std::endl;
    }
    return false; //Requested stiffness values failed to be set
}

/**
  * \brief: Helper function to set LED values on the NAO
  **/ 
void hal_experimental::set_actuators_leds() //TODO check value: bool &requested_stiffness_set
{
    //log_file << "Setting led actuators (why did I call it actuators?) \n";
    // if(!requested_stiffness_set)
    // {
        led_request_alias.arraySetSize(1);
        try
        {
            for(int i = 0; i < NumOfLedActuatorIds; ++i)
            {
                //Local LED index. Don't confuse with ledIndex variable
                int led_index = faceLedBlueLeft180DegActuator + ledIndex;
                //Ensure our ledIndex isnt > actual number of leds
                if(NumOfLedActuatorIds == ++ledIndex) ledIndex = 0;
                // if(actuators[led_index] != last_requested_actuators[led_index])
                // {
                    //Alias name
                    led_request_alias[0] = std::string(actuatorNames[led_index]);
                    //Update last_requested_actuators val to actuators val
                    last_requested_actuators[led_index] = actuators[led_index];
                    //last_requested_actuators[led_index] = actuators[led_index];
                    ////Finish setting value and tell naoqi
                    //Value for LED (0..1 float)
                    led_request_alias[2][0][0] = last_requested_actuators[led_index];
                    //DCM time for light to be changed. Currently set to be current time (i.e. instant)
                    led_request_alias[2][0][1] = dcm_proxy->getTime(0);
                    dcm_proxy->set(led_request_alias);
                    //log_file << "Actuators LED set \n";
                    break;
                // } else {
                //     log_file
                // }
            }
            //log_file << "set_actuator_leds successfully called\n";

        } 
        catch (const AL::ALError &e)
        {
            log_file << "An Exception has occured while setting LEDS: " << e.what() << "\n";
            std::cout << "AN LED ERROR HAS OCCURED: " << e.what() << "\n";
        }
    //}
}

/**
  * set_actuators: Set Actuators main function + helper function calls.
  * 
  **/
void hal_experimental::set_actuators()
{
	try
	{
        log_file << "[dcm_time:" << dcm_time << "] set_actuators has been called. Stupid dangerous UNSAFE mode is enabled (lol)\n";
        //pineappleJuice->actuator_semaphore.wait();
        dcm_time = dcm_proxy->getTime(0);
        
        //currently actuator update fails aren't handled
        pineappleJuice->actuators_current_read = pineappleJuice->actuators_newest_update;
        /*
        if (pineappleJuice->actuators_newest_update != last_reading_actuator)
        {
            if(actuator_update_fails == 0) 
                log_file << "NaoQi has failed at updating the actuator value. Bad NaoQi. Bad." << "\n";
            actuator_update_fails++;
        }
        else actuator_update_fails = 0;
        */
        last_reading_actuator = pineappleJuice->actuators_newest_update;
        //read_actuators = pineappleJuice->actuators[pineappleJuice->actuators_current_read];
        read_actuators = pineappleJuice->actuators_unsafe;
        
        //Get actual actuator vals to be assigned. These values won't exactly correspond to
        //what NAOInterface passed due to the various states the robot may be in.
        //actuators = robot_state_handler(read_actuators);
        //pineappleJuice->actuator_semaphore.post();
        log_file << "I may have gotten through the unsafe part with only minimal irreversible brain damage.\n";

        //Set Actuator values
        set_actuators_positions();
        log_file << "set_actuators_positions was called\n";
        bool was_set = set_actuators_stiffness();
        log_file << "set_actuators_stiffness was called\n";
        // set_actuators_leds();
        // log_file << "set_actuators_leds was called" << std::endl;
        log_file << "Hardware values updated" << '\n';
      //  SAY("My hardware values have been updated");
        //TODO: Gamecontroller stuff (team info)
        log_file << "I should have said something dumb about my hardware values being updated" << std::endl;;
        
	}
    catch(AL::ALError &e) //uh...this could yield an Boost.Interprocess exception or a AL::Exception...I wonder if I can throw it as a boost variant?
    {
        log_file << "set_actuators exception: " << e.what() << std::endl;
    }
    print_actuators();    
}
/** \brief: init_aliases: This function initializes all the required communication channels for hardware control
  *  tl;dr this method and datastructure is cancer. I really don't want to comment much on it because it sucks. 
  * the relevant documentation can be found here: http://doc.aldebaran.com/2-1/naoqi/sensors/dcm-api.html#DCMProxy

  **/
void hal_experimental::init_aliases() 
{
    try
    {
        //First we need to initialize two dimensions for our alias.
        //These two positions allow us to give the alias a name (1st dimension)
        //and then tell it what hardware values we want it to communicate with (2nd dimension)
        commands.arraySetSize(2);


        ///BEGIN POSITION ACTUATOR ALIAS INITIALIZATION
        commands[0] = std::string("positionActuators"); //Nick name
        commands[1].arraySetSize(NumOfPositionActuatorIds); //Allocation of hardware items
        log_file << "commands[1] has " << NumOfPositionActuatorIds << "dimensions" << std::endl;
        //Assign the long strings from the array of strings for positionActuators in util/more_hardware_data.h
        //These are also mapped out in the HardwareMap object.
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        {
            commands[1][i] = std::string(actuatorNames[i]);
        }
        // Create Memory Proxy aliases for actuator values.
        //createAlias(commands) binds our commands alias to the DCM in NaoQi.
        //Returns an ALValue (which is what the aliases are composed of)
        commandsAlias = dcm_proxy->createAlias(commands);    
        ///END

        //LEDs
        //NOTE: This might be wrong...
        // commands[0] = std::string("LEDS");
        // commands[1].arraySetSize(3);
        // for(int i = faceLedRedLeft0DegActuator; i < rFootLedBlueActuator; ++i)
        // {
        //     int realIndex = i - faceLedRedLeft0DegActuator;
        //     commands[1][realIndex] = std::string(actuatorNames[i]);
        // }
        log_file << "1. Actuator Alias Initialized.\n";

        ///BEGIN STIFFNESS ACTUATOR ALIAS INITIALIZATION
        //Recycle
        commands[0] = std::string("stiffnessActuators");
        commands[1].arraySetSize(NumOfStiffnessActuatorIds); //< Ugly data structure
        for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        {
            commands[1][i] = std::string(actuatorNames[headYawStiffnessActuator + i]);
        }
        commandsAlias = dcm_proxy->createAlias(commands);
        ///END


        ///BEGIN POSITION REQUEST ALIAS INITIALIZATION

        position_request_alias.arraySetSize(6); //6 dimensions because too damn many degrees of freedom
        position_request_alias[0] = std::string("positionActuators"); //Use the Alias we created above
        position_request_alias[1] = std::string("ClearAll"); //Clear any set values in the alias
        position_request_alias[2] = std::string("time-separate"); //Timing Paramers
        position_request_alias[3] = 0; //Importance Level which Aldabaran never implemented
        position_request_alias[4].arraySetSize(1); //we just add a dim for time to execute the command
        position_request_alias[5].arraySetSize(NumOfPositionActuatorIds); //List of commands
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        {
            position_request_alias[5][i].arraySetSize(1);   
        }
        debug_alvalue(position_request_alias, "position_request_alias");
        ///END

        log_file << "2. Position Alias Initialized.\n";


        ///BEGIN STIFFNESS REQUEST ALIAS INITIALIZATION
        //Repeat
        stiffness_request_alias.arraySetSize(6);
        stiffness_request_alias[0] = std::string("stiffnessActuators");
        stiffness_request_alias[1] = std::string("ClearAll");
        stiffness_request_alias[2] = std::string("time-separate");
        stiffness_request_alias[3] = 0; 
        stiffness_request_alias[4].arraySetSize(1);
        stiffness_request_alias[5].arraySetSize(NumOfStiffnessActuatorIds);
        for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        {
            stiffness_request_alias[5][i].arraySetSize(1);
        }
        debug_alvalue(stiffness_request_alias, "stiffness_request_alias");
        ///END

        log_file << "3. Stiffness Alias Initialized.\n";


        // ///BEGIN LED REQUEST ALIAS INITIALIZATION

        // led_request_alias.arraySetSize(3);
        // led_request_alias[1] = std::string("ClearAll");
        // led_request_alias[2].arraySetSize(1);
        // led_request_alias[2][0].arraySetSize(2);
        // led_request_alias[2][0][1] = 0; //As you can see, LEDS are suuuper easy to work with

        // debug_alvalue(led_request_alias, "led_request_alias");
        // ///END

        // log_file << "4. LED Alias Initialized.\n";

        //Initialize Sensor Pointers
        for(int i = 0; i < NumOfSensorIds; ++i)
            sensor_ptrs[i] = static_cast<float *>(nao_memory_proxy->getDataPtr(sensorNames[i]));
        
        log_file << "5. Sensor Pointers initialized" << '\n';

        ///Initialize Requested Actuators...zero'd out by default
        void *ms_ret = std::memset(last_requested_actuators, 0, sizeof(last_reading_actuator)); 
        if(ms_ret == NULL)
        {
            log_file << "A catastrophic error has occured! Could not set memory for actuator values." << std::endl;
            SAY("Please connect me to the internet, I need to download more ram");
            return;
        }
        for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
            last_requested_actuators[i] = -1.f;
        
        log_file << "6. Requested Actuators Initialized." << "\n";
        SAY("Aliases initialized");
    }
    catch(const AL::ALError &e) 
    {
        std::cerr << "Error initializing hardware aliases: " << e.what() << std::endl;
        log_file << "Error initializing hardware aliases: " << e.what() << '\n';
        //SAY("Oh no, my spine broke. What did you do to me?");
    }
    

}

void hal_experimental::read_sensors()
{
    try
    {
        log_file << "Reading Sensor Values in a stupid UNSAFE way\n";
        //pineappleJuice->sensor_semaphore.wait();
        int sensor_data_written = 0;
        //currently support for dropped reads/writes is disabled
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
            log_file << "Sensor data being overwritten to shared memory!" << "\n"; 
        }
        //float *current_sensor = pineappleJuice->sensors[sensor_data_written];
        float *current_sensor = pineappleJuice->sensors_unsafe;
        for(int i = 0; i < NumOfSensorIds; ++i)
        {
            current_sensor[i] = *sensor_ptrs[i];
        }
        //Update sensor values in shared memory
        pineappleJuice->sensors_newest_update = sensor_data_written;
        log_file << "[read_sensors()] sensor data read...posting semaphore." << "\n";
        log_file << "Done with UNSAFE sensor stuff\n";
        //pineappleJuice->sensor_semaphore.post();
        SAY("My Sensor Values have been updated");
        log_file << "[read_sensors()] semaphore posted!" << std::endl;
        
    }
    catch (const std::exception &e)
    {
        log_file << "An error occured while trying to read the sensor data: " << e.what() << "\n";   
    }
    print_sensors();
}


void hal_experimental::print_sensors()
{
    log_file << "================= SENSOR VALUES! =================" << "\n";
    pineappleJuice->sensor_semaphore.wait();
    for(int i = 0; i < NumOfSensorIds; ++i)
        log_file << sensorNames[i] << " = " << pineappleJuice->sensors_unsafe[i] << "\n";
    //    std::cout << sensorNames[i] << " = " << pineappleJuice->sensors[pineappleJuice->sensors_newest_update][i] << std::endl;
    pineappleJuice->sensor_semaphore.post(); 
}
void hal_experimental::print_actuators()
{
    log_file << "================= ACTUATOR VALUES! =================" << "\n";
    pineappleJuice->actuator_semaphore.wait();
    for(int i = 0; i < NumOfActuatorIds; ++i)
            log_file << sensorNames[i] << " = " << pineappleJuice->actuators_unsafe[i] << "\n";
    //    std::cout << sensorNames[i] << " = " << pineappleJuice->actuators[pineappleJuice->actuators_newest_update][i] << std::endl;
    pineappleJuice->sensor_semaphore.post(); 
}

void hal_experimental::actuator_joint_test()
{
    // std::cout << "[actuator_jt]: Starting Test..." << std::endl;
    // log_file << "[actuator_jt]: Starting Test..." << std::endl;
    int current_dcm_time;
    try
    {
        current_dcm_time = dcm_proxy->getTime(0);
    }
    catch(const AL::ALError &e)
    {
        log_file << "[postCallBack RT][ERROR]: Error accessing dcm: " << e.what() << std::endl;
        std::cout << "[postCallBack RT][ERROR]: Error accessing dcm: " << e.what() << std::endl;
    }
    // std::cout << "[actuator_jt]: Got time! " << current_dcm_time << std::endl;
    // log_file << "[actuator_jt]: Got time! " << current_dcm_time << std::endl;
    //Time for the next cycle. Note there needs to be 10ms between sets,
    //however this ammount of time should happen naturally.
    try
    {
        // std::cout << "[actuator_jt]: Attempting to set values..." << std::endl;
        // log_file << "[actuator_jt]: Attempting to set values..." << std::endl;
        // std::cout << "[actuator_jt]: 1. Setting Time..." << std::endl;
        // log_file << "[actuator_jt]: 1. Setting Time..." << std::endl;
        commands[4][0] = current_dcm_time; 


        if(initialJointSensorValues.size() == 0)
        {
            // std::cout << "[actuator_jt]: initialJointSensorValues is empty. Assigning those now..." << std::endl;
            // log_file << "[actuator_jt]: initialJointSensorValues is empty. Assigning those now..." << std::endl;
            //for(int i = 0; i < NumOfPositionActuatorIds; ++i)
            for(int i = 0; i < 25; ++i)
            {
                initialJointSensorValues.push_back(sensorValues[i]);
                // log_file << "[connectToDCMLoop] initialSensorValues[" << i << "] = " << sensorValues[i] << '\n';
                // std::cout << "[connectToDCMLoop] initialSensorValues[" << i << "] = " << sensorValues[i] << '\n';
            }
        }

        // std::cout << "[actuator_jt]: 2. Setting initialJointSensorVals to commands[5][i][0]: " << initialJointSensorValues.size()  << " | " << NumOfPositionActuatorIds << std::endl;
        // log_file << "[actuator_jt]: 2. Setting initialJointSensorVals to commands[5][i][0]: " << initialJointSensorValues.size() << " | " << NumOfPositionActuatorIds << std::endl;

       
       
       // for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        for(int i = 0; i < 25; ++i)
        {
            commands[5][i][0] = 0.2f;//initialJointSensorValues[i];
        }

        // std::cout << "[actuator_jt]: 3. Trying to get sensor values with fast access..." << std::endl;
        // log_file << "[actuator_jt]: 3. Trying to get sensor values with fast access..." << std::endl;
        fMemoryFastAccess->GetValues(sensorValues);
        // std::cout << "[actuator_jt]: Got sensor values with fast access! " << sensorValues.size() << std::endl;
        // log_file << "[actuator_jt]: Got sensor values with fast access! " << sensorValues.size() << std::endl;
        //Test
        std::cout << "[ShoulderPitchPositionActuator] RIGHT =  " <<  sensorValues[rShoulderPitchPositionSensor] << " | LEFT = " << sensorValues[lShoulderPitchPositionSensor]  << std::endl;
        std::cout << "[ShoulderRollPositionActuator]: RIGHT = " << sensorValues[rShoulderRollPositionSensor] << " | LEFT = " << sensorValues[lShoulderRollPositionSensor] << std::endl;
        std::cout << "[ElbowYawPositionActuator]: RIGHT = " << sensorValues[rElbowRollPositionSensor] << " | LEFT = " <<  sensorValues[lElbowRollPositionSensor] << std::endl;
        std::cout << "[ElbowRollPositionActuator]: RIGHT = " << sensorValues[rElbowRollPositionSensor] << " | LEFT = " << sensorValues[lElbowRollPositionSensor] <<  std::endl;
        
        //!!IMPORTANT!! commands[5][..PositionACTUATOR][0] = sensorValues[..PositionSENSOR];
        commands[5][lShoulderPitchPositionActuator][0] =  sensorValues[rShoulderPitchPositionSensor];
        commands[5][lShoulderRollPositionActuator][0]  = -sensorValues[rShoulderRollPositionSensor];
        commands[5][lElbowYawPositionActuator][0]      = -sensorValues[rElbowRollPositionSensor];
        commands[5][lElbowRollPositionActuator][0]     = -sensorValues[rElbowRollPositionSensor];
        // commands[5][L_SHOULDER_PITCH][0] =   sensorValues[R_SHOULDER_PITCH];
        // commands[5][L_SHOULDER_ROLL][0]  = - sensorValues[R_SHOULDER_ROLL];
        // commands[5][L_ELBOW_YAW][0]      = - sensorValues[R_ELBOW_YAW];
        // commands[5][L_ELBOW_ROLL][0]     = - sensorValues[R_ELBOW_ROLL];



        //commands[5][headYawPositionActuator][0] = sensorValues[headYawPositionSensor] + deg2rad(10);
        //commands[5][headYawPositionActuator][0] = 0.3;
        // std::cout << "[actuator_jt]: 4. Values should be set! Assigning to alias..." << std::endl;
        // log_file <<  "[actuator_jt]: 4. Values should be set! Assigning to alias..." << std::endl;
    }
    catch(const AL::ALError &e)
    {
        std::cout << "[DCM POST LOOP][ERROR] An error occurred in the post DCM callback loop: " << e.toString() << std::endl;
        log_file << "[DCM POST LOOP][ERROR] An error occurred in the post DCM callback loop: " << e.toString() << std::endl;
    }

    try
    {
    //    std::cout << "Trying to set postcallback commands alias..." << std::endl;
        dcm_proxy->setAlias(commands);
     //   std::cout << "Post callback vals should be set!" << std::endl;
    } 
    catch(const AL::ALError &e)
    {
        log_file << "[postCallBack RT][ERROR]: Error setting dcm alias: " << e.what() << std::endl;
        std::cout << "[postCallBack RT][ERROR]: Error setting dcm alias: " << e.what() << std::endl;
    }
    // std::cout << "[actuator_jt]: Should be done!" << std::endl;
    // log_file <<  "[actuato   r_jt]: Should be done!" << std::endl;
}

void hal_experimental::testAliases()
{
    //This is pretty specific to the head, the point is to make it move back and forth
    //Also modulo's aren't the cheapest computation
    if (testLEDRot % 29 == 0)
    {
        mult *= -1;
    
        //Print data
        try
        {
            log_file << "[testAlias] Reading sensor values!\n";
            for(int i = 0; i < NumOfSensorIds; ++i)
            {
                AL::ALValue v = nao_memory_proxy->getData(sensorNames[i]);
                log_file << sensorNames[i] <<" = " << v.toString() << "\n";
            }
            log_file << "Done reading sensor values \n\n" << std::endl;
        }
        catch(const AL::ALError &e)
        {
            log_file << "[testAlias] Error has occured while reading sensor values: " << e.what() << std::endl;
            std::cout << "[testAlias] Error has occured while reading sensor values: " << e.what() << std::endl;
        }
    }
    float jank_position_value = deg2rad(float(testLEDRot % 29 + 0.5 * mult));


    if(!testAliasesInitialized)
    {
        try
        {

            positionTestAlias.arraySetSize(2); //6 dimensions because too damn many degrees of freedom
            positionTestAlias[0] = std::string("positionActuatorsTest"); //Use the Alias we created above
            positionTestAlias[1].arraySetSize(NumOfPositionActuatorIds);
            for(int i = 0; i < NumOfPositionActuatorIds; ++i)
            {
                positionTestAlias[1][i] = std::string(actuatorNames[i]);
                std::cout << "[testAliases] SET position_request_alias[5][" << i << "] = " << actuatorNames[i] << "\n";

                log_file << "[testAliases] SET position_request_alias[5][" << i << "] = " << actuatorNames[i] << "\n";
            }
            dcm_proxy->createAlias(positionTestAlias);
            log_file << "[testAliases] aliase created in the dcm!" << std::endl;
            std::cout << "[testAliases] aliase created in the dcm!" << std::endl;
            testAlias.arraySetSize(4);
            testAlias[0] = std::string("positionActuatorsTest");
            testAlias[1] = std::string("ClearAll"); //Clear any set values in the alias
            testAlias[2] = std::string("time-mixed"); //Timing Paramers
            testAlias[3].arraySetSize(NumOfPositionActuatorIds);
            /*
            testAlias[3] = 0; //Importance Level which Aldabaran never implemented
            testAlias[4].arraySetSize(1); //we just add a dim for time to execute the command
            testAlias[5].arraySetSize(NumOfPositionActuatorIds); //List of command
            */ 

        } catch(const AL::ALError &e) {
            log_file << "testAliases Error on initialization: " << e.what() << "\n";
            std::cout << "testAliases Error on initialization: " << e.what() << "\n";

        }
        log_file << "testAliases done with setup \n";
        testAliasesInitialized = true;

    }
    try
    {
        log_file << "[testAlias] Setting hardware values" << std::endl;
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        {
            testAlias[3][i].arraySetSize(1);
            testAlias[3][i][0].arraySetSize(2);
            testAlias[3][i][0][0] = jank_position_value;//actuators[i];
            testAlias[3][i][0][1] = dcm_proxy->getTime(100);
        }
            //testAlias[5][i][0] = actuators[?][i];
        log_file << "[testAliases] Set another dimension\n";
        std::cout << "[testAliases] Set another dimension" << std::endl;

        dcm_proxy->setAlias(testAlias); //Assign the alias for the naoqi DCM
      //  log_file << "dcm time: " << dcm_proxy->getTime(0) <<  " positions were set.\n";
    }
    catch(const AL::ALError &e)
    {
        log_file << "An error has occured while testing Aliases: " << e.what() << '\n';
        std::cout << "An error has occured while testing Aliases: " << e.what() << '\n';
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

void hal_experimental::LOG(const std::string function, const std::string message)
{
    std::cout << "[" << function << "]: " << message << std::endl;
    std::cout << "[" << function << "]: " << message << std::endl; 
 
}
//Returns an approximated value
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
