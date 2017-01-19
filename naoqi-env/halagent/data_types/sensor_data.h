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

Sensor Data container which stores the returned sensor values, which includes
joint values, IMU values, FSR values, buttons. Currently excludes sonar data.
*/

#ifndef _SENSOR_DATA_H
#define _SENSOR_DATA_H

#pragma once //Only needs to be included once


#include "hardware_data.h"
#include "joint_data.h"

struct sensor_data {
    sensor_data() {
        for(int i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i) sensors[i] = NAN;
    }
    sensor_data(bool reset) {
        joints = joint_data(true); //sets all joint values to 0
        for(int i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i) sensors[i] = 0;
    }
    joint_data joints;
    float[Sensors::NUMBER_OF_SENSORS];
};

#endif