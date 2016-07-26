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

// Includes
#include "include/modules/hal/libnao.h"

// Naoqi module stuff
Nao *Nao::instance = 0;
extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
  AL::ALModule::createModule<Nao>(pBroker);
  return 0;
}


using namespace boost::interprocess;

////Borrow from the B-Human 2015 code release
static const char* sensorNames[] =
{
  // joint sensors
  "Device/SubDeviceList/HeadYaw/Position/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Position/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LWristYaw/Position/Sensor/Value",
  "Device/SubDeviceList/LWristYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LWristYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHand/Position/Sensor/Value",
  "Device/SubDeviceList/LHand/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHand/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RWristYaw/Position/Sensor/Value",
  "Device/SubDeviceList/RWristYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RWristYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHand/Position/Sensor/Value",
  "Device/SubDeviceList/RHand/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHand/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Temperature/Sensor/Value",

  // touch sensors
  "Device/SubDeviceList/Head/Touch/Front/Sensor/Value",
  "Device/SubDeviceList/Head/Touch/Middle/Sensor/Value",
  "Device/SubDeviceList/Head/Touch/Rear/Sensor/Value",
  "Device/SubDeviceList/LHand/Touch/Back/Sensor/Value",
  "Device/SubDeviceList/LHand/Touch/Left/Sensor/Value",
  "Device/SubDeviceList/LHand/Touch/Right/Sensor/Value",
  "Device/SubDeviceList/RHand/Touch/Back/Sensor/Value",
  "Device/SubDeviceList/RHand/Touch/Left/Sensor/Value",
  "Device/SubDeviceList/RHand/Touch/Right/Sensor/Value",

  // switches
  "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/ChestBoard/Button/Sensor/Value",

  // inertial sensors
  "Device/SubDeviceList/InertialSensor/GyroscopeX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyroscopeY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyroscopeZ/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerZ/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value",

  // sonar sensors
  "Device/SubDeviceList/US/Left/Sensor/Value",
  "Device/SubDeviceList/US/Left/Sensor/Value1",
  "Device/SubDeviceList/US/Left/Sensor/Value2",
  "Device/SubDeviceList/US/Left/Sensor/Value3",
  "Device/SubDeviceList/US/Left/Sensor/Value4",
  "Device/SubDeviceList/US/Left/Sensor/Value5",
  "Device/SubDeviceList/US/Left/Sensor/Value6",
  "Device/SubDeviceList/US/Left/Sensor/Value7",
  "Device/SubDeviceList/US/Left/Sensor/Value8",
  "Device/SubDeviceList/US/Left/Sensor/Value9",
  "Device/SubDeviceList/US/Right/Sensor/Value",
  "Device/SubDeviceList/US/Right/Sensor/Value1",
  "Device/SubDeviceList/US/Right/Sensor/Value2",
  "Device/SubDeviceList/US/Right/Sensor/Value3",
  "Device/SubDeviceList/US/Right/Sensor/Value4",
  "Device/SubDeviceList/US/Right/Sensor/Value5",
  "Device/SubDeviceList/US/Right/Sensor/Value6",
  "Device/SubDeviceList/US/Right/Sensor/Value7",
  "Device/SubDeviceList/US/Right/Sensor/Value8",
  "Device/SubDeviceList/US/Right/Sensor/Value9",

  // battery sensors
  "Device/SubDeviceList/Battery/Current/Sensor/Value",
  "Device/SubDeviceList/Battery/Charge/Sensor/Value",
  "Device/SubDeviceList/Battery/Temperature/Sensor/Value",

  // fsr sensors
  "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/TotalWeight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/TotalWeight/Sensor/Value",
};

static const char* actuatorNames[] =
{
  "HeadYaw/Position/Actuator/Value",
  "HeadPitch/Position/Actuator/Value",
  "LShoulderPitch/Position/Actuator/Value",
  "LShoulderRoll/Position/Actuator/Value",
  "LElbowYaw/Position/Actuator/Value",
  "LElbowRoll/Position/Actuator/Value",
  "LWristYaw/Position/Actuator/Value",
  "LHand/Position/Actuator/Value",
  "RShoulderPitch/Position/Actuator/Value",
  "RShoulderRoll/Position/Actuator/Value",
  "RElbowYaw/Position/Actuator/Value",
  "RElbowRoll/Position/Actuator/Value",
  "RWristYaw/Position/Actuator/Value",
  "RHand/Position/Actuator/Value",
  "LHipYawPitch/Position/Actuator/Value",
  "LHipRoll/Position/Actuator/Value",
  "LHipPitch/Position/Actuator/Value",
  "LKneePitch/Position/Actuator/Value",
  "LAnklePitch/Position/Actuator/Value",
  "LAnkleRoll/Position/Actuator/Value",
  "RHipRoll/Position/Actuator/Value",
  "RHipPitch/Position/Actuator/Value",
  "RKneePitch/Position/Actuator/Value",
  "RAnklePitch/Position/Actuator/Value",
  "RAnkleRoll/Position/Actuator/Value",

  "HeadYaw/Hardness/Actuator/Value",
  "HeadPitch/Hardness/Actuator/Value",
  "LShoulderPitch/Hardness/Actuator/Value",
  "LShoulderRoll/Hardness/Actuator/Value",
  "LElbowYaw/Hardness/Actuator/Value",
  "LElbowRoll/Hardness/Actuator/Value",
  "LWristYaw/Hardness/Actuator/Value",
  "LHand/Hardness/Actuator/Value",
  "RShoulderPitch/Hardness/Actuator/Value",
  "RShoulderRoll/Hardness/Actuator/Value",
  "RElbowYaw/Hardness/Actuator/Value",
  "RElbowRoll/Hardness/Actuator/Value",
  "RWristYaw/Hardness/Actuator/Value",
  "RHand/Hardness/Actuator/Value",
  "LHipYawPitch/Hardness/Actuator/Value",
  "LHipRoll/Hardness/Actuator/Value",
  "LHipPitch/Hardness/Actuator/Value",
  "LKneePitch/Hardness/Actuator/Value",
  "LAnklePitch/Hardness/Actuator/Value",
  "LAnkleRoll/Hardness/Actuator/Value",
  "RHipRoll/Hardness/Actuator/Value",
  "RHipPitch/Hardness/Actuator/Value",
  "RKneePitch/Hardness/Actuator/Value",
  "RAnklePitch/Hardness/Actuator/Value",
  "RAnkleRoll/Hardness/Actuator/Value",

  "Face/Led/Red/Left/0Deg/Actuator/Value",
  "Face/Led/Red/Left/45Deg/Actuator/Value",
  "Face/Led/Red/Left/90Deg/Actuator/Value",
  "Face/Led/Red/Left/135Deg/Actuator/Value",
  "Face/Led/Red/Left/180Deg/Actuator/Value",
  "Face/Led/Red/Left/225Deg/Actuator/Value",
  "Face/Led/Red/Left/270Deg/Actuator/Value",
  "Face/Led/Red/Left/315Deg/Actuator/Value",
  "Face/Led/Green/Left/0Deg/Actuator/Value",
  "Face/Led/Green/Left/45Deg/Actuator/Value",
  "Face/Led/Green/Left/90Deg/Actuator/Value",
  "Face/Led/Green/Left/135Deg/Actuator/Value",
  "Face/Led/Green/Left/180Deg/Actuator/Value",
  "Face/Led/Green/Left/225Deg/Actuator/Value",
  "Face/Led/Green/Left/270Deg/Actuator/Value",
  "Face/Led/Green/Left/315Deg/Actuator/Value",
  "Face/Led/Blue/Left/0Deg/Actuator/Value",
  "Face/Led/Blue/Left/45Deg/Actuator/Value",
  "Face/Led/Blue/Left/90Deg/Actuator/Value",
  "Face/Led/Blue/Left/135Deg/Actuator/Value",
  "Face/Led/Blue/Left/180Deg/Actuator/Value",
  "Face/Led/Blue/Left/225Deg/Actuator/Value",
  "Face/Led/Blue/Left/270Deg/Actuator/Value",
  "Face/Led/Blue/Left/315Deg/Actuator/Value",
  "Face/Led/Red/Right/0Deg/Actuator/Value",
  "Face/Led/Red/Right/45Deg/Actuator/Value",
  "Face/Led/Red/Right/90Deg/Actuator/Value",
  "Face/Led/Red/Right/135Deg/Actuator/Value",
  "Face/Led/Red/Right/180Deg/Actuator/Value",
  "Face/Led/Red/Right/225Deg/Actuator/Value",
  "Face/Led/Red/Right/270Deg/Actuator/Value",
  "Face/Led/Red/Right/315Deg/Actuator/Value",
  "Face/Led/Green/Right/0Deg/Actuator/Value",
  "Face/Led/Green/Right/45Deg/Actuator/Value",
  "Face/Led/Green/Right/90Deg/Actuator/Value",
  "Face/Led/Green/Right/135Deg/Actuator/Value",
  "Face/Led/Green/Right/180Deg/Actuator/Value",
  "Face/Led/Green/Right/225Deg/Actuator/Value",
  "Face/Led/Green/Right/270Deg/Actuator/Value",
  "Face/Led/Green/Right/315Deg/Actuator/Value",
  "Face/Led/Blue/Right/0Deg/Actuator/Value",
  "Face/Led/Blue/Right/45Deg/Actuator/Value",
  "Face/Led/Blue/Right/90Deg/Actuator/Value",
  "Face/Led/Blue/Right/135Deg/Actuator/Value",
  "Face/Led/Blue/Right/180Deg/Actuator/Value",
  "Face/Led/Blue/Right/225Deg/Actuator/Value",
  "Face/Led/Blue/Right/270Deg/Actuator/Value",
  "Face/Led/Blue/Right/315Deg/Actuator/Value",
  "Ears/Led/Left/36Deg/Actuator/Value",
  "Ears/Led/Left/72Deg/Actuator/Value",
  "Ears/Led/Left/108Deg/Actuator/Value",
  "Ears/Led/Left/144Deg/Actuator/Value",
  "Ears/Led/Left/180Deg/Actuator/Value",
  "Ears/Led/Left/216Deg/Actuator/Value",
  "Ears/Led/Left/252Deg/Actuator/Value",
  "Ears/Led/Left/288Deg/Actuator/Value",
  "Ears/Led/Left/324Deg/Actuator/Value",
  "Ears/Led/Left/0Deg/Actuator/Value",
  "Ears/Led/Right/0Deg/Actuator/Value",
  "Ears/Led/Right/36Deg/Actuator/Value",
  "Ears/Led/Right/72Deg/Actuator/Value",
  "Ears/Led/Right/108Deg/Actuator/Value",
  "Ears/Led/Right/144Deg/Actuator/Value",
  "Ears/Led/Right/180Deg/Actuator/Value",
  "Ears/Led/Right/216Deg/Actuator/Value",
  "Ears/Led/Right/252Deg/Actuator/Value",
  "Ears/Led/Right/288Deg/Actuator/Value",
  "Ears/Led/Right/324Deg/Actuator/Value",
  "ChestBoard/Led/Red/Actuator/Value",
  "ChestBoard/Led/Green/Actuator/Value",
  "ChestBoard/Led/Blue/Actuator/Value",
  "Head/Led/Rear/Left/0/Actuator/Value",
  "Head/Led/Rear/Left/1/Actuator/Value",
  "Head/Led/Rear/Left/2/Actuator/Value",
  "Head/Led/Rear/Right/0/Actuator/Value",
  "Head/Led/Rear/Right/1/Actuator/Value",
  "Head/Led/Rear/Right/2/Actuator/Value",
  "Head/Led/Middle/Right/0/Actuator/Value",
  "Head/Led/Front/Right/0/Actuator/Value",
  "Head/Led/Front/Right/1/Actuator/Value",
  "Head/Led/Front/Left/0/Actuator/Value",
  "Head/Led/Front/Left/1/Actuator/Value",
  "Head/Led/Middle/Left/0/Actuator/Value",
  "LFoot/Led/Red/Actuator/Value",
  "LFoot/Led/Green/Actuator/Value",
  "LFoot/Led/Blue/Actuator/Value",
  "RFoot/Led/Red/Actuator/Value",
  "RFoot/Led/Green/Actuator/Value",
  "RFoot/Led/Blue/Actuator/Value",

  "US/Actuator/Value"
};

static const char* teamInfoNames[] =
{
  "GameCtrl/teamNumber",
  "GameCtrl/teamColour",
  "GameCtrl/playerNumber"
};

static const float sitDownAngles[25] =
{
  0.f,
  0.f,

  0.89f,
  0.06f,
  0.26f,
  -0.62f,
  -1.57f,
  0.f,

  0.89f,
  -0.06f,
  -0.26f,
  0.62f,
  1.57f,
  0.f,

  0.f,
  0.f,
  -0.87f,
  2.16f,
  -1.18f,
  0.f,

  0.f,
  -0.87f,
  2.16f,
  -1.18f,
  0.f
};


Nao::Nao(boost::shared_ptr<AL::ALBroker> pBroker) :
    ALModule(pBroker, "Pineapple"),
    shm(open_or_create, "Fridge", 65536),
    proxy(0), memory(0), dcmTime(0),
    lastReadingActuators(-1),
    actuatorDrops(0),
    frameDrops(allowedFrameDrops + 1),
    state(sitting),
    phase(0.f),
    ledIndex(0),
    rightEarLEDsChangedTime(0),
    startPressedTime(0),
    lastPineappleStartTime(0)
{


    //setModuleDescription("Forms a parasitic relationship with a Pineapple.");

    LOG_DEBUG << "libnao is starting up...Running some quick checks";

    assert(lpNumOfSensorIds == sizeof(sensorNames) / sizeof(*sensorNames));
    assert(lpNumOfActuatorIds == sizeof(actuatorNames) / sizeof(*actuatorNames));
    assert(lpNumOfTeamInfoIds == sizeof(teamInfoNames) / sizeof(*teamInfoNames));

    //Create shared memory
    LOG_DEBUG << "All tests passed. Creating shared memory block 'PineappleJuice'";
    pineappleJuice = shm.construct<LPData_Buffer>("PineappleJuice")();

    LOG_DEBUG << "Shared memory block PineappleJuice was created.";
    LOG_DEBUG << "Attempting to negotiate with naoqi...";

    try {

        memory = new AL::ALMemoryProxy(pBroker); //Gets the robots name
        LOG_INFO << "Setting BodyId";
        std::string bodyId = (std::string) memory->getData("Device/DeviceList/ChestBoard/BodyId", 0);
        strncpy(pineappleJuice->bodyId, bodyId.c_str(), sizeof(pineappleJuice->bodyId));
        pineappleJuice->bodyId[15] = 0;
        LOG_INFO << "BodyId Set";

        LOG_INFO << "Setting Head ID";
        std::string headId = (std::string) memory->getData("RobotConfig/Head/FullHeadId", 0);
        strncpy(pineappleJuice->headId, headId.c_str(), sizeof(pineappleJuice->headId));
        pineappleJuice->headId[15] = 0;
        LOG_INFO << "HeadId set";

        //Create aliases for stiffnessRequest and positionRequest
        proxy = new AL::DCMProxy(pBroker);

        AL::ALValue params, result;
        params.arraySetSize(1);
        params.arraySetSize(2);

        LOG_INFO << "Setting params values for positionActuators alias";
        params[0] = std::string("positionActuators");
        params[1].arraySetSize(lpNumOfPositionActuatorIds);
        for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
            params[1][i] = std::string(actuatorNames[i]);
        result = proxy->createAlias(params);

        LOG_INFO << "Setting params values for stiffnessActuators alias";
        params[0] = std::string("stiffnessActuators");
        params[1].arraySetSize(lpNumOfStiffnessActuatorIds);
        for(int i = 0; i < lpNumOfStiffnessActuatorIds; ++i)
            params[1][i] = std::string(actuatorNames[headYawStiffnessActuator + i]);
        result = proxy->createAlias(params);

        LOG_INFO << "Setting usRequest actuators";
        params[0] = std::string("usRequest");
        params[1].arraySetSize(1);
        params[1][0] = std::string(actuatorNames[usActuator]);
        result = proxy->createAlias(params);

        LOG_INFO << "preparing positionRequest actuators";
        // prepare positionRequest
        positionRequest.arraySetSize(6);
        positionRequest[0] = std::string("positionActuators");
        positionRequest[1] = std::string("ClearAll");
        positionRequest[2] = std::string("time-separate");
        positionRequest[3] = 0;
        positionRequest[4].arraySetSize(1);
        positionRequest[5].arraySetSize(lpNumOfPositionActuatorIds);
        for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
          positionRequest[5][i].arraySetSize(1);

        LOG_INFO << "Preparing stiffnessRequest";
        // prepare stiffnessRequest
        stiffnessRequest.arraySetSize(6);
        stiffnessRequest[0] = std::string("stiffnessActuators");
        stiffnessRequest[1] = std::string("ClearAll");
        stiffnessRequest[2] = std::string("time-separate");
        stiffnessRequest[3] = 0;
        stiffnessRequest[4].arraySetSize(1);
        stiffnessRequest[5].arraySetSize(lpNumOfStiffnessActuatorIds);
        for(int i = 0; i < lpNumOfStiffnessActuatorIds; ++i)
          stiffnessRequest[5][i].arraySetSize(1);

        LOG_INFO << "Preparing usRequest";
        // prepare sonar Request
        usRequest.arraySetSize(6);
        usRequest[0] = std::string("usRequest");
        usRequest[1] = std::string("Merge"); // doesn't work with "ClearAll"
        usRequest[2] = std::string("time-separate");
        usRequest[3] = 0;
        usRequest[4].arraySetSize(1);
        usRequest[5].arraySetSize(1);
        usRequest[5][0].arraySetSize(1);

        LOG_INFO << "Preparing ledRequest";
        // prepare ledRequest
        ledRequest.arraySetSize(3);
        ledRequest[1] = std::string("ClearAll");
        ledRequest[2].arraySetSize(1);
        ledRequest[2][0].arraySetSize(2);
        ledRequest[2][0][1] = 0;

        LOG_INFO << "Preparing sensor pointers";
        // prepare sensor pointers
        for(int i = 0; i < lpNumOfSensorIds; ++i)
          sensorPtrs[i] = (float*) memory->getDataPtr(sensorNames[i]);
        resetSonar();

        LOG_INFO << "Initializing requested actuators";
        // initialize requested actuators
        memset(requestedActuators, 0, sizeof(requestedActuators));
        for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
          requestedActuators[i] = -1.f;

        LOG_INFO << "Registering onPreProcess and onPostProcess callbacks";
        // register "onPreProcess" and "onPostProcess" callbacks
        instance = this;
        proxy->getGenericProxy()->getModule()->atPreProcess(&onPreProcess);
        proxy->getGenericProxy()->getModule()->atPostProcess(&onPostProcess);

        LOG_DEBUG << "libpineapple successfully started!";
        return; // success
      } catch(AL::ALError& e) {
        LOG_FATAL << "libpineapple failed to start because: " << e.what();
      }
}
//}
//close(); // error
//}

//}
Nao::~Nao()
{
    shutdown();
}

void Nao::shutdown()
{
    LOG_WARNING << "libnao is shutting down";
    //Clear the shared memory


    if(proxy)
    {
        proxy->getGenericProxy()->getModule()->removeAllPreProcess();
        proxy->getGenericProxy()->getModule()->removeAllPostProcess();
        LOG_INFO << "Deleting proxy";
        delete proxy;
    }

    if(memory)
    {
        LOG_INFO << "Deleting access to DCM Memory";
        delete memory;
    }

    LOG_INFO << "Removing shared memory block";
    shared_memory_object::remove("Fridge");

    LOG_WARNING << "libnao has successfully shut down";
}

void Nao::setEyeLEDS(float *actuators)
{
    for(int i = faceLedRedLeft0DegActuator; i <= faceLedBlueRight315DegActuator; ++i)
        actuators[i] = 0.f;

    if(state == shuttingDown)
    {
        actuators[faceLedRedLeft180DegActuator] = 1.f;
        actuators[faceLedRedRight180DegActuator] = 1.f;
    } else if (pineappleJuice->state != okState) {

        float error_blink = float(dcmTime / 500 & 1);
        for(int i = faceLedRedLeft0DegActuator; i <= faceLedRedLeft315DegActuator; ++i)
            actuators[i] = error_blink;
        for(int i = faceLedRedRight0DegActuator; i < faceLedRedRight315DegActuator; ++i)
            actuators[i] = error_blink;
    } else {
        //Normal running mode
        float okstate_blink = float(dcmTime / 500 & 1);
        actuators[faceLedBlueLeft180DegActuator] = okstate_blink;
        actuators[faceLedBlueRight180DegActuator] = okstate_blink;
    }
}

void Nao::setBatteryStatusLEDS(float *actuators)
{
    //Battery status appears on the right ear
    for(int i = earsLedRight0DegActuator; i < earsLedRight324DegActuator; ++i)
    {
        if(actuators[i] != requestedActuators[i])
        {
            rightEarLEDsChangedTime = dcmTime;
            requestedActuators[i] = actuators[i];
        }

        if(state != standing || dcmTime - rightEarLEDsChangedTime > 5000)
        {
            for(int i = 0; i < int(*sensorPtrs[batteryChargeSensor] * 10.f) && i < 10; ++i)
                actuators[earsLedLeft0DegActuator + i] = 1.f;
        }
    }
}

float* Nao::state_handler(float *actuators)
{
    static float controlledActuators[lpNumOfActuatorIds];

    switch(state)
    {
        sitting: state = sitting;
        case sitting:
            std::memset(controlledActuators, 0, sizeof(controlledActuators));
            if(frameDrops > allowedFrameDrops || (actuators[lHipPitchStiffnessActuator] == 0.f && actuators[rHipPitchStiffnessActuator] == 0.f))
                return controlledActuators;
            for(int i = 0; i < lpNumOfActuatorIds; ++i)
                startAngles[i] = *sensorPtrs[i * 3];

        standingUp:
            state = standingUp;
            phase = 0.f;
        case standingUp:
            if(phase < 1.f && frameDrops <= allowedFrameDrops)
            {
                std::memset(controlledActuators, 0, sizeof(controlledActuators));
                phase = std::min(phase + 0.005f, 1.f);
                for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                {
                    controlledActuators[i] = actuators[i] * phase + startAngles[i] * (1.f - phase);
                    float h = std::min(actuators[i + headYawStiffnessActuator], 0.5f);
                    controlledActuators[i + headYawStiffnessActuator] = actuators[i + headYawStiffnessActuator] * phase + h * (1.f - phase);
                }
                return controlledActuators;
            }
            state = standing;


        case standing: if(frameDrops <= allowedFrameDrops) return actuators;

        case preShuttingDown:
            for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
            {
                startAngles[i] = positionRequest[5][i][0];
                if(actuators[lHipPitchStiffnessActuator] = 0.f && actuators[rHipPitchStiffnessActuator] == 0.f)
                    startStiffness[i] = 0.f;
                else if(i >= lShoulderPitchPositionActuator && i <= rElbowRollPositionActuator)
                    startStiffness[i] = 0.4f;
                else
                    startStiffness[i] = std::min<float>(stiffnessRequest[5][i][0], 0.3f);
            }
            state = state == preShuttingDown ? shuttingDown : sittingDown;
            phase = 0.f;

        case shuttingDown:
            shuttingDown:
                if((phase < 1.f && frameDrops > allowedFrameDrops) || state == shuttingDown)
                {
                    std::memset(controlledActuators, 0, sizeof(controlledActuators));
                    phase =  std::min(phase + 0.005f, 1.f);
                    for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                    {
                        controlledActuators[i] = sitDownAngles[i] * phase + startAngles[i] * (1.f - phase);
                        controlledActuators[i + headYawStiffnessActuator] = startStiffness[i];
                    }
                    return controlledActuators;
                } else if(frameDrops <= allowedFrameDrops) {
                    for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                        startAngles[i] = positionRequest[5][i][0];
                    goto standingUp;
                } else {
                    goto sitting;
                }
        case preShuttingDownWhileSitting:
            for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                startStiffness[i] = 0.f;
            state = shuttingDown;
            phase = 0.995f;
            goto shuttingDown;
    }
}

void Nao::copyNonServos(const float *srcActuators, float *destActuators)
{
    for(int i = faceLedRedLeft0DegActuator; i < lpNumOfActuatorIds; ++i)
        destActuators[i] = srcActuators[i];
}

void Nao::resetSonar()
{
    for(int i = lUsSensor; i <= rUs9Sensor; ++i)
        *sensorPtrs[i] = 0.f;
}

void Nao::setActuators()
{
#ifdef DEBUG
    BOOST_LOG_FUNCTION();
    LOG_DEBUG << "Trying to set actuator values";
#endif
    try {
        //Set actuators to values stored in the shared memory
        dcmTime = proxy->getTime(0);

        pineappleJuice->readingActuators = pineappleJuice->newestActuators;
        LOG_TRACE << "readingActuators values updated";

        if(pineappleJuice->readingActuators == lastReadingActuators)
        {
            if(actuatorDrops == 0)
                LOG_FATAL << "Actuator request was missed";
            ++actuatorDrops;
        } else {
            actuatorDrops = 0;
        }

        lastReadingActuators = pineappleJuice->readingActuators;
        float *readingActuators = pineappleJuice->actuators[pineappleJuice->readingActuators];
        float *actuators = state_handler(readingActuators);


        if(state != standing)
        {
            if(frameDrops > 0 || state == shuttingDown)
                setEyeLEDS(actuators);
            else
                copyServoData(readingActuators, actuators);
        }
        setBatteryStatusLEDS(actuators);

        //Setting position actuators
        positionRequest[4][0] = dcmTime;
        for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
            positionRequest[5][i][0] = actuators[i];
        proxy->setAlias(positionRequest);

        //Setting stiffness actuators
        bool requestedStiffness = false;
        for(int i = headYawStiffnessActuator; i < headYawStiffnessActuator + lpNumOfStiffnessActuatorIds; ++i)
        {
            if(actuators[i] != requestedActuators[i])
            {
                stiffnessRequest[4][0] = dcmTime; // sets a delay of 0
                for(int j = 0; j < lpNumOfStiffnessActuatorIds; ++j) //assign the stiffness to the alias 'stiffnessRequest'
                    stiffnessRequest[5][j][0] = requestedActuators[headYawStiffnessActuator + j] = actuators[headYawStiffnessActuator + j];
                proxy->setAlias(stiffnessRequest);
                requestedStiffness = true;
                break;
            }
        }

        //set Sonar actuator
        bool requestedSonar = false;
        if(requestedActuators[usActuator] != actuators[usActuator])
        {
            requestedActuators[usActuator] = actuators[usActuator];
            if(actuators[usActuator] >= 0.f)
            {
                resetSonar();
                usRequest[4][0] = dcmTime;
                usRequest[5][0][0] = actuators[usActuator];
                proxy->setAlias(usRequest);
                requestedSonar = true;
            }
        }

        //set LEDs
        if(!requestedStiffness && !requestedSonar)
        {
            for(int i = 0; i < lpNumOfLedActuatorIds; ++i)
            {
                int j = faceLedRedLeft0DegActuator + ledIndex;
                if(++ledIndex == lpNumOfLedActuatorIds) ledIndex = 0;
                if(actuators[j] != requestedActuators[j])
                {
                    ledRequest[0] = std::string(actuatorNames[j]);
                    ledRequest[2][0][0] = requestedActuators[j] = actuators[j];
                    ledRequest[2][0][1] = dcmTime;
                    proxy->set(ledRequest);
                    break;
                }
            }
        }

        //Team info
        if(pineappleJuice->pineappleStartTime != lastPineappleStartTime)
        {
            for(int i = 0; i < lpNumOfTeamInfoIds; ++i)
                memory->insertData(teamInfoNames[i], pineappleJuice->teamInfo[i]);

            lastPineappleStartTime = pineappleJuice->pineappleStartTime;
        }
    } catch(AL::ALError &ex) {
        LOG_FATAL << "There was a problem with setting the actuators: " << ex.what();
    }
}

void Nao::readSensors()
{
    try
    {
        //copy sensor vals into shared memory
        int writerCounter = 0;
        if(writerCounter == pineappleJuice->newestSensors) ++writerCounter;
        if((writerCounter == pineappleJuice->readingSensors) && (++writerCounter == pineappleJuice->newestSensors)) ++writerCounter;

        //Tests
        assert(writerCounter != pineappleJuice->newestSensors);
        assert(writerCounter != pineappleJuice->readingSensors);

        float *sensors = pineappleJuice->sensors[writerCounter];
        for(int i = 0; i < lpNumOfSensorIds; ++i)
            sensors[i] = *sensorPtrs[i];
        /**
         * @brief PineappleMcPineappleFace: needs the robocup control game data to work.

        AL::ALValue PineappleMcPineappleFace = memory->getData("GameCtrl/RoboCupGameControlData");
        if(PineappleMcPineappleFace.isBinary() && PineappleMcPineappleFace.getSize() == sizeof(RoboCup::RoboCupGameControlData))
            std::memcpy(&pineappleJuice->gameControlData[writerCounter], PineappleMcPineappleFace, sizeof(RoboCup::RoboCupGameControlData));
            */

        pineappleJuice->newestSensors = writerCounter;


        //If chest button pressed (3s) send a shutdown request
        if(*sensorPtrs[chestButtonSensor] == 0.f)
            startPressedTime = dcmTime;
        else if(state != shuttingDown && startPressedTime && dcmTime - startPressedTime > 3000)
        {
            //Make sure this actually works on tests or our pineapples will never turn off
            (void) !system("( /home/nao/bin/pineapple stop && sudo shutdown -h now ) &");
            state = (state == sitting ? preShuttingDownWhileSitting : preShuttingDown);
        }
    } catch (AL::ALError &ex) {
        LOG_FATAL << "Failed to read sensors: " << ex.what();
    }
    //Semaphore stuff is supposed to go here
}


// This gets called by NaoQi before communication with the chest board
void Nao::onPreProcess()
{
    instance->setActuators();
}

// called immedietly after communication with chest board to get sensor values.
void Nao::onPostProcess()
{
    instance->readSensors();
}



