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
	virtual void set_shoulder(double roll, double pitch) = 0;
	virtual void set_elbow(double roll, double yaw) = 0;
	virtual void set_wrist(double yaw) = 0;
	//Thank you abstraction!
	void set_shoulder_roll(double roll);
	void set_shoulder_pitch(double pitch);
	void set_elbow_roll(double roll);
	void set_elbow_yaw(double yaw);
	void set_wrist_yaw(double yaw);

	//<R,L>ARM::ACTUATORS::<shoulder, elbow, wrist> getters
	double get_shoulder_roll();
	double get_shoulder_pitch();
	double get_elbow_roll();
	double get_elbow_yaw();
	double get_wrist_yaw();
	void current_actuator_vals();

	//void set_stiffness_vals(double shoulder_roll, double shoulder_pitch, double elbow_roll, double elbow_yaw, double wrist_yaw, double hand);
	std::vector<double> get_sensors();
	bool checkBounds(std::pair<double, double> actuator, double value);

	//There's probably a much better way to do this
	std::pair<double, double> wrist_bounds;
	std::pair<double, double> shoulder_roll_bounds;
	std::pair<double, double> shoulder_pitch_bounds;
	std::pair<double, double> elbow_roll_bounds;
	std::pair<double, double> elbow_yaw_bounds;
	std::pair<double, double> hand_position_bounds = std::make_pair<double, double>(0, 1);

	//Debug functions
	std::string print_arm_status();
protected:
	//Actuators
	double ShoulderRoll, ShoulderPitch;
	double ElbowRoll, ElbowYaw;
	double WristYaw;
	//Sensors
	double ShoulderRollSensor, ShoulderPitchSensor;
	double ElbowRollSensor, ElbowPitchSensor;
	struct stiffness_vals {
		double shoulder_roll, shoulder_pitch, elbow_roll, elbow_yaw, wrist_yaw, hand = 0.0;
	};



};
