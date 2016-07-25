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
-- Created by Paul Heinen 7/19/16

// FILE DESCRIPTION

LibPineapple directly interfaces with the DCM and the HAL
to read from as well as write to the sensors and actuators
on the NAO robot. This allows more control and effeciency
then what some of the Aldebaran API's potentially offer.

Parts of this code are borrowed from the B-Human 2015 Release.

*/

#ifndef LIBPINEAPPLE_H
#define LIBPINEAPPLE_H

/// Includes
#include "include/common.h"
#include "include/modules/hal/ananas_comosus.h"

using namespace boost::interprocess;



class LibPineapple : public Module, public AL::Module {
public:

    static LibPineapple* Instance();
    void Reconfigure(std::string config_file, uint16_t id);
    bool RunFrame();
    bool ProcessIntent(Intent &i);
    bool Install();
    bool Uninstall();


private:

    LibPineapple();
    static LibPineapple* instance;



    float *sensorPtrs[lpNumOfSensorIds];
    int dcmTime;
    float requestedActuators[lpNumOfActuatorIds];
    int lastReadingActuators;
    int actuatorDrops;
    int frameDrops;
    float phase;
    int ledIndex;
    int rightEarLEDsChangedTime;
    float startAngles[lpNumOfPositionActuatorIds];
    float startStiffness[lpNumOfPositionActuatorIds];

    static const int allowedFrameDrops = 6; //Frame drops allowed before NAO goes into emergency sitting mode

    void shutdown();
    void setEyeLEDS(float *actuators);
    void setBatteryStatusLEDS(float *actuators);
    void copyServoData(const float *srcActuators, float *destActuators);
    void resetSonarMeasurements();
    float *state_handler(float *actuators);
    void setActuators();

    enum State {sitting, standingUp, standing, sittingDown, preShutDown, preSittingShutDown, shuttingDown} state;

};


#endif // LIBPINEAPPLE_H
