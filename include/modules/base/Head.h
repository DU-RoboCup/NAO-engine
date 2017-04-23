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
Class for the head
*/
#pragma once

//INCLUDES
#include <utility>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class Head
{
public:

	Head();
	~Head();
	struct EyeLED { float RED = 0.0; float GREEN = 1.0; float BLUE = 0.0; }; //% of each LED, 0 being off, 1 being full brightness
	
	std::string get_current_thought();
	std::unordered_map<float, std::pair<float, float>> pitch_limits_map;
	void set_head_actuators(float yaw, float pitch);
	void set_yaw(float yaw);
	void set_pitch(float pitch);
	bool checkBounds(std::pair<float, float> bounds, float val);
	std::pair<float, float> get_actuators();
	float get_head_yaw();
	float get_head_pitch();
	std::unordered_map<std::string, EyeLED> get_Eye_LEDS();
	EyeLED get_Eye_LEDS(std::string &eye_and_LED_number);

	//TODO: Write set LED functions
private:
	std::unordered_map<std::string, EyeLED> eye_led_map;
	//NOTE: Pitch value is limited to the Yaw value to prevent collision with plastic. http://doc.aldebaran.com/2-1/family/robots/joints_robot.html#robot-joints-v4-head-joints
	float head_yaw, head_pitch;
	std::pair<float, float> YawRange;
	std::pair<float, float> PitchRange;
};
