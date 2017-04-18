#ifndef _HAL_EXPERIMENTAL_H
#define _HAL_EXPERIMENTAL_H

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <boost/shared_ptr.hpp>
#include "alcommon/albroker.h"
#include "alcommon/alproxy.h"
#include "alproxies/almemoryproxy.h"
#include "alcommon/almodule.h"
#include "alvalue/alvalue.h"
#include "alproxies/dcmproxy.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <iostream>
#include <string>
#include <cstring>

#include "../../include/memory/hal_data.h"


namespace AL {
    class ALBroker;
}

class hal_experimental : public AL::ALModule {

public:
    hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);
    ~hal_experimental();
    static const std::string name;
	void set_actuators();
    void read_sensors();
    void preCallBack();
    void postCallBack();

    void set_LEDS();
    float* robot_state_handler(float *actuator_vals);
    // Helper functions for setting actuators
    void set_actuators_positions();
    bool set_actuators_stiffness();
    void set_actuators_leds(bool &requested_stiffness_set);
    void print_sensors();
    void print_actuators();
    std::pair<hal_data *, std::size_t> shared_data_ptr; ///< Because for some reason it segfaults just using pineappleJuice...
    /**
      * \brief debug_alvalue: Insanity Check...
      * @param v: An ALValue alias (which is a weird multidemensional jagged array)
      * @param name: Alias name
      **/
    void debug_alvalue(AL::ALValue &v, std::string name);
private:
    AL::DCMProxy *dcm_proxy;
    AL::ALMemoryProxy *nao_memory_proxy;
    AL::ALValue position_request_alias, stiffness_request_alias, led_request_alias, game_controller;
    AL::ALValue commands, commandsAlias;
    //Head and Body ID's and Version number. Useful for compatability checking
    std::string body_ID; 
    std::string body_version;
    std::string head_ID;
    std::string head_version;
    boost::interprocess::managed_shared_memory shm;

	uint8_t last_reading_actuator;
	size_t actuator_update_fails;
	//int dcm_time;
    int dcm_time;
    float last_requested_actuators[NumOfActuatorIds];
    int ledIndex;
    hal_data *pineappleJuice;
    float *read_actuators;
    float *actuators;
    float *sensor_ptrs[NumOfSensorIds];
    static hal_experimental *instance;

    bool cout_debug;
};
#endif