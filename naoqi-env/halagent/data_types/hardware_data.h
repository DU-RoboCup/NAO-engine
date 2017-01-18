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
-- Created by Paul Heinen 1/15/17

// FILE DESCRIPTION
This file stores data regarding the hardware of the robot. 
Values are from the NaoQi docs.

*/

#ifndef _HARDWARE_DATA_H
#define _HARDWARE_DATA_H

#include <string>


namespace Joints {

    typedef enum JointNames {
        HeadYaw = 0,
        HeadPitch,
        RShoulderRoll,
        RShoulderPitch,
        RElbowRoll,
        RElbowYaw,
        RWristYaw,
        RHand,
        LShoulderRoll,
        LShoulderPitch,
        LElbowYaw,
        LElbowRoll,
        LWristYaw,
        LHand,
        RHipYawPitch,
        RHipPitch,
        RHipRoll,
        RKneePitch,
        RAnklePitch,
        RAnkleRoll,
        LHipYawPitch,
        LHipPitch,
        LHipRoll,
        LKneePitch,
        LAnklePitch,
        LAnkleRoll,
        NUMBER_OF_JOINTS
    } JointNames;

    const JointNames joint_names[] = {
        HeadYaw,
        HeadPitch,
        RShoulderRoll,
        RShoulderPitch,
        RElbowRoll,
        RElbowYaw,
        RWristYaw,
        RHand,
        LShoulderRoll,
        LShoulderPitch,
        LElbowYaw,
        LElbowRoll,
        LWristYaw,
        LHand,
        RHipYawPitch,
        RHipPitch,
        RHipRoll,
        RKneePitch,
        RAnklePitch,
        RAnkleRoll,
        LHipYawPitch,
        LHipPitch,
        LHipRoll,
        LKneePitch,
        LAnklePitch,
        LAnkleRoll,
    };

    const std::string joint_names[] = {
        "HeadYaw",
        "HeadPitch",
        "RShoulderRoll",
        "RShoulderPitch",
        "RElbowRoll",
        "RElbowYaw",
        "RWristYaw",
        "RHand",
        "LShoulderRoll",
        "LShoulderPitch",
        "LElbowYaw",
        "LElbowRoll",
        "LWristYaw",
        "LHand",
        "RHipYawPitch",
        "RHipPitch",
        "RHipRoll",
        "RKneePitch",
        "RAnklePitch",
        "RAnkleRoll",
        "LHipYawPitch",
        "LHipPitch",
        "LHipRoll",
        "LKneePitch",
        "LAnklePitch",
        "LAnkleRoll",
    };

    namespace JointLimits {
        // Note: All these values are in degrees

        const float HeadYaw_Min = -119.5;
        const float HeadYaw_Max = 119.5;
        const float HeadPitch_Min = -38.5;
        const float HeadPitch_Max = 29.5;
        const float LShoulderPitch_Min = -119.5;
        const float LShoulderPitch_Max = 119.5;
        const float LShoulderRoll_Min = -18.0;
        const float LShoulderRoll_Max = 76.0;
        const float LElbowRoll_Min = -88.5;
        const float LElbowRoll_Max = -2.0;
        const float LElbowYaw_Min = -119.5;
        const float LElbowYaw_Max = 119.5;
        const float LWristYaw_Min = -104.5;
        const float LWristYaw_Max = 104.5;
        const float LHand_Min = 0.0;
        const float LHand_Max = 57.2958;
        const float LHipYawPitch_Min = -65.62;
        const float LHipYawPitch_Max = 42.44;
        const float LHipPitch_Min = -101.63;
        const float LHipPitch_Max = 27.73;
        const float LHipRoll_Min = -21.74;
        const float LHipRoll_Max = 45.29;
        const float LKneePitch_Min = -5.29;
        const float LKneePitch_Max = 121.04;
        const float LAnklePitch_Min = -68.15;
        const float LAnklePitch_Max = 52.86;
        const float LAnkleRoll_Min = -22.79;
        const float LAnkleRoll_Max = 44.06;
        const float RHipPitch_Min = -101.54;
        const float RHipPitch_Max = 27.82;
        const float RHipRoll_Min = -42.30;
        const float RHipRoll_Max = 23.76;
        const float RKneePitch_Min = -5.90;
        const float RKneePitch_Max = 121.47;
        const float RAnklePitch_Min = -67.97;
        const float RAnklePitch_Max = 53.40;
        const float RAnkleRoll_Min = -45.03;
        const float RAnkleRoll_Max = 22.27;
        const float RShoulderPitch_Min = -119.5;
        const float RShoulderPitch_Max = 119.5;
        const float RShoulderRoll_Min = -76;
        const float RShoulderRoll_Max = 18;
        const float RElbowYaw_Min = -119.5;
        const float RElbowYaw_Max = 119.5;
        const float RElbowRoll_Min = 2.0;
        const float RElbowRoll_Max = 88.5;
        const float RWristYaw_Min = -104.5;
        const float RWristYaw_Max = 104.5;
        const float RHand_Min = 0.0; 
        const float RHand_Max = 57.2958;

        const float MaxAngle[] = {
            HeadYaw_Max,
            HeadPitch_Max,
            LShoulderPitch_Max,
            LShoulderRoll_Max,
            LElbowYaw_Max,
            LElbowRoll_Max,
            LWristYaw_Max,
            LHand_Max,
            LHipYawPitch_Max,
            LHipRoll_Max,
            LHipPitch_Max,
            LKneePitch_Max,
            LAnklePitch_Max,
            LAnkleRoll_Max,
            RHipRoll_Max,
            RHipPitch_Max,
            RKneePitch_Max,
            RAnklePitch_Max,
            RAnkleRoll_Max,
            RShoulderPitch_Max,
            RShoulderRoll_Max,
            RElbowYaw_Max,
            RElbowRoll_Max,
            RWristYaw_Max,
            RHand_Max
      };

      const float MinAngle[] = {
            HeadYaw_Min,
            HeadPitch_Min,
            LShoulderPitch_Min,
            LShoulderRoll_Min,
            LElbowYaw_Min,
            LElbowRoll_Min,
            LWristYaw_Min,
            LHand_Min,
            LHipYawPitch_Min,
            LHipRoll_Min,
            LHipPitch_Min,
            LKneePitch_Min,
            LAnklePitch_Min,
            LAnkleRoll_Min,
            RHipRoll_Min,
            RHipPitch_Min,
            RKneePitch_Min,
            RAnklePitch_Min,
            RAnkleRoll_Min,
            RShoulderPitch_Min,
            RShoulderRoll_Min,
            RElbowYaw_Min,
            RElbowRoll_Min,
            RWristYaw_Min,
            RHand_Min
    };

    //Maximum Motor Speed. Calculations taken from the 2016 Runswift release
    //Values are in degrees/second
    namespace MotorLimits {
        const float MOTOR1_REDUCTIONA_RAD = 2.23;
        const float MOTOR1_REDUCTIONB_RAD = 3.43;
        const float MOTOR2_REDUCTIONA_RAD = 8.96;
        const float MOTOR2_REDUCTIONB_RAD = 12.52;
        const float MOTOR3_REDUCTIONA_RAD = 3.85;
        const float MOTOR3_REDUCTIONB_RAD = 3.34;

        const float HeadPitchSpeed       = MOTOR3_REDUCTIONB_RAD;
        const float HeadYawSpeed         = MOTOR3_REDUCTIONA_RAD;
        const float ShoulderPitchSpeed   = MOTOR3_REDUCTIONA_RAD;
        const float ShoulderRollSpeed    = MOTOR3_REDUCTIONB_RAD;
        const float ElbowYawSpeed        = MOTOR3_REDUCTIONA_RAD;
        const float ElbowRollSpeed       = MOTOR3_REDUCTIONB_RAD;
        const float WristYawSpeed        = MOTOR2_REDUCTIONA_RAD;
        const float HandSpeed            = MOTOR2_REDUCTIONB_RAD;
        const float HipYawPitchSpeed     = MOTOR1_REDUCTIONA_RAD;
        const float HipRollSpeed         = MOTOR1_REDUCTIONA_RAD;
        const float HipPitchSpeed        = MOTOR1_REDUCTIONB_RAD;
        const float KneePitchSpeed       = MOTOR1_REDUCTIONB_RAD;
        const float AnklePitchSpeed      = MOTOR1_REDUCTIONB_RAD;
        const float AnkleRollSpeed       = MOTOR1_REDUCTIONA_RAD;
    
        const float MaxSpeed[] = {
            HeadYawSpeed,
            HeadPitchSpeed,
            ShoulderPitchSpeed,  // Left arm
            ShoulderRollSpeed,
            ElbowYawSpeed,
            ElbowRollSpeed,
            WristYawSpeed,
            HandSpeed,
            HipYawPitchSpeed,    // Left leg
            HipRollSpeed,
            HipPitchSpeed,
            KneePitchSpeed,
            AnklePitchSpeed,
            AnkleRollSpeed,
            HipYawPitchSpeed,    // Right leg
            HipRollSpeed,
            HipPitchSpeed,
            KneePitchSpeed,
            AnklePitchSpeed,
            AnkleRollSpeed,
            ShoulderPitchSpeed,  // Right arm
            ShoulderRollSpeed,
            ElbowYawSpeed,
            ElbowRollSpeed,
            WristYawSpeed,
            HandSpeed
        };
    };
}; 

/** Limb names, masses, and lengths from the NAO Documentation.
* Borrowed from the Runswift 2016 code release._broker
**/ 
namespace Limbs {

    const float NeckOffsetZ = 126.50;
    const float ShoulderOffsetY = 98.00;
    const float ElbowOffsetY = 15.00;
    const float UpperArmLength = 105.00;
    const float LowerArmLength = 55.95;
    const float ShoulderOffsetZ = 100.00;
    const float HandOffsetX = 57.75;
    const float HandOffsetZ = 12.31;
    const float HipOffsetZ = 85.00;
    const float HipOffsetY = 50.00;
    const float ThighLength = 100.00;
    const float TibiaLength = 102.90;
    const float FootHeight = 45.19;

    const float Length[] = {
        NeckOffsetZ,
        ShoulderOffsetY,
        ElbowOffsetY,
        UpperArmLength,
        LowerArmLength,
        ShoulderOffsetZ,
        HandOffsetX,
        HandOffsetZ,
        HipOffsetZ,
        HipOffsetY,
        ThighLength,
        TibiaLength,
        FootHeight
    };

    const float TorsoMass = 1.04956;
    const float NeckMass = 0.06442; // Head Yaw
    const float HeadMass = 0.60533; // Head Pitch
    const float RightShoulderMass = 0.07504; // R Shoulder Pitch
    const float RightBicepMass = 0.15794; // R Shoulder Roll
    const float RightElbowMass = 0.06483; // R Elbow Yaw
    const float RightForearmMass = 0.07778; // R Elbow Roll
    const float RightHandMass = 0.18533; // R Wrist Yaw
    const float RightPelvisMass = 0.07118; // R Hip Yaw Pitch
    const float RightHipMass = 0.13053; // R Hip Roll
    const float RightThighMass = 0.38976; // R Hip Pitch
    const float RightTibiaMass = 0.29163; // R Knee Pitch
    const float RightAnkleMass = 0.13415; // R Ankle Pitch
    const float RightFootMass = 0.16171; // R Ankle Roll
    const float LeftShoulderMass = 0.07504; // L Shoulder Pitch
    const float LeftBicepMass = 0.15777; // L Shoulder Roll
    const float LeftElbowMass = 0.06483; // L Elbow Yaw
    const float LeftForearmMass = 0.07761; // L Elbow Roll
    const float LeftHandMass = 0.18533; // L Wrist Yaw
    const float LeftPelvisMass = 0.06981; // L Hip Yaw Pitch
    const float LeftHipMass = 0.13053; // L Hip Roll
    const float LeftThighMass = 0.38968; // L Hip Pitch
    const float LeftTibiaMass = 0.29142; // L Knee Pitch
    const float LeftAnkleMass = 0.13416; // L Ankle Pitch
    const float LeftFootMass = 0.16184; // L Ankle Roll

    const float Mass[] = {
        TorsoMass,
        NeckMass,
        HeadMass,
        RightShoulderMass,
        RightBicepMass,
        RightElbowMass,
        RightForearmMass,
        RightHandMass,
        RightPelvisMass,
        RightHipMass,
        RightThighMass,
        RightTibiaMass,
        RightAnkleMass,
        RightFootMass,
        LeftShoulderMass,
        LeftBicepMass,
        LeftElbowMass,
        LeftForearmMass,
        LeftHandMass,
        LeftPelvisMass,
        LeftHipMass,
        LeftThighMass,
        LeftTibiaMass,
        LeftAnkleMass,
        LeftFootMass,
    };

    // Location of centre of masses in mm. Note: these are converted from m
    // so that they match the unit of measurement of the lengths of the link. 

    const float TorsoCoM[] = {-4.13, 0.09, 43.42, 1};
    const float NeckCoM[] = {-0.01, 0.14, -27.42, 1};
    const float HeadCoM[] = {-1.12, 0.03, 52.58, 1};
    const float RightShoulderCoM[] = {-1.65, 26.63, 0.14, 1};
    const float RightBicepCoM[] = {24.29, -9.52, 3.20, 1};
    const float RightElbowCoM[] = {-27.44, 0.00, -0.14, 1};
    const float RightForearmCoM[] = {25.52, -2.81, 0.90, 1};
    const float RightHandCoM[] = {34.34, -0.88, 3.08, 1};
    const float RightPelvisCoM[] = {-7.66, 12.00, 27.16, 1};
    const float RightHipCoM[] = {-15.49, -0.29, -5.16, 1};
    const float RightThighCoM[] = {1.39, -2.25, -53.74, 1};
    const float RightTibiaCoM[] = {3.94, -2.21, -49.38, 1};
    const float RightAnkleCoM[] = {0.45, -0.30, 6.84, 1};
    const float RightFootCoM[] = {25.40, -3.32, -32.39, 1};
    const float LeftShoulderCoM[] = {-1.65, -26.63, 0.14, 1};
    const float LeftBicepCoM[] = {24.55, 5.63, 3.30, 1};
    const float LeftElbowCoM[] = {-27.44, 0.00, -0.14, 1};
    const float LeftForearmCoM[] = {25.56, 2.81, 0.76, 1};
    const float LeftHandCoM[] = {34.34, -0.88, 3.08, 1};
    const float LeftPelvisCoM[] = {-7.81, -11.14, 26.61, 1};
    const float LeftHipCoM[] = {-15.49, 0.29, -5.15, 1};
    const float LeftThighCoM[] = {1.38, 2.21, -53.73, 1};
    const float LeftTibiaCoM[] = {4.53, 2.25, -49.36, 1};
    const float LeftAnkleCoM[] = {0.45, 0.29, 6.85, 1};
    const float LeftFootCoM[] = {25.42, 3.30, -32.39, 1};
};

//LED values

namespace LEDs {

    enum LEDCode {
        LeftEar1 = 0,
        LeftEar2,
        LeftEar3,
        LeftEar4,
        LeftEar5,
        LeftEar6,
        LeftEar7,
        LeftEar8,
        LeftEar9,
        LeftEar10,
        RightEar1,
        RightEar2,
        RightEar3,
        RightEar4,
        RightEar5,
        RightEar6,
        RightEar7,
        RightEar8,
        RightEar9,
        RightEar10,
        LeftEyeRed1,
        LeftEyeRed2,
        LeftEyeRed3,
        LeftEyeRed4,
        LeftEyeRed5,
        LeftEyeRed6,
        LeftEyeRed7,
        LeftEyeRed8,
        LeftEyeGreen1,
        LeftEyeGreen2,
        LeftEyeGreen3,
        LeftEyeGreen4,
        LeftEyeGreen5,
        LeftEyeGreen6,
        LeftEyeGreen7,
        LeftEyeGreen8,
        LeftEyeBlue1,
        LeftEyeBlue2,
        LeftEyeBlue3,
        LeftEyeBlue4,
        LeftEyeBlue5,
        LeftEyeBlue6,
        LeftEyeBlue7,
        LeftEyeBlue8,
        RightEyeRed1,
        RightEyeRed2,
        RightEyeRed3,
        RightEyeRed4,
        RightEyeRed5,
        RightEyeRed6,
        RightEyeRed7,
        RightEyeRed8,
        RightEyeGreen1,
        RightEyeGreen2,
        RightEyeGreen3,
        RightEyeGreen4,
        RightEyeGreen5,
        RightEyeGreen6,
        RightEyeGreen7,
        RightEyeGreen8,
        RightEyeBlue1,
        RightEyeBlue2,
        RightEyeBlue3,
        RightEyeBlue4,
        RightEyeBlue5,
        RightEyeBlue6,
        RightEyeBlue7,
        RightEyeBlue8,
        ChestRed,
        ChestGreen,
        ChestBlue,
        LeftFootRed,
        LeftFootGreen,
        LeftFootBlue,
        RightFootRed,
        RightFootGreen,
        RightFootBlue,
        NUMBER_OF_LEDS
    };

    const std::string ledNames[NUMBER_OF_LEDS] = {
        "Ears/Led/Left/0Deg/Actuator/Value",
        "Ears/Led/Left/36Deg/Actuator/Value",
        "Ears/Led/Left/72Deg/Actuator/Value",
        "Ears/Led/Left/108Deg/Actuator/Value",
        "Ears/Led/Left/144Deg/Actuator/Value",
        "Ears/Led/Left/180Deg/Actuator/Value",
        "Ears/Led/Left/216Deg/Actuator/Value",
        "Ears/Led/Left/252Deg/Actuator/Value",
        "Ears/Led/Left/288Deg/Actuator/Value",
        "Ears/Led/Left/324Deg/Actuator/Value",
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
        "ChestBoard/Led/Red/Actuator/Value",
        "ChestBoard/Led/Green/Actuator/Value",
        "ChestBoard/Led/Blue/Actuator/Value",
        "LFoot/Led/Red/Actuator/Value",
        "LFoot/Led/Green/Actuator/Value",
        "LFoot/Led/Blue/Actuator/Value",
        "RFoot/Led/Red/Actuator/Value",
        "RFoot/Led/Green/Actuator/Value",
        "RFoot/Led/Blue/Actuator/Value",
    };
};

//Sensor data

namespace Sensors {
    //Foot Force Sensitive Resistors (FSRs) position values in mm in the Ankle frame: http://doc.aldebaran.com/2-1/family/robots/fsr_robot.html
    //Format is: FSR_Position[left/right foot][Sensor number (1-4)][x or y axis]
    const float FSR_Position[2][4][2] = {
        {
            //Left Foot
            { 70.25, 29.9}, //Front Left Sensor
            { 70.25, -2.31 }, //Front Right Sensor
            { -30.25, 2.99 }, //Rear Left Sensor
            { -29.65, -1.91} //Rear Right Sensor
        },
        {
            //Right Foot
            { 70.25, 2.31 }, //Front Left Sensor
            { 70.25, -2.99 }, //Front Right Sensor
            { -30.25, 1.91 }, //Rear Left Sensor
            { -29.65, -2.99 } //Rear Right Sensor
        }
    };

    typedef enum SensorCodesEnum {
        InertialSensor_AccX = 0,
        InertialSensor_AccY,
        InertialSensor_AccZ,
        InertialSensor_GyrX,
        InertialSensor_GyrY,
        InertialSensor_GyrRef,
        InertialSensor_AngleX,
        InertialSensor_AngleY,

        InertialSensor_GyroscopeX,
        InertialSensor_GyroscopeY,
        InertialSensor_GyroscopeZ,
        InertialSensor_AccelerometerX,
        InertialSensor_AccelerometerY,
        InertialSensor_AccelerometerZ,

        LFoot_FSR_FrontLeft,
        LFoot_FSR_FrontRight,
        LFoot_FSR_RearLeft,
        LFoot_FSR_RearRight,
        LFoot_FSR_CenterOfPressure_X,
        LFoot_FSR_CenterOfPressure_Y,

        RFoot_FSR_FrontLeft,
        RFoot_FSR_FrontRight,
        RFoot_FSR_RearLeft,
        RFoot_FSR_RearRight,
        RFoot_FSR_CenterOfPressure_X,
        RFoot_FSR_CenterOfPressure_Y,

        LFoot_Bumper_Left,
        LFoot_Bumper_Right,
        RFoot_Bumper_Left,
        RFoot_Bumper_Right,

        ChestBoard_Button,

        Head_Touch_Front,
        Head_Touch_Rear,
        Head_Touch_Middle,

        Battery_Charge,
        Battery_Current,
        US,

        NUMBER_OF_SENSORS
    } SensorCode;

    const SensorCode sensorCodes[] = {
        InertialSensor_AccX,
        InertialSensor_AccY,
        InertialSensor_AccZ,
        InertialSensor_GyrX,
        InertialSensor_GyrY,
        InertialSensor_GyrRef,
        InertialSensor_AngleX,
        InertialSensor_AngleY,

        InertialSensor_GyroscopeX,
        InertialSensor_GyroscopeY,
        InertialSensor_GyroscopeZ,
        InertialSensor_AccelerometerX,
        InertialSensor_AccelerometerY,
        InertialSensor_AccelerometerZ,

        LFoot_FSR_FrontLeft,
        LFoot_FSR_FrontRight,
        LFoot_FSR_RearLeft,
        LFoot_FSR_RearRight,
        LFoot_FSR_CenterOfPressure_X,
        LFoot_FSR_CenterOfPressure_Y,

        RFoot_FSR_FrontLeft,
        RFoot_FSR_FrontRight,
        RFoot_FSR_RearLeft,
        RFoot_FSR_RearRight,
        RFoot_FSR_CenterOfPressure_X,
        RFoot_FSR_CenterOfPressure_Y,

        LFoot_Bumper_Left,
        LFoot_Bumper_Right,
        RFoot_Bumper_Left,
        RFoot_Bumper_Right,

        ChestBoard_Button,

        Head_Touch_Front,
        Head_Touch_Rear,
        Head_Touch_Middle,

        Battery_Charge,
        Battery_Current,
        US
    };

    const std::string sensorNames[] = {
        "InertialSensor/AccX",
        "InertialSensor/AccY",
        "InertialSensor/AccZ",
        "InertialSensor/GyrX",
        "InertialSensor/GyrY",
        "InertialSensor/GyrRef",
        "InertialSensor/AngleX",
        "InertialSensor/AngleY",

        "InertialSensor/GyroscopeX",
        "InertialSensor/GyroscopeY",
        "InertialSensor/GyroscopeZ",
        "InertialSensor/AccelerometerX",
        "InertialSensor/AccelerometerY",
        "InertialSensor/AccelerometerZ",

        "LFoot/FSR/FrontLeft",
        "LFoot/FSR/FrontRight",
        "LFoot/FSR/RearLeft",
        "LFoot/FSR/RearRight",
        "LFoot/FSR/CenterOfPressure/X",
        "LFoot/FSR/CenterOfPressure/Y",

        "RFoot/FSR/FrontLeft",
        "RFoot/FSR/FrontRight",
        "RFoot/FSR/RearLeft",
        "RFoot/FSR/RearRight",
        "RFoot/FSR/CenterOfPressure/X",
        "RFoot/FSR/CenterOfPressure/Y",

        "LFoot/Bumper/Left",
        "LFoot/Bumper/Right",
        "RFoot/Bumper/Left",
        "RFoot/Bumper/Right",

        "ChestBoard/Button",

        "Head/Touch/Front",
        "Head/Touch/Rear",
        "Head/Touch/Middle",

        "Battery/Charge",
        "Battery/Current",
        "US"
    };
};

//Ultrasound Stuff left out for now


#endif