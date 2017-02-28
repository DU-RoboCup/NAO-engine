
/*
Copyright (c) 2017 "University of Denver"

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
-- Created by Paul Heinen 1/16/17

// FILE DESCRIPTION
Container for all hal data on the NAO. This container will be inserted into shared
memory for interprocess communication.
*/
#ifndef _HAL_DATA_H
#define _HAL_DATA_H

//#include "data_types/joint_data.h"
//#include "data_types/sensor_data.h"
#include "data_types/more_hardware_data.h"
#include <boost/interprocess/sync/named_semaphore.hpp>

struct hal_data {
    unsigned int sensors_newest_update; ///< Index of the sensor with the most recently updated value 
    unsigned int sensors_newest_read; ///< Index from the list of the current sensor being read
    unsigned int actuators_newest_update; ///< Index of the actuator with the most recently updated value
    unsigned int actuators_current_read; ///< Index from the actuator of the current sensor being read

    //joint_data joints[3];

    char text_to_speak[3][35]; //Text to have the NAO say
    bool standing;
    
	float sensors[3][NumOfSensorIds];
	float actuators[3][NumOfActuatorIds];

    boost::interprocess::named_semaphore semaphore;

    hal_data() : semaphore(boost::interprocess::open_or_create, "robo_semaphore", 0)
    {}
};

#endif
