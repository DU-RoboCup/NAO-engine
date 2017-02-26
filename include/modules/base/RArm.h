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
Derived Right Arm class
*/

//INCLUDES
#pragma once
#include "Arm.h"
#include <vector>
class RArm :
	public Arm
{
public:
	RArm();
	~RArm();
	void set_shoulder(float roll, float pitch);
	void set_elbow(float roll, float yaw);
	void set_wrist(float yaw);
	std::string test_message();
	//Bounds...there's got to be a lazier way to do this...

private:
	std::string hellomsg;
};

