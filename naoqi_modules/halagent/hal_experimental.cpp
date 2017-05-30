#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;    

struct HardwareMap 
{
    typedef std::pair<int, std::string> _value;
    typedef boost::unordered_map<std::string, _value> hashmap;
    //HardwareMap generator...brought to you by the power of Reflection.
    //Constructor ensures we populate both maps from the start
    HardwareMap() 
    {
        std::cout << "Constructing hardware map in hal_experimental" << std::endl;
        //Generate sensor map 

        for (size_t index = 0; index < BetterSensorIds::_values().size(); ++index)
        {
            BetterSensorIds _sensor = BetterSensorIds::_values()[index];
            _value v = std::make_pair<int, std::string>(_sensor, sensorNames[_sensor]);
            SensorMap.emplace(_sensor._to_string(), v);
        }
        std::cout << "Done initializing sensor map!\nInitializing Actuators Map" << std::endl;
        for (size_t index = 0; index < BetterActuatorIds::_values().size(); ++index)
        {
            BetterActuatorIds _actuator = BetterActuatorIds::_values()[index];
            _value v = std::make_pair<int, std::string>(_actuator, sensorNames[_actuator]);
            ActuatorMap.emplace(_actuator._to_string(), v);
        }
    }
    //Default destructor, nothing special
    ~HardwareMap() {}

    //Overloaded subscript operator for accessing keys from either map
     const _value& operator[](const std::string key)
    {
        //Handle runtime exceptions of non-existent key lookup
        //TODO: This can be optimized to constant time lookups
        if(!(this->SensorMap.find(key) == this->SensorMap.end()))
            return this->SensorMap[key];
        else if (!(this->ActuatorMap.find(key) == this->ActuatorMap.end()))
            return this->ActuatorMap[key];
        else
        {
            _value null_val = std::make_pair<int,std::string>(0,"NULL");
            return null_val;
        }
    }
    const hashmap getSensorMap()
    {
        return SensorMap;
    }
    const hashmap getActuatorMap()
    {
        return ActuatorMap;
    }


    private:
        hashmap SensorMap;
        hashmap ActuatorMap;
};
//
// Borrowed and modified from rUNSWift 2016 release
// 
// Not really thread safe

#define SAY(text) speak_proxy->post.say(text); 

//const std::string hal_experimental::name("hal_experimental");
hal_experimental* hal_experimental::instance = NULL;
hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        dcm_proxy(NULL),
        nao_memory_proxy(NULL),
        speak_proxy(NULL),
        ledIndex(0),
        cout_debug(true)
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
        SAY("I could't use the LOG Fie");
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


    HardwareMap hm;
    /** \Brief: This part is complicated and you really need to shouldn't care about it.
      * 
      * Inside this try block we are doing quite a few things. To summarize, we establish communication
      * proxies with NAOQi, then we begin creating AL::Aliases to (questionably) more effeciently
      * send values to the DCM.
      **/
    try
    {
        // Establish Communication with NaoQi
        dcm_proxy = new DCMProxy(pBroker);
        nao_memory_proxy = new ALMemoryProxy(pBroker);
        speak_proxy = new ALTextToSpeechProxy("localhost", 9559);
        body_ID = static_cast<std::string>(nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0));
        head_ID = static_cast<std::string>(nao_memory_proxy->getData("Device/DeviceList/Head/FullHeadId", 0));
        // body_version = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0));
        // head_version = static_cast<std::string>(nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0));
        log_file << "HeadID proxy call value: " << head_ID << "\n" << "BodyID: " << body_ID;
        SAY("You better have fixed me this time!");
        // commands.arraySetSize(2); //< Two dimensional array.

        // ///BEGIN POSITION ACTUATOR ALIAS INITIALIZATION

        // commands[0] = std::string("positionActuators");
        // commands[1].arraySetSize(NumOfPositionActuatorIds); //< Ugly data structure

        // for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        // {
        //     commands[1][i] = std::string(actuatorNames[i]);
        // }
        // // Create Memory Proxy aliases for actuator values
        // commandsAlias = dcm_proxy->createAlias(commands);
        
        // ///END

        // log_file << "1. Actuator Alias Initialized.\n";


        // ///BEGIN STIFFNESS ACTUATOR ALIAS INITIALIZATION
       
        // commands[0] = std::string("stiffnessActuators");
        // commands[1].arraySetSize(NumOfStiffnessActuatorIds); //< Ugly data structure
        // for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        // {
        //     commands[1][i] = std::string(actuatorNames[headYawStiffnessActuator + i]);
        // }
        // debug_alvalue(commands, "commands Alias");
        // commandsAlias = dcm_proxy->createAlias(commands);
       
        // ///END


        // ///BEGIN POSITION REQUEST ALIAS INITIALIZATION

        // position_request_alias.arraySetSize(6);
        // position_request_alias[0] = std::string("positionActuators");
        // position_request_alias[1] = std::string("ClearAll"); //Clear any set values in the alias
        // position_request_alias[2] = std::string("time-separate"); //Timing Paramers
        // position_request_alias[3] = 0; //idk
        // position_request_alias[4].arraySetSize(1);
        // position_request_alias[5].arraySetSize(NumOfPositionActuatorIds);
        // for(int i = 0; i < NumOfPositionActuatorIds; ++i)
        // {
        //     position_request_alias[5][i].arraySetSize(1);
        // }
        // debug_alvalue(position_request_alias, "position_request_alias");
        // ///END

        // log_file << "2. Position Alias Initialized.\n";


        // ///BEGIN STIFFNESS REQUEST ALIAS INITIALIZATION

        // stiffness_request_alias.arraySetSize(6);
        // stiffness_request_alias[0] = std::string("stiffnessActuators");
        // stiffness_request_alias[1] = std::string("ClearAll"); //Clear any set values in the alias
        // stiffness_request_alias[2] = std::string("time-separate"); //Timing Paramers
        // stiffness_request_alias[3] = 0; //idk
        // stiffness_request_alias[4].arraySetSize(1);
        // stiffness_request_alias[5].arraySetSize(NumOfStiffnessActuatorIds);
        // for(int i = 0; i < NumOfStiffnessActuatorIds; ++i)
        // {
        //     stiffness_request_alias[5][i].arraySetSize(1);
        // }
        // debug_alvalue(stiffness_request_alias, "stiffness_request_alias");
        // ///END

        // log_file << "3. Stiffness Alias Initialized.\n";


        // ///BEGIN LED REQUEST ALIAS INITIALIZATION

        // led_request_alias.arraySetSize(3);
        // led_request_alias[1] = std::string("ClearAll");
        // led_request_alias[2].arraySetSize(1);
        // led_request_alias[2][0].arraySetSize(2);
        // led_request_alias[2][0][1] = 0; //As you can see, LEDS are suuuper easy to work with

        // debug_alvalue(led_request_alias, "led_request_alias");
        // ///END

        // log_file << "4. LED Alias Initialized.\n";

        // //Initialize Sensor Pointers
        // for(int i = 0; i < NumOfSensorIds; ++i)
        //     sensor_ptrs[i] = static_cast<float *>(nao_memory_proxy->getDataPtr(sensorNames[i]));
        
        // log_file << "5. Sensor Pointers initialized" << '\n';

        // ///Initialize Requested Actuators
        // void *ms_ret = std::memset(last_requested_actuators, 0, sizeof(last_reading_actuator)); //Allocate memory
        // if(ms_ret == NULL)
        // {
        //     log_file << "A catastrophic error has occured! Could not set memory for actuator values." << std::endl;
        //     return;
        // }
        // for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
        //     last_requested_actuators[i] = -1.f;
        
        // log_file << "6. Requested Actuators Initialized." << "\n";


        instance = this; ///< Remove this to cause fun segfaults...
        testLEDRot = 0;
        try {
            dcm_proxy->getGenericProxy()->getModule()->atPreProcess(boost::bind(&hal_experimental::preCallBack, this));
            dcm_proxy->getGenericProxy()->getModule()->atPostProcess(boost::bind(&hal_experimental::postCallBack, this));
            SAY("Wooho I am ready");
            std::cout << "Ready for things. Making LEDS blink now" << std::endl;
           // testLEDS(true);
            //set_LEDS();
        } catch (std::exception &e) {
            log_file << "ERROR: dcm proxy error: " << e.what() << "\n";
        }
        log_file << "7. Proxy call backs established.\n";
	} catch(AL::ALError &e) {
        log_file << "[ERROR] Fatal Error: Could Not Initialize Interface with NaoQi due to: "<< e.what() << "\n";
    }
    //Boost Interprocess Shared Memory Initialization:
        log_file << "Testing actuator semaphore wait...\n";
        pineappleJuice->actuator_semaphore.wait();
        log_file << "still segfaulting?\n";
        if(dcm_proxy == NULL) 
            log_file << "dcm_proxy is null\n"; 
        else 
            log_file << "dcm_proxy is not null\n";
        dcm_time = dcm_proxy->getTime(0); ///< Get's current time on NAO
        log_file << "NO SEGFAULT: DCM Time = " << dcm_time << "\n";
        log_file << "woohoo I'm in a semaphore" << '\n';
        pineappleJuice->actuator_semaphore.post();
        try
        {
            commandsAlias.arraySetSize(2);
            commandsAlias[0] = std::string("ChestLeds");
            commandsAlias[1].arraySetSize(3);
            commandsAlias[1][0] = std::string("ChestBoard/Led/Red/Actuator/Value");
            commandsAlias[1][1] = std::string("ChestBoard/Led/Green/Actuator/Value");
            commandsAlias[1][2] = std::string("ChestBoard/Led/Blue/Actuator/Value");
            dcm_proxy->createAlias(commandsAlias);
            std::cout << "test LEDs aliass created" << std::endl;
            commands.arraySetSize(4);
            commands[0] = std::string("ChestLeds");
            commands[1] = std::string("ClearAll");
            commands[2] = std::string("time-mixed");
        }
        catch (const AL::ALError &e) {
            log_file << "Oh damn there was an error with the testLEDS: " << e.what() << "\n";
        }
    SAY("I am ready for action! Woohoo");
 //   get_ip_address();
    lastTime = dcm_proxy->getTime(0);
    testLEDS(true);

    std::cout << "hal_experimental Fully Initialized!" << std::endl;

}
/**
* The method is called by NaoQi immediately before it communicates with the chest board.
* It sets all the actuators.
*/
void hal_experimental::preCallBack() 
{
    std::cout << "Pre-Callback called" << std::endl;
    log_file << "preCallBack called" << '\n';
    instance->testLEDS(true);
    //set_actuators();
    //log_file << "preCallBack: set_actuators succcessfully called\n";
    //instance->speak();
    //SAY("HURR DURR I'm a sheep");

}
/**
* The method is called by NaoQi immediately after it communicates with the chest board.
* It reads all sensors.
*/
void hal_experimental::postCallBack()
{
    std::cout << "Post-Callback called" << std::endl;  
    log_file << "postCallBack successfully called\n";
    //instance->testLEDS(false);
    //instance->read_sensors();
    //log_file << "Sensors have been read\n";
    //SAY("RIP in peace!");
}

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
    //buffer overflow and use an exploit to make the NAO part of your botnet (lol).
    log_file << "get_ip_address: Performing Sanity Check\n";
    FILE *cmdResult;
    char cmdBuffer[512];
    if(!(cmdResult = popen("ls","r")))
    {
        log_file << "get_ip_address: Sanity check failed!\n";
        SAY("I am insane");
        return;
    }
    while(fgets(cmdBuffer, sizeof(cmdBuffer), cmdResult) != NULL)
    {
        log_file << "get_ip_address [SANITY CHECK]: " << std::string(cmdBuffer) << "\n";
        SAY(string(cmdBuffer));
    }
    pclose(cmdResult);

    SAY("I have given up on becoming a leet haxor")
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
    delete dcm_proxy;
    delete nao_memory_proxy;
    delete speak_proxy;
}

void hal_experimental::set_LEDS()
{
    for(int i = faceLedRedLeft0DegActuator; i <= faceLedBlueRight315DegActuator; ++i)
        actuators[i] = 0.f;
    // LED Debug stuff can go here
    //float blink_val = float(dcm_time / 500 & 1);
    float blink_val = 0.35;
    actuators[faceLedGreenRight180DegActuator] =  blink_val;
    actuators[faceLedGreenLeft180DegActuator] = blink_val;
    log_file << "LEDS were set" << std::endl;
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
        position_request_alias[4][0] = dcm_time; 
        for(int i = 0; i < NumOfPositionActuatorIds; ++i)
            position_request_alias[5][i][0] = 0.2;//actuators[i];
            //position_request_alias[5][i][0] = actuators[?][i];
        log_file << "dcm time: " << dcm_time <<  " positions were set.\n";
        dcm_proxy->setAlias(position_request_alias); //Assign the alias for the naoqi DCM
        log_file << "Alias set for position request\n";
    } 
    catch (const AL::ALError &e)
    {
        log_file << "An exception has occured while setting actuator positions: " << e.what() << std::endl;
    }
}

/**
  * \brief: Tries to set the stiffness values requested from NAOInterface for the servos.
  **/
bool hal_experimental::set_actuators_stiffness()
{
    log_file << "Stiffness IDs to be set: " << headYawStiffnessActuator + NumOfStiffnessActuatorIds << "\n";
    try
    {
        for(int i = headYawStiffnessActuator; i < headYawStiffnessActuator + NumOfStiffnessActuatorIds; ++i)
        {
            if(actuators[i] != last_requested_actuators[i])
            {
                stiffness_request_alias[4][0] = dcm_time; //First Time Value (based on internal clock)
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
void hal_experimental::set_actuators_leds(bool &requested_stiffness_set) //TODO check value
{
    log_file << "Setting led actuators (why did I call it actuators?) \n";
    if(!requested_stiffness_set)
    {
        try
        {
            for(int i = 0; i < NumOfLedActuatorIds; ++i)
            {
                //Local LED index. Don't confuse with ledIndex variable
                int led_index = faceLedBlueLeft180DegActuator + ledIndex;
                //Ensure our ledIndex isnt > actual number of leds
                if(NumOfLedActuatorIds == ++ledIndex) ledIndex = 0;
                if(actuators[led_index] != last_requested_actuators[led_index])
                {
                    //Alias name
                    led_request_alias[0] = std::string(actuatorNames[led_index]);
                    //Update last_requested_actuators val to actuators val
                    last_requested_actuators[led_index] = 1.0;//actuators[led_index];
                    //last_requested_actuators[led_index] = actuators[led_index];
                    ////Finish setting value and tell naoqi
                    //Value for LED (0..1 float)
                    led_request_alias[2][0][0] = 1.0;//last_requested_actuators[led_index];
                    //DCM time for light to be changed. Currently set to be current time (i.e. instant)
                    led_request_alias[2][0][1] = dcm_time;
                    dcm_proxy->set(led_request_alias);
                    log_file << "Actuators LED set \n";
                    return;
                }
            }
            log_file << "set_actuator_leds successfully called\n";

        } 
        catch (const AL::ALError &e)
        {
            log_file << "An Exception has occured while setting LEDS: " << e.what() << "\n";   
        }
    }
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
        actuators = robot_state_handler(read_actuators);
        //pineappleJuice->actuator_semaphore.post();
        log_file << "I may have gotten through the unsafe part with only minimal irreversible brain damage.\n";

        //Set Actuator values
        set_actuators_positions();
        log_file << "set_actuators_positions was called\n";
        bool was_set = set_actuators_stiffness();
        log_file << "set_actuators_stiffness was called\n";
        set_actuators_leds(was_set);
        log_file << "set_actuators_leds was called" << std::endl;
        log_file << "Hardware values updated" << '\n';
        SAY("My hardware values have been updated");
        //TODO: Gamecontroller stuff (team info)
        log_file << "I should have said something dumb about my hardware values being updated" << std::endl;;
        
	}
    catch(AL::ALError &e) //uh...this could yield an Boost.Interprocess exception or a AL::Exception...I wonder if I can throw it as a boost variant?
    {
        log_file << "set_actuators exception: " << e.what() << std::endl;
    }
    print_actuators();    
}

void hal_experimental::init() 
{
    std::cout << "Hello world! This proves naoqi calls things automatically" << std::endl;

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

//Execute shell commands
std::string hal_experimental::execute_shell_command(const char* command)
{
    char buffer[128];
    std::string res = "";
    FILE *pipe = popen(command, "rw+");
    if(!pipe) {
        log_file << "[Execute Command]: !pipe was raised \n";
        return "NULL"; //If something breaks or an invalid command is issued, safely returns a null string.
    }
    try
    {
        while(!feof(pipe))
        {
            if(fgets(buffer, 128, pipe) != NULL)
                res += buffer;
            else
                log_file << "Something went wrong acquiring the buffer \n";
        }
        log_file << "[Execute Command]: " << res << '\n';
    }
    catch(const std::exception &e) {
        pclose(pipe);
        log_file << "Invalid command PIPED: " << e.what() << '\n';
        return "NULL";
    }
    pclose(pipe);
    log_file << "result of execute command: " << res << std::endl;
    return res; 
}

void hal_experimental::testLEDS(bool debug)
{
       //SAY("I will try to make lights blink");
       //std::cout << "Waiting to set LEDS!" << std::endl;
       //boost::this_thread::sleep(boost::posix_time::seconds(20));
       std::cout << "Actually setting LEDS now" << std::endl;
       try
       {
        std::cout << "Setting chest LEDS" << std::endl;
		commands[3].arraySetSize(3);
		if (testLEDRot == 0 && (dcm_proxy->getTime(0) - lastTime > 2000)) {
            //ChestBoard/Led/Red/Actuator/Value
            commands[3][0].arraySetSize(2);
            commands[3][0][0].arraySetSize(2);
            commands[3][0][0][0] = 1.0;
            commands[3][0][0][1] = dcm_proxy->getTime(0);
            commands[3][0][1].arraySetSize(2);
            commands[3][0][1][0] = 0.0;
            commands[3][0][1][1] = dcm_proxy->getTime(2000);
            testLEDRot++;
            lastTime = dcm_proxy->getTime(0);
            std::cout << "Red value should be set\n";
        } else if (testLEDRot == 1 && (dcm_proxy->getTime(0) - lastTime > 2000)) {
            //ChestBoard/Led/Green/Actuator/Value
            commands[3][1].arraySetSize(1);
            commands[3][1][0].arraySetSize(2);
            commands[3][1][0][0] = 0.25;
            commands[3][1][0][1] = dcm_proxy->getTime(2000);
            testLEDRot++;
            lastTime = dcm_proxy->getTime(0);
            std::cout << "Green value should be set\n";
        } else if (testLEDRot == 2 && (dcm_proxy->getTime(0) - lastTime > 2000)) { 
            //ChestBoard/Led/Blue/Actuator/Value
            commands[3][2].arraySetSize(1);
            commands[3][2][0].arraySetSize(2);
            commands[3][2][0][0] = 0.125;
            commands[3][2][0][1] = dcm_proxy->getTime(2000);
            testLEDRot = 0;
            lastTime = dcm_proxy->getTime(0);
            std::cout << "Blue value should be set\n";
    //     SAY("My chest LEDS should be doing stuff");
        }
        dcm_proxy->setAlias(commands);
        std::cout << "Chest LEDS should have been set" << std::endl;
       } catch(const AL::ALError &e) {
           std::cout << "LED setting error occured: " << e.what() << std::endl;
           log_file << "LED setting error occured: " << e.what() << "\n";
  //         SAY("I cannot even LED");

       }
  //     SAY("My LED's should have blinked!");
}

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
