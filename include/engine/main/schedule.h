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

This file deals with scheduling of modules on the main core

*/


#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "common.h"
#include <functional>

typedef uint32_t SeriesID;
typedef uint32_t TaskID;

typedef std::function<bool()> FrameAction;



struct Scheduler_Task {
	Scheduler_Task() : time(0), action(0), noop(true) {}
	Scheduler_Task(clock_t t,FrameAction a, bool n) : time(t), action(a), noop(n) {}
	clock_t time;
	FrameAction action;
	bool noop;
};

/*
class Schedule {
	public:
		virtual uint32_t ScheduleAction(FrameAction task) = 0;
		virtual FrameAction GetNextAction() = 0;
		virtual bool Unschedule(FrameAction act);
		
	private:
};*/

class RoundRobin /*: public Schedule*/ {
	public:
		RoundRobin() {current_action = 0;}
		~RoundRobin() {}
		uint32_t ScheduleAction(FrameAction task);
		FrameAction GetNextAction();
		bool Unschedule(FrameAction act);

		static bool NO_OP() { return true;}

	private:
		int current_action;
		std::vector<FrameAction> actions;
};



#endif /*_SCHEDULE_H_*/
