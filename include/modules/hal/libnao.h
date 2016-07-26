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
-- Created by Paul Heinen 7/22/16

// FILE DESCRIPTION

LibNao is created on startup by the pineapple engine. It uses the
Aldebaran API's to directly write to and read from the DCM/HAL
as well as read and send information to the libpineapple module via
a the 'PineappleJuice' shared memory block found in the 'Fridge'.

Parts of this code are borrowed from the B-Human 2015 Release.

*/
#ifndef LIBNAO_H
#define LIBNAO_H

///Includes
#include "include/common.h"
#include "include/modules/hal/ananas_comosus.h"
#include <boost/shared_ptr.hpp>
#include <exception>
#include <string>

using namespace boost::interprocess;

class Nao : public AL::ALModule
{
public:
    Nao(boost::shared_ptr<AL::ALBroker> pBroker);

    ~Nao();

private:
    static Nao *instance;

    managed_shared_memory shm;
    LPData_Buffer *pineappleJuice; // Shared memory struct

    AL::DCMProxy *proxy;
    AL::ALMemoryProxy *memory;
    AL::ALValue positionRequest;
    AL::ALValue stiffnessRequest;
    AL::ALValue usRequest;
    AL::ALValue ledRequest;

    float *sensorPtrs[lpNumOfSensorIds];
    int dcmTime;

    float requestedActuators[lpNumOfActuatorIds]; /**< The previous actuator values requested. */

    int lastReadingActuators; /**< The previous actuators read. For detecting frames without seemingly new data from bhuman. */
    int actuatorDrops; /**< The number of frames without seemingly new data from bhuman. */
    int frameDrops; /**< The number frames without a reaction from pineapple. */

    enum State {sitting, standingUp, standing, sittingDown, preShuttingDown, preShuttingDownWhileSitting, shuttingDown} state;
    float phase; /**< How far is the Nao in its current standing up or sitting down motion [0 ... 1]? */
    int ledIndex; /**< The index of the last LED set. */

    int rightEarLEDsChangedTime; // Last time when the right ear LEDs were changed by the B-Human code

    float startAngles[lpNumOfPositionActuatorIds]; /**< Start angles for standing up or sitting down. */
    float startStiffness[lpNumOfPositionActuatorIds]; /**< Start stiffness for sitting down. */

    int startPressedTime; /**< The last time the chest button was not pressed. */
    unsigned lastPineappleStartTime;

    static const int allowedFrameDrops = 6; //Frame drops allowed before NAO goes into emergency sitting mode

    void shutdown();
    void setEyeLEDS(float *actuators);
    void setBatteryStatusLEDS(float *actuators);
    void copyServoData(const float *srcActuators, float *destActuators);
    void resetSonar();
    float *state_handler(float *actuators);
    void setActuators();
    void readSensors();
    void readActuators();
    void copyNonServos(const float *srcActuators, float *destActuators);
    static void onPreProcess();
    static void onPostProcess();
    
};

#endif // LIBNAO_H
