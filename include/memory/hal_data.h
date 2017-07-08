
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
memory for interprocess comm unication.
*/
#pragma once 
#ifndef _HAL_DATA_H_
#define _HAL_DATA_H_

#include "../util/more_hardware_data.h"
#include <boost/interprocess/sync/named_semaphore.hpp>
//#include <boost/interprocess/sync/interprocess_semaphore.hpp>
//#include <boost/shared_ptr.hpp>
using namespace boost::interprocess;
struct hal_data {


    float sensor_values[NumOfSensorIds];
    float actuator_values[NumOfActuatorIds];
    //boost::interprocess::interprocess_semaphore actuator_semaphore, sensor_semaphore;/*, speak_semaphore;*/
    //boost::interprocess::named_semaphore semaphore;
    hal_data() 
    /*:actuator_semaphore(1), sensor_semaphore(1)/*, speak_semaphore(1)*/
    {}
};

#endif
