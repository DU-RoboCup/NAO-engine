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
-- Created by Paul Heinen 2/2/17
// FILE DESCRIPTION
Abstract/Interface class for arms
*/
#include "include/modules/base/Arm.h"

void Arm::current_actuator_vals()
{
	std::cout << "Actuator Values: Shouder[" << ShoulderRoll << ", " << ShoulderPitch << "]; Elbow[" << ElbowRoll << ", " << ElbowYaw << "]; Wrist[" << WristYaw << "]" << std::endl;
}

void Arm::set_shoulder_roll(float roll)
{
	set_shoulder(roll, ShoulderPitch);
}

void Arm::set_shoulder_pitch(float  pitch)
{
	set_shoulder(ShoulderRoll, pitch);
}

void Arm::set_elbow_roll(float  roll)
{
	set_elbow(roll, ElbowRoll);
}

void Arm::set_elbow_yaw(float  yaw)
{
	set_elbow(ElbowRoll, yaw);
}

void Arm::set_wrist_yaw(float  yaw)
{
	set_wrist(yaw);
}

float Arm::get_shoulder_roll()
{
	return ShoulderRoll;
}

float Arm::get_shoulder_pitch()
{
	return ShoulderPitch;
}

float Arm::get_elbow_roll()
{
	return ElbowRoll;
}

float Arm::get_elbow_yaw()
{
	return ElbowYaw;
}

float Arm::get_wrist_yaw()
{
	return WristYaw;
}



std::vector<float> Arm::get_sensors()
{
	return std::vector<float>() = {ShoulderRollSensor, ShoulderPitchSensor, ElbowRollSensor, ElbowPitchSensor};
}

bool Arm::checkBounds(std::pair<float, float> actuator, float value)
{
	return true ? (value > actuator.first && value < actuator.second) : false;
}

std::string Arm::print_arm_status()
{
	return std::string("Arm_Status: Shoulder<" + std::to_string(ShoulderRoll) + ", " + std::to_string(ShoulderPitch) + ">, Elbow<"
	+ std::to_string(ElbowRoll) + ", " + std::to_string(ElbowYaw) + "> Wrist<" + std::to_string(WristYaw) + ">");
}
