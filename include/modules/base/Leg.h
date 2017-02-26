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
	typedef std::vector<std::pair<float, float>> BoundList; //Typing this is annoying and screenspace is limited

	virtual void set_hip(float roll, float yaw, float pitch) = 0;
	virtual void set_knee(float pitch) = 0;
	virtual void set_ankle(float roll, float pitch) = 0;
	

	///Helper functions for setting Actuators
	void set_hip_roll(float roll);
	void set_hip_yaw(float yaw);
	void set_hip_pitch(float pitch);
	void set_knee_pitch(float pitch);
	void set_ankle_roll(float roll);
	void set_ankle_pitch(float pitch);
	void set_hip_stiffness(float roll_stiffness, float yaw_stiffness, float pitch_stiffness);
	void set_knee_stiffness(float pitch_stiffness);
	void set_ankle_stiffness(float roll_stiffness, float pitch_stiffness);
	//Helper Functions for setting stiffness

	void set_hip_roll_stiffness(float roll);
	void set_hip_yaw_stiffness(float yaw);
	//void set_hip_pitch_stiffness(float pitch);
	void set_knee_pitch_stiffness(float pitch);
	void set_ankle_roll_stiffness(float roll);
	void set_ankle_pitch_stiffness(float pitch);

	float get_hip_roll();
	float get_hip_yaw();
	float get_hip_pitch();
	float get_knee_pitch();
	float get_ankle_roll();
	float get_ankle_pitch();
	// BoundList get_actuator_bounds();

	bool checkBounds(std::pair<float, float> bounds, float val);

private:
	float HipRoll_A, HipYaw_A, HipPitch_A, KneePitch_A, AnkleRoll_A, AnklePitch_A;///<Actuator Positions Values
	float HipRoll_Stiffness, HipYaw_Stiffness, HipPitch_Stiffness, KneePitch_Stiffness, AnkleRoll_Stiffness, AnklePitch_Stiffness = 0.0; ///< Actuator Stiffness Values
	std::pair<float, float> HipRollBounds, HipPitchBounds, HipYawBounds, KneePitchBounds, AnklePitchBounds, AnkleRollBounds = {0,0};
};
