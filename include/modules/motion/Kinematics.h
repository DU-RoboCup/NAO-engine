/*
Copyright (c) 2016 "University of Denver"

This file is part of Pineapple.

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
-- Created by Paul Heinen 07/18/2017

// FILE DESCRIPTION

This module incorporates a modified version of the Forward and Inverse Kinematics solver from
the Technical University of Crete to solve Kinematic systems in real-time.

*/

#ifndef _KINEMATICS_H_
#define _KINEMATICS_H_

//Common NAO-Engine includes
#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

//Get access to hardware info
#include "include/util/hardwaremap.h"

class Kinematics : public Module
{

public:
    //Required Module Functions
    static Kinematics* Instance();
    bool RunFrame();
    bool ProcessIntent(Intent &i);
    bool Install();
    bool Uninstall();

    //Function for handling various intents
    void process_and_parse_intents();
    //Function for performing local tests
    void perform_tests();
    //Access the Bazaar
    void initialize_bazaar_access();
    //Get values from Bazaar in a usable form
    bool get_hardware_data();
    //Update Bazaar and notify subscribers
    bool set_hardware_data();

private:
    Kinematics();
    static Kinematics *instance;

    uint32_t sensor_data_subscription_id, actuator_data_subscription_id;
    bool subscribe_return_code;
    //These are both of the boost any dynamic type. That means all original type info
    //is lost in conversion, thus they must be cast back.
    std::shared_ptr<boost::any> sensor_values_dt, actuator_values_dt;
    const float *sensor_values;
    float *actuator_values;

    const std::vector<float> *sensor_vals;
    std::vector<float> *actuator_vals;
    //std::shared_ptr<float *>> sensor_values;
    //std::shared_ptr<float *>> actuator_values;

    PendingIntents pending_intents;
};


#endif /*Kinematics*/