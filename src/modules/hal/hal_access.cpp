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

#include "hal_access.h"

hal_access::hal_access(std::shared_ptr<AL::ALBroker> broker_ptr, const &broker_name_ptr) :
    ALModule(broker_ptr, "Pineapple")
{
    try {
        //Initialize DCM and memory proxy
        dcm_proxy = new DCMProxy(broker_ptr);
        if(dcm_proxy == NULL) throw AL::ALError(name, "constructor", "dcm_proxy == NULL");
        
        nao_memory_proxy = new AL::ALMemoryProxy(broker_ptr);
        if(nao_memory_proxy == NULL) throw AL::ALError(name, "constructor", "nao_memory_proxy == NULL");

        body_ID = (std::string) nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0);
        head_ID = (std::string) nao_memory_proxy->getData("RobotConfig/Head/FullHeadId", 0);
        body_version = (std::string) nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0);
        head_version = (std::string) nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0);


    
    }
}

//Module info for NaoQi
extern "C" int _create_module(std::shared_ptr<AL::ALBroker> broker_ptr)
{
    AL::ALModule::createModule<Pineapple>(broker_ptr);
    return 0;
}