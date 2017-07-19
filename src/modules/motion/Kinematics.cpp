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
    LOG_DEBUG << "Uninstalled 'Kinematics' Module!";
    return true;
}
// ================== END REQUIRED MODULE FUNCTION  ================== //


Kinematics::Kinematics()
{
    LOG_DEBUG << "Kinematics Constructor Called!";
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

