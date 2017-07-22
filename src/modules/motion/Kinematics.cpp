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

#include "include/modules/motion/Kinematics.h"


// ================== BEGIN REQUIRED MODULE FUNCTION  ================== //
REGISTER_MODULE(Kinematics);

Kinematics *Kinematics::instance;
Kinematics *Kinematics::Instance()
{
    if(UNLIKELY(Kinematics::instance == 0))
    {
        Kinematics::instance = new Kinematics();
    }
    return Kinematics::instance;
}

bool Kinematics::RunFrame()
{
    // Call calculation and intent processing functions here
    process_and_parse_intents();
    // Update hardware values from the Bazaar
    if(get_hardware_data())
    {
        LOG_DEBUG << "Hardware Data updated!";
        // You now have access to the latest sensor data and current actuator values
        // Call kinematics calculation functions with the array of floats in sensor_values
        // assign results of kinematics calculations to actuator_values.


        //Finally notify subscribers of update.
        set_hardware_data();
    }

    return true;
}

bool Kinematics::ProcessIntent(Intent &i)
{
    pending_intents.push_back(i);
    return true;
}

bool Kinematics::Install()
{
    LOG_DEBUG << "Installed 'Kinematics' Module!";
    return true;
}

bool Kinematics::Uninstall()
{
    Bazaar::Unsubscribe("sensor_data", sensor_data_subscription_id);
    Bazaar::Unsubscribe("actuator_data", actuator_data_subscription_id);
    LOG_DEBUG << "Uninstalled 'Kinematics' Module!";
    return true;
}
// ================== END REQUIRED MODULE FUNCTION  ================== //


Kinematics::Kinematics()
{
    LOG_DEBUG << "Kinematics Constructor Called!";
    //Access Sensor and Actuator values of NAOInterface through the Bazaar
    sensor_data_subscription_id = Bazaar::Subscribe("sensor_data", this->instance, "LOCAL/Kinematics/Subscribe/Sensor_Data");
    actuator_data_subscription_id = Bazaar::Subscribe("actuator_data", this->instance, "LOCAL/Kinematics/Subscribe/Actuator_Data");
    //Verify that we have subscribed
    if(sensor_data_subscription_id && actuator_data_subscription_id)
    {
        //Get pointers of boost::any dynamic type to objects in the Bazaar
        sensor_values_dt = Bazaar::Get("sensor_data");
        actuator_values_dt = Bazaar::Get("actuator_data");
        subscribe_return_code = true;
        LOG_DEBUG << "Bazaar connections for sensor values established";
    }
    else
    {
        LOG_WARNING << "One of the subscription return codes was False";
        subscribe_return_code = false;
    }
}

/**
  * \brief get_hardware_data: Provides direct pointer access to sensor and actuator values.
  *                           You only want to update the actuator values with the calculated kinematics data.
  *                           because of this and because changing sensor vals is pointless, sensor_values is constant.
  **/
bool Kinematics::get_hardware_data()
{
    BOOST_LOG_FUNCTION(); //Traces this function call in the logger
    try
    {
        //Ensure arrays have been created
        if(subscribe_return_code)
        {
            sensor_values = boost::any_cast<float *>(*sensor_values_dt); //immutable
            actuator_values = boost::any_cast<float *>(*actuator_values_dt); //mutable
        }
        else
        {
            LOG_WARNING << "One or both of the values from the Bazaar is a nullptr";
            return false;
        }
        return true;
    } 
    catch(const boost::bad_any_cast &e) 
    {
        LOG_WARNING << "A Boost Bad any_cast has occured: " << e.what();
    }
    return false;
}

/**
  * \brief set_hardware_data: Ensures the data is updated in the Bazaar
  **/
bool Kinematics::set_hardware_data()
{
    //Touch will notify all subscribers that the data has changed with the intent
    //used to subscribe to the data (i.e. NAOInterface)
    Bazaar::Touch("sensor_data");
    Bazaar::Touch("actuator_data");
    return true;
}

/**
  * \brief process_and_parse_intents: Processes and Parses all new intents. Deals with varying intents.
  **/
void Kinematics::process_and_parse_intents()
{
    //TODO: Create specialized intents between Kinematics and NAOInterface to allow for all values required for Kinematics calculations
    LOG_DEBUG << "Processing new intents...";
    if(!pending_intents.empty())
    {
        ParsedIntent new_intent = pending_intents.pop_front().Parse(); //this is a vector<string>
        const std::string module_received_from = new_intent[0];
        //Do stuff below
    }
}

//Perform Local Kinematics test here
void Kinematics::perform_tests()
{
//Guards to ensure tests aren't run when crosscompiled
#ifndef TARGET_IS_CROSS
    LOG_DEBUG << "Performing 'Kinematics' Module tests";
    //Do tests here
#endif
}

