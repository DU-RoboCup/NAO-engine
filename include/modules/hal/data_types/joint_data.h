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

Joint Data container which stores  joint angle and stiffness information.
*/

#ifndef _JOINT_DATA_H
#define _JOINT_DATA_H

#include "hardware_data.h" 

struct joint_data {
    joint_data() {
        for(int i = 0; i < Joints::NUMBER_OF_JOINTS; ++i)
        {
            angles[i] = NAN;
            stiffness[i] = NAN;
            temperatures[i] = NAN;
            currents[i] = NAN;    
        }
    }
    //Reset all joint values to zero
    joint_data(bool reset_code)
    {
        for(int i = 0; i < Joints::NUMBER_OF_JOINTS; ++i)
        {
            angles[i] = 0;
            stiffness[i] = 0;
            temperatures[i] = 0;
            currents[i] = 0; 
        }
    }
    float angles[Joints::NUMBER_OF_JOINTS];
    float stiffness[Joints::NUMBER_OF_JOINTS];
    float temperatures[Joints::NUMBER_OF_JOINTS];
    float currents[Joints::NUMBER_OF_JOINTS];

}

#endif