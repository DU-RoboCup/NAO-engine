/*
Copyright (c) 2017 "University of Denver"

This file is part of Project Pineapple.

Pineapple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

VERSION HISTORY
-- Created by Paul Heinen 1/15/17

// FILE DESCRIPTION
Naoqi module for communicating with the hardware on the Nao. This module does not 
follow/implement our custom module system, but rather uses the NaoQi module system,
thus it is built differently. This modules sole role is to send/recieve data from 
the NAO by communicating with the DCM layer.    

Communication Looks like this:
    this_module ---> Sends actuator commands --> DCM <--- NAO electronics recieve values

A more detailed explanation can be found here: http://doc.aldebaran.com/2-1/naoqi/sensors/dcm.html#how-it-works

*/
#ifndef _HAL_ACCESS_H
#define _HAL_ACCESS_H

#include <alproxies/dcmproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <vector>
#include <string>
#include <iostream>
#include "hal_data.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/allocators/allocator.hpp>


using namespace boost::interprocess;

class hal_access : AL::ALModule
{
public: 
    //Input a broker for communication with other NaoQi modules: see http://doc.aldebaran.com/2-1/dev/naoqi/index.html#broker
    hal_access(boost::shared_ptr<AL::ALBroker> broker_ptr, const std::string &broker_name_ptr);

    //DCM loop accessor functions
    void start_loop();
    void stop_loop();
    const static std::string name = "Bender"; //Temporary til this gets implemented later

    virtual ~hal_access();

private:
    //instance
    static hal_access *instance;

    //Initializes the link to the DCM and ALMemory
    void init_link();

    //Initialize fast access to ALMemory
    void init_fast_access();

    //Initialize connection with the main DCM loop
    void connect_dcm_loop();

    //Synchronize the callbacks from the DCM 
    void sync_dcm_callback();

    /////Actuator/Sensor specific functions
    void create_actuator_hardness_alias();
    void create_actuator_position_alias();
    void prepare_actuator_position_command();
    void set_stiffness(const float &stiffness);
    
    //Head and Body ID's and Version number. Useful for compatability checking
    std::string &body_ID; 
    std::string &body_version;
    std::string &head_ID; 
    std::string &head_version;

    //ALValues for communication with the DCM
    AL::DCMProxy *dcm_proxy; //DCMProxy, communicate with the DCM directly with this
    AL::ALValue position_request;
    AL::ALValue stiffness_request;
    AL::ALValue led_request;
    AL::ALValue head_position_request;
    AL::ALValue head_stiffness_request;
    int time_offset;

    //ALMemory
    AL::ALMemoryProxy *nao_memory_proxy; //ALMemory proxy, communicate directly with the NAOs memory with this
    std::vector<float *> sensor_ptrs;
    std::vector<float *> joint_ptrs;
    std::vector<float *> temperature_ptrs;
    std::vector<float *> current_ptrs;
    int *battery_status_ptr;

    //Boost Interprocess Shared Memory
    managed_shared_memory shm;
    named_semaphore semaphore;
    hal_data *pineappleJuice; //Struct to be passed around in shared memory

    //Read the current sensor/actuator values

}

#endif
