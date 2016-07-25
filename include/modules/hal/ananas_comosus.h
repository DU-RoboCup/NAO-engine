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

"The pineapple (Ananas comosus) is a tropical plant with edible multiple fruit consisting
of coalesced berries, also called pineapples,[2][3] and the most economically significant
plant in the Bromeliaceae family.[4]." (wikipedia)

ananas_comosus.h contains all of the data structures and variables needed for the IPC between
libnao and libpineapple.

Parts of this code are borrowed from the B-Human 2015 Release.

*/
#ifndef ANANAS_COMOSUS_H
#define ANANAS_COMOSUS_H

/// Includes
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <alcommon/albroker.h>
#include <alcommon/alproxy.h>
#include <alproxies/dcmproxy.h>
#include <alproxies/almemoryproxy.h>
#undef BOOST_SIGNALS_NO_DEPRECATION_WARNING

using namespace boost::interprocess;


enum LPSensorIds
{
  // joint sensors
  headYawPositionSensor,
  headYawCurrentSensor,
  headYawTemperatureSensor,
  headPitchPositionSensor,
  headPitchCurrentSensor,
  headPitchTemperatureSensor,
  lShoulderPitchPositionSensor,
  lShoulderPitchCurrentSensor,
  lShoulderPitchTemperatureSensor,
  lShoulderRollPositionSensor,
  lShoulderRollCurrentSensor,
  lShoulderRollTemperatureSensor,
  lElbowYawPositionSensor,
  lElbowYawCurrentSensor,
  lElbowYawTemperatureSensor,
  lElbowRollPositionSensor,
  lElbowRollCurrentSensor,
  lElbowRollTemperatureSensor,
  lWristYawPositionSensor,
  lWristYawCurrentSensor,
  lWristYawTemperaturSensor,
  lHandPositionSensor,
  lHandCurrentSensor,
  lHandTemperaturSensor,
  rShoulderPitchPositionSensor,
  rShoulderPitchCurrentSensor,
  rShoulderPitchTemperatureSensor,
  rShoulderRollPositionSensor,
  rShoulderRollCurrentSensor,
  rShoulderRollTemperatureSensor,
  rElbowYawPositionSensor,
  rElbowYawCurrentSensor,
  rElbowYawTemperatureSensor,
  rElbowRollPositionSensor,
  rElbowRollCurrentSensor,
  rElbowRollTemperatureSensor,
  rWristYawPositionSensor,
  rWristYawCurrentSensor,
  rWristYawTemperaturSensor,
  rHandPositionSensor,
  rHandCurrentSensor,
  rHandTemperaturSensor,
  lHipYawPitchPositionSensor,
  lHipYawPitchCurrentSensor,
  lHipYawPitchTemperatureSensor,
  lHipRollPositionSensor,
  lHipRollCurrentSensor,
  lHipRollTemperatureSensor,
  lHipPitchPositionSensor,
  lHipPitchCurrentSensor,
  lHipPitchTemperatureSensor,
  lKneePitchPositionSensor,
  lKneePitchCurrentSensor,
  lKneePitchTemperatureSensor,
  lAnklePitchPositionSensor,
  lAnklePitchCurrentSensor,
  lAnklePitchTemperatureSensor,
  lAnkleRollPositionSensor,
  lAnkleRollCurrentSensor,
  lAnkleRollTemperatureSensor,
  rHipRollPositionSensor,
  rHipRollCurrentSensor,
  rHipRollTemperatureSensor,
  rHipPitchPositionSensor,
  rHipPitchCurrentSensor,
  rHipPitchTemperatureSensor,
  rKneePitchPositionSensor,
  rKneePitchCurrentSensor,
  rKneePitchTemperatureSensor,
  rAnklePitchPositionSensor,
  rAnklePitchCurrentSensor,
  rAnklePitchTemperatureSensor,
  rAnkleRollPositionSensor,
  rAnkleRollCurrentSensor,
  rAnkleRollTemperatureSensor,

  // touch sensors
  headTouchFrontSensor,
  headTouchMiddleSensor,
  headTouchRearSensor,
  lHandTouchBackSensor,
  lHandTouchLeftSensor,
  lHandTouchRightSensor,
  rHandTouchBackSensor,
  rHandTouchLeftSensor,
  rHandTouchRightSensor,

  // switches
  lBumperLeftSensor,
  lBumperRightSensor,
  rBumperLeftSensor,
  rBumperRightSensor,
  chestButtonSensor,

  // inertial sensors
  gyroXSensor,
  gyroYSensor,
  gyroZSensor,
  accXSensor,
  accYSensor,
  accZSensor,
  angleXSensor,
  angleYSensor,

  // sonar sensors
  lUsSensor,
  lUs1Sensor,
  lUs2Sensor,
  lUs3Sensor,
  lUs4Sensor,
  lUs5Sensor,
  lUs6Sensor,
  lUs7Sensor,
  lUs8Sensor,
  lUs9Sensor,
  rUsSensor,
  rUs1Sensor,
  rUs2Sensor,
  rUs3Sensor,
  rUs4Sensor,
  rUs5Sensor,
  rUs6Sensor,
  rUs7Sensor,
  rUs8Sensor,
  rUs9Sensor,

  // battery sensors
  batteryCurrentSensor,
  batteryChargeSensor,
  batteryTemperatureSensor,

  // fsr sensors
  lFSRFrontLeftSensor,
  lFSRFrontRightSensor,
  lFSRRearLeftSensor,
  lFSRRearRightSensor,
  rFSRFrontLeftSensor,
  rFSRFrontRightSensor,
  rFSRRearLeftSensor,
  rFSRRearRightSensor,
  lFSRTotalSensor,
  rFSRTotalSensor,

  lpNumOfSensorIds,
};

enum LPActuatorIds
{
  // joint request
  headYawPositionActuator,
  headPitchPositionActuator,
  lShoulderPitchPositionActuator,
  lShoulderRollPositionActuator,
  lElbowYawPositionActuator,
  lElbowRollPositionActuator,
  lWristYawPositionActuator,
  lHandPositionActuator,
  rShoulderPitchPositionActuator,
  rShoulderRollPositionActuator,
  rElbowYawPositionActuator,
  rElbowRollPositionActuator,
  rWristYawPositionActuator,
  rHandPositionActuator,
  lHipYawPitchPositionActuator,
  lHipRollPositionActuator,
  lHipPitchPositionActuator,
  lKneePitchPositionActuator,
  lAnklePitchPositionActuator,
  lAnkleRollPositionActuator,
  rHipRollPositionActuator,
  rHipPitchPositionActuator,
  rKneePitchPositionActuator,
  rAnklePitchPositionActuator,
  rAnkleRollPositionActuator,
  lpNumOfPositionActuatorIds,

  // stiffness request
  headYawStiffnessActuator = lpNumOfPositionActuatorIds,
  headPitchStiffnessActuator,
  lShoulderPitchStiffnessActuator,
  lShoulderRollStiffnessActuator,
  lElbowYawStiffnessActuator,
  lElbowRollStiffnessActuator,
  lWristYawStiffnessActuator,
  lHandStiffnessActuator,
  rShoulderPitchStiffnessActuator,
  rShoulderRollStiffnessActuator,
  rElbowYawStiffnessActuator,
  rElbowRollStiffnessActuator,
  rWristYawStiffnessActuator,
  rHandStiffnessActuator,
  lHipYawPitchStiffnessActuator,
  lHipRollStiffnessActuator,
  lHipPitchStiffnessActuator,
  lKneePitchStiffnessActuator,
  lAnklePitchStiffnessActuator,
  lAnkleRollStiffnessActuator,
  rHipRollStiffnessActuator,
  rHipPitchStiffnessActuator,
  rKneePitchStiffnessActuator,
  rAnklePitchStiffnessActuator,
  rAnkleRollStiffnessActuator,

  // led request
  faceLedRedLeft0DegActuator,
  faceLedRedLeft45DegActuator,
  faceLedRedLeft90DegActuator,
  faceLedRedLeft135DegActuator,
  faceLedRedLeft180DegActuator,
  faceLedRedLeft225DegActuator,
  faceLedRedLeft270DegActuator,
  faceLedRedLeft315DegActuator,
  faceLedGreenLeft0DegActuator,
  faceLedGreenLeft45DegActuator,
  faceLedGreenLeft90DegActuator,
  faceLedGreenLeft135DegActuator,
  faceLedGreenLeft180DegActuator,
  faceLedGreenLeft225DegActuator,
  faceLedGreenLeft270DegActuator,
  faceLedGreenLeft315DegActuator,
  faceLedBlueLeft0DegActuator,
  faceLedBlueLeft45DegActuator,
  faceLedBlueLeft90DegActuator,
  faceLedBlueLeft135DegActuator,
  faceLedBlueLeft180DegActuator,
  faceLedBlueLeft225DegActuator,
  faceLedBlueLeft270DegActuator,
  faceLedBlueLeft315DegActuator,
  faceLedRedRight0DegActuator,
  faceLedRedRight45DegActuator,
  faceLedRedRight90DegActuator,
  faceLedRedRight135DegActuator,
  faceLedRedRight180DegActuator,
  faceLedRedRight225DegActuator,
  faceLedRedRight270DegActuator,
  faceLedRedRight315DegActuator,
  faceLedGreenRight0DegActuator,
  faceLedGreenRight45DegActuator,
  faceLedGreenRight90DegActuator,
  faceLedGreenRight135DegActuator,
  faceLedGreenRight180DegActuator,
  faceLedGreenRight225DegActuator,
  faceLedGreenRight270DegActuator,
  faceLedGreenRight315DegActuator,
  faceLedBlueRight0DegActuator,
  faceLedBlueRight45DegActuator,
  faceLedBlueRight90DegActuator,
  faceLedBlueRight135DegActuator,
  faceLedBlueRight180DegActuator,
  faceLedBlueRight225DegActuator,
  faceLedBlueRight270DegActuator,
  faceLedBlueRight315DegActuator,
  earsLedLeft0DegActuator,
  earsLedLeft36DegActuator,
  earsLedLeft72DegActuator,
  earsLedLeft108DegActuator,
  earsLedLeft144DegActuator,
  earsLedLeft180DegActuator,
  earsLedLeft216DegActuator,
  earsLedLeft252DegActuator,
  earsLedLeft288DegActuator,
  earsLedLeft324DegActuator,
  earsLedRight0DegActuator,
  earsLedRight36DegActuator,
  earsLedRight72DegActuator,
  earsLedRight108DegActuator,
  earsLedRight144DegActuator,
  earsLedRight180DegActuator,
  earsLedRight216DegActuator,
  earsLedRight252DegActuator,
  earsLedRight288DegActuator,
  earsLedRight324DegActuator,
  chestBoardLedRedActuator,
  chestBoardLedGreenActuator,
  chestBoardLedBlueActuator,
  headLedRearLeft0Actuator,
  headLedRearLeft1Actuator,
  headLedRearLeft2Actuator,
  headLedRearRight0Actuator,
  headLedRearRight1Actuator,
  headLedRearRight2Actuator,
  headLedMiddleRight0Actuator,
  headLedFrontRight0Actuator,
  headLedFrontRight1Actuator,
  headLedFrontLeft0Actuator,
  headLedFrontLeft1Actuator,
  headLedMiddleLeft0Actuator,
  lFootLedRedActuator,
  lFootLedGreenActuator,
  lFootLedBlueActuator,
  rFootLedRedActuator,
  rFootLedGreenActuator,
  rFootLedBlueActuator,
  lpNumOfNormalActuatorIds,

  usActuator = lpNumOfNormalActuatorIds,

  lpNumOfActuatorIds
};

enum LPTeamInfoIds
{
  teamNumber,
  teamColor,
  playerNumber,
  lpNumOfTeamInfoIds
};

const int lpNumOfStiffnessActuatorIds = lpNumOfPositionActuatorIds;
const int lpNumOfLedActuatorIds = rFootLedBlueActuator + 1 - faceLedRedLeft0DegActuator;

enum PState
    {
      okState = 0,
      abnormalTerminationState = 1,
      sigINTState = 2,
      sigQUITState = 3,
      sigILLState = 4,
      sigABRTState = 6,
      sigFPEState = 8,
      sigKILLState = 9,
      sigSEGVState = 11,
      sigPIPEState = 13,
      sigALRMState = 14,
      sigTERMState = 15,
    };


/* Structs */

/**
 * @brief The LPData struct: Stores all the sensor and actuator values and id's.
 */
struct LPData_Buffer
{
    //Semaphore will start with writer enabled;
    //LData_Buffer(): writer(1), reader(0) {}
    //interprocess_semaphore writer, reader;

    volatile int readingSensors; /**< Index of sensor data reserved for reading. */
    volatile int newestSensors; /**< Index of the newest sensor data. */
    volatile int readingActuators; /**< Index of actuator commands reserved for reading. */
    volatile int newestActuators; /**< Index of the newest actuator command. */

    char bodyId[16]; /* Device/DeviceList/ChestBoard/BodyId */
    char headId[16]; /* RobotConfig/Head/FullHeadId */
    float sensors[3][lpNumOfSensorIds];
    float actuators[3][lpNumOfActuatorIds];

    //RoboCup::RoboCupGameControlData gameControlData[3];

    PState state;
    int teamInfo[lpNumOfTeamInfoIds];
    unsigned pineappleStartTime;
};

/**
 * @brief The shm_remove struct removes the shared memory pool 'PineappleJuice'
 */
//struct shm_remove
//{
//   shm_remove() { shared_memory_object::remove("PineappleJuice"); }
//   ~shm_remove(){ shared_memory_object::remove("PineappleJuice"); }
//} remove_shared_memory;

#endif // ANANAS_COMOSUS_H
