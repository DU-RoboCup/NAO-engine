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
    }

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





}



#endif