
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

struct hal_data {
    volatile uint8_t sensors_read;
    volatile uint8_t sensors_current_read; //value of the latest reading
    volatile uint8_t actuators_read;
    volatile uint8_t actuators_current_read;

    //joint_data joints[3];

    char text_to_speak[3][35]; //Text to have the NAO say

    volatile bool standing;
    void init()
    {
        sensors_read = 0;
        sensors_current_read = 0;
        actuators_read = 0;
        actuators_current_read = 0;
        for(int i = 0; i < 3; ++i) text_to_speak[i][0] = 0;
    }
    
};

#endif
