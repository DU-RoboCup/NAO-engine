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

//INCLUDES
#pragma once
#include <vector>
#include <iostream>
#include <string>
class Arm
{
public:

	//<R,L>ARM::ACTUATORS::<shoulder, elbow, wrist> setters
	/**
	  * Interface methods. Need to be implemented in each body part due to actuator limit checking.
	  */
	virtual void set_shoulder(float roll, float pitch) = 0;
	virtual void set_elbow(float roll, float yaw) = 0;
	virtual void set_wrist(float yaw) = 0;
	//Thank you abstraction!
	void set_shoulder_roll(float roll);
	void set_shoulder_pitch(float pitch);
	void set_elbow_roll(float roll);
	void set_elbow_yaw(float yaw);
	void set_wrist_yaw(float yaw);

	//<R,L>ARM::ACTUATORS::<shoulder, elbow, wrist> getters
	float get_shoulder_roll();
	float get_shoulder_pitch();
	float get_elbow_roll();
	float get_elbow_yaw();
	float get_wrist_yaw();
	void current_actuator_vals();

	//void set_stiffness_vals(float shoulder_roll, float shoulder_pitch, float elbow_roll, float elbow_yaw, float wrist_yaw, float hand);
	std::vector<float> get_sensors();
	bool checkBounds(std::pair<float, float> actuator, float value);

	//There's probably a much better way to do this
	std::pair<float, float> wrist_bounds;
	std::pair<float, float> shoulder_roll_bounds;
	std::pair<float, float> shoulder_pitch_bounds;
	std::pair<float, float> elbow_roll_bounds;
	std::pair<float, float> elbow_yaw_bounds;
	std::pair<float, float> hand_position_bounds = std::make_pair<float, float>(0, 1);

	//Debug functions
	std::string print_arm_status();
protected:
	//Actuators
	float ShoulderRoll, ShoulderPitch;
	float ElbowRoll, ElbowYaw;
	float WristYaw;
	//Sensors
	float ShoulderRollSensor, ShoulderPitchSensor;
	float ElbowRollSensor, ElbowPitchSensor;
	struct stiffness_vals {
		float shoulder_roll, shoulder_pitch, elbow_roll, elbow_yaw, wrist_yaw, hand = 0.0;
	};



};
