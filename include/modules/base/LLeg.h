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
Derived Left Leg class
*/

//INCLUDES
#pragma once
#include "Leg.h"
class LLeg :
	public Leg
{
public:
	LLeg();
	~LLeg();
	void set_hip(float roll, float yaw, float pitch);
	void set_knee(float pitch);
	void set_ankle(float roll, float pitch);
private:
	std::pair<float, float> HipRollBounds, HipPitchBounds, HipYawBounds, KneePitchBounds, AnklePitchBounds, AnkleRollBounds = {0,0};
};

