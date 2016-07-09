/*
Copyright (c) 2016 "University of Denver"

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
-- Created by David Chan 7/8/16

// FILE DESCRIPTION

Implementation of the scheduler

*/

#include "engine/main/schedule.h"

// ------------------ SIMPLE ROUND ROBIN SCHEDULING --------------------- //
uint32_t RoundRobin::ScheduleAction(FrameAction act) {
	this->actions.push_back(act);
}

FrameAction RoundRobin::GetNextAction() {
	if (actions.size() == 0) {
		return RoundRobin::NO_OP;
	}
	else {
		current_action = (current_action + 1) % actions.size();
		return actions[current_action];
	}
}

bool RoundRobin::Unschedule(FrameAction act) {
	
	// Can't do this in this base sample of the round-robin scheduler
	/*
	for (int i = 0; i < actions.size(); i++ ){
		if (actions[i] == act) {
			actions.erase(actions.begin() + i);
			return true;
		}
	}
	return false;
	*/
	return false;
}

// ------------------- MORE COMPLICATED SCHEDULER ---------------------- //