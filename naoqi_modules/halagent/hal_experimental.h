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
	float dcm_time;
    float last_requested_actuators[NumOfActuatorIds];



    hal_data *pineappleJuice;
    float *sensor_ptrs[NumOfSensorIds];
    static hal_experimental *instance;

};
#endif