#ifndef _HAL_EXPERIMENTAL_H
#define _HAL_EXPERIMENTAL_H

//Boost Includes
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <boost/shared_ptr.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/permissions.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_map.hpp>
//Aldabaran Includes
#include "alcommon/albroker.h"
#include "alcommon/albrokermanager.h"
#include "alcommon/alproxy.h"
#include "alproxies/almemoryproxy.h"
#include "alcommon/almodule.h"
#include "alvalue/alvalue.h"
#include "alproxies/dcmproxy.h"
#include <alproxies/altexttospeechproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>

//Standard Library Includes
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include <fstream>
#include <limits>
#include <vector>
#include <semaphore.h>
//Project Includes
#include "../../include/memory/hal_data.h"
//#include "../../include/util/hardwaremap.h"



namespace AL {
    class ALBroker;
    class ALMemoryFastAccess;
}

using namespace boost::interprocess;

class hal_experimental : public AL::ALModule {

public:
    hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);
   ~hal_experimental();

    void create_interprocess_memory();
    void open_interprocess_memory();
    
    static const std::string name;
    void onPreProcess();
    void onPostProcess();
    void startLoop();
    void stopLoop();

    //Interprocess syncing
    void update_actuator_values();
    void update_sensor_values();
    void update_text_to_speak();
    
    void initialize_everything();
    void connectToDCMLoop();

    
    /// Alias initialization
    //Creation
    void create_fast_sensor_access();
    void create_actuator_position_aliases();
    void create_actuator_stiffness_aliases();
    //Preparation
    void init_actuator_position_aliases();
    //void init_actuator_stiffness_aliases();

    //Control
    void set_all_actuator_stiffnesses(const float &stiffnessVal);    


    void set_LEDS();
    float* robot_state_handler(float *actuator_vals);
    //test
    void testLEDS();
    void actuator_joint_test();
    std::pair<hal_data *, std::size_t> shared_data_ptr; ///< Because for some reason it segfaults just using pineappleJuice...

    std::ofstream log_file;
private:

    float deg2rad(float degress);
    void LOG(const std::string function, const std::string message);
    void get_ip_address();

    boost::shared_ptr<AL::ALBroker> pBrokerCopy;
    //AL Proxies
    boost::shared_ptr<AL::DCMProxy> dcm_proxy;
    boost::shared_ptr<AL::ALMemoryFastAccess> fMemoryFastAccess;

    AL::ALMemoryProxy *nao_memory_proxy;
    AL::ALTextToSpeechProxy *speak_proxy;

    //DCM Signal Hooks for Real Time Communication
    ProcessSignalConnection fDCMPostProcessConnection, fDCMPreProcessConnection;

    //Main aliases
    AL::ALValue position_request_alias, stiffness_request_alias, led_request_alias;
    AL::ALValue commands, commandsAlias;

    //Sensor names for fast memory access
    std::vector<std::string> fSensorKeys;
    //Pointer for fast memory access

    // Test Aliases
    AL::ALValue commandsTestAlias, commandsTest; //Chest LEDS
    AL::ALValue positionTestAlias, testAlias; //Actuator Position
    //Head and Body ID's and Version number. Useful for compatability checking
    std::string body_ID; 
    std::string body_version;
    std::string head_ID;
    std::string head_version;
    boost::interprocess::managed_shared_memory shm;
    //boost::interprocess::named_semaphore actuator_semaphore, sensor_semaphore;
    

	uint8_t last_reading_actuator;
	size_t actuator_update_fails;
	//int dcm_time;
    int dcm_time;
    float last_requested_actuators[NumOfActuatorIds];
    hal_data *pineappleJuice;
    float *read_actuators;
    //float *actuators;
    float actuators[NumOfActuatorIds];
    std::vector<float> sensorValues;
    std::vector<float> initialJointSensorValues;

    float *sensor_ptrs[NumOfSensorIds];
    static hal_experimental *instance;
    sem_t *semaphore;

    char last_spoken_text[35];
    //TESTS
    unsigned long long testLEDRot;
    int lastTime;
    bool testLEDInitialized;

};
#endif
