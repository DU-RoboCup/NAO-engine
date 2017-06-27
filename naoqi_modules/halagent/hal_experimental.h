#ifndef _HAL_EXPERIMENTAL_H
#define _HAL_EXPERIMENTAL_H

//Boost Includes
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
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
//Project Includes
#include "../../include/memory/hal_data.h"
//#include "../../include/util/hardwaremap.h"



namespace AL {
    class ALBroker;
    class ALMemoryFastAccess;
}

class hal_experimental : public AL::ALModule {

public:
    hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);
   ~hal_experimental();
    static const std::string name;
	void set_actuators();
    void read_sensors();
    void onPreProcess();
    void onPostProcess();
    void startLoop();
    void stopLoop();
    
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
    // Helper functions for setting actuators
    void set_actuators_positions();
    bool set_actuators_stiffness();
    void set_actuators_leds(); //bool &requested_stiffness_set
    void init_aliases();
    //test
    void testLEDS();
    void testAliases();
    void actuator_joint_test();
    void speak(); // This can be used to annoy people
    void print_sensors();
    void print_actuators();
    void dummyfunction();
    void get_ip_address();
    void LOG(const std::string function, const std::string message);
    std::pair<hal_data *, std::size_t> shared_data_ptr; ///< Because for some reason it segfaults just using pineappleJuice...

    /**
      * \brief debug_alvalue: Insanity Check...
      * @param v: An ALValue alias (which is a weird multidemensional jagged array)
      * @param name: Alias name
      **/
    void debug_alvalue(AL::ALValue &v, std::string name);
    std::ofstream log_file;
private:

    float deg2rad(float degress);

    boost::shared_ptr<AL::ALBroker> pBrokerCopy;
    //AL Proxies
    boost::shared_ptr<AL::DCMProxy> dcm_proxy;
    //boost::shared_ptr<AL::DCMProxy> dcm_proxy;
    AL::ALMemoryProxy *nao_memory_proxy;
    AL::ALTextToSpeechProxy *speak_proxy;
    boost::shared_ptr<AL::ALMemoryFastAccess> fMemoryFastAccess;

    //DCM Signal Hooks
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
    std::string my_ip_address;
    boost::interprocess::managed_shared_memory shm;

	uint8_t last_reading_actuator;
	size_t actuator_update_fails;
	//int dcm_time;
    int dcm_time;
    float last_requested_actuators[NumOfActuatorIds];
    int ledIndex;
    hal_data *pineappleJuice;
    float *read_actuators;
    //float *actuators;
    float actuators[NumOfActuatorIds];
    std::vector<float> sensorValues;
    std::vector<float> initialJointSensorValues;

    float *sensor_ptrs[NumOfSensorIds];
    static hal_experimental *instance;
    //TESTS
    unsigned long long testLEDRot;

    int lastTime;
    int mult;
    bool testLEDInitialized, testAliasesInitialized;

    FILE *fp;
    bool cout_debug;
};
#endif
