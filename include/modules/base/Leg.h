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
Abstract/Interface class for Legs
*/

//INCLUDES
#pragma once
#include <utility>
#include <vector>
#include <iostream>
#include <memory>

class Leg
{
public:
	typedef std::vector<std::pair<double, double>> BoundList; //Typing this is annoying and screenspace is limited

	virtual void set_hip(double roll, double yaw, double pitch) = 0;
	virtual void set_knee(double pitch) = 0;
	virtual void set_ankle(double roll, double pitch) = 0;
	virtual BoundList get_actuator_bounds() = 0;

	///Helper functions for setting Actuators
	void set_hip_roll(double roll);
	void set_hip_yaw(double yaw);
	void set_hip_pitch(double pitch);
	void set_knee_pitch(double pitch);
	void set_ankle_roll(double roll);
	void set_ankle_pitch(double pitch);
	//virtual std::vector<double> get_hip_values() = 0;
	//virtual std::vector<double> get_knee_values() = 0;
	//virtual std::vector<double> get_ankle_values() = 0;
	void set_hip_stiffness(double roll_stiffness, double yaw_stiffness, double pitch_stiffness);
	void set_knee_stiffness(double pitch_stiffness);
	void set_ankle_stiffness(double roll_stiffness, double pitch_stiffness);
	//Helper Functions for setting stiffness

	void set_hip_roll_stiffness(double roll);
	void set_hip_yaw_stiffness(double yaw);
	void set_hip_pitch_stiffness(double pitch);
	void set_knee_pitch_stiffness(double pitch);
	void set_ankle_roll_stiffness(double roll);
	void set_ankle_pitch_stiffness(double pitch);

	double get_hip_roll();
	double get_hip_yaw();
	double get_hip_pitch();
	double get_knee_pitch();
	double get_ankle_roll();
	double get_ankle_pitch();
	
	bool checkBounds(std::pair<double, double> bounds, double val);

private:
	
	double HipRoll_A, HipYaw_A, HipPitch_A, KneePitch_A, AnkleRoll_A, AnklePitch_A;///<Actuator Positions Values
	double HipRoll_Stiffness, HipYaw_Stiffness, HipPitch_Stiffness, KneePitch_Stiffness, AnkleRoll_Stiffness, AnklePitch_Stiffness = 0.0; ///< Actuator Stiffness Values

};
class Hip
{
public:
	Hip();
	Hip(std::pair<double, double> &hrBounds, std::pair<double, double> &hyBounds, std::pair<double, double> &hpBounds);
	~Hip();
	void set_stiffness(double roll_stiffness, double yaw_stiffness, double pitch_stiffness);
	void update_actuators(double roll, double yaw, double pitch);
	std::vector<double> get_actuator_values();
	std::vector<double> get_sensor_values();
	std::pair<double, double> HipRollBounds, HipYawBounds, HipPitchBounds;
private:
	//Actuators
	
	double HipRoll_A, HipYaw_A, HipPitch_A;
	double HipRoll_Stiffness, HipYaw_Stiffness, HipPitch_Stiffness = 0;
	//Sensors - More available[current(A), Temperature status code, other status codes]
	double HipRoll_S, HipYaw_S, HipPitch_S, Temperature = 0;
};

class Knee
{
public:
	void set_stiffness(double pitch_stiffness);
	void update_actuators(double pitch);
	double get_actuator_values();
	std::vector<double> get_sensor_values();
	std::pair<double, double> KneePitchBounds;

private:
	//Actuators
	double KneePitch_A, KneePitch_Stiffness = 0;
	//Sensors
	double KneePitch_S, Temperature = 0;
};

class Ankle
{
public:
	void set_stiffness(double roll_stiffness, double pitch_stiffness);
	void update_actuators(double &roll_stiffness, double &pitch_stiffness);
	std::vector<double> get_actuator_values();
	std::vector<double> get_sensor_values();
	std::pair<double, double> AnkleRollBounds, AnklePitchBounds;
private:
	double AnkleRoll_A, AnklePitch_A;
	double AnkleRoll_Stiffness, AnklePitch_Stiffness;
	//Sensors
	double AnkleRoll_S, AnklePitch_S, Temperature = 0;
};