
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

The main context loop for the robot, deals with initialization, scheduling, and termination

*/


#ifndef _CONTEXT_h_GUARD_
#define _CONTEXT_h_GUARD_

#include "common.h"
#include "engine/main/mloader.h"

// Real Time
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <signal.h>


#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <atomic>
#include <map>
#include <memory>


#define NUM_THREADS 2

struct ModuleRecord {
	
	ModuleRecord(Module* m, uint16_t id) : thread_handle(0), module_handle(m), module_id(id) {}
	ModuleRecord(std::thread* t, Module* m, uint16_t id) : thread_handle(t), module_handle(m), module_id(id) {}
	ModuleRecord(std::thread* t, Module* m, uint16_t id, float rfps) : thread_handle(t), module_handle(m), module_id(id), requested_fps(rfps) {}
	
	std::thread* thread_handle;
	Module* module_handle;
	uint16_t module_id;

	float avg_frame_runtime = 0;
	float avg_fps = 0;
	uint64_t num_runs = 1;
	float requested_fps = 0;
};

class Context {
	public:
		void Run();
		void Initialize();
		void Execute();
		void Cleanup();
		void Kill();

		bool dead() {return is_dead;}
		void checkin(uint8_t thread_id) {m_check[thread_id] = true;}

		static void MainThreadLoop(Context* myFrame, ModuleRecord& m);
	private:
		std::atomic<bool> is_dead;
		std::vector<ModuleRecord> loaded_modules;
		std::map<uint8_t, std::atomic<bool>> m_check;
};

#endif /*_FRAME_h_GUARD_*/