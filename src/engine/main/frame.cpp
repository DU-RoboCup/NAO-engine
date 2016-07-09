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

The main frame loop, responsible for initialization, running
and termination

*/

#include "engine/main/frame.h"

void Frame::Run() {
	this->Initialize();
	this->Execute();
	this->Cleanup();
}

void Frame::Initialize() {
	LOG_DEBUG << "Initializing the main frame...";

	// We're no longer dead, so we run it
	this->is_dead = false;

	// Load the main conficuration
	LuaTable mconfig = LuaTable::fromFile("config/main.config");

	// Figure out which modules that we need to load initially
	int num_modules = (int) mconfig["len_init_modules"].get<double>();
	std::vector<std::string> init_modules;
	for (int i = 1; i <= num_modules; i++) {
		init_modules.push_back(mconfig["init_modules"][i].get<std::string>());
	}

	// Load the Modules
	for (auto i = init_modules.begin(); i != init_modules.end(); i++) {
		std::string module = *i;
		LOG_DEBUG << "Loading initial module " << module;
		uint16_t module_id = ModuleLoader::Instance()->LoadModule("config/modules/" + module);
		if (UNLIKELY(module_id == 0)) {
			// The module wasn't loaded properly
			LOG_WARNING << "Failed to load module " << module;
			continue;
		}
		this->loaded_module_ids.push_back(module_id);
	}

	// Initialize the schedules
	for (int i = 1; i <= NUM_THREADS; i++) {
		schedules[i] = new RoundRobin();
	}

	// Schedule the modules
	for (int i = 0; i < loaded_module_ids.size(); i++) {
		// Get which schedule we should add the module to
		uint16_t which_thread = ModuleLoader::Instance()->GetModule(loaded_module_ids[i])->GetThread();
		if (which_thread > NUM_THREADS || which_thread < 1){
			LOG_WARNING << "Invalid thread " << which_thread << " when scheduling module " << ModuleLoader::Instance()->GetModule(loaded_module_ids[i])->GetName();
			LOG_WARNING << "Scheduling " << ModuleLoader::Instance()->GetModule(loaded_module_ids[i])->GetName() << " on thread " << NUM_THREADS;
			which_thread = NUM_THREADS;
		}

		// Add the module to the schedule
		uint16_t fps = ModuleLoader::Instance()->GetModule(loaded_module_ids[i])->GetFPS();
		std::function<bool()> f_act = std::bind(&Module::RunFrame, ModuleLoader::Instance()->GetModule(loaded_module_ids[i]));
		schedules[which_thread]->ScheduleAction(f_act);
	}


	// Setup the checkin variables
	for (int i = 1; i <= NUM_THREADS; i++) {
		m_check[i] = false;
	}

}

void Frame::Execute() {
	// Create the threads that are a part of the thread pool
	LOG_DEBUG << "Creating new threads in the frame...";

	for (int i = 1; i <= NUM_THREADS; i++) {
		threads[i] = new std::thread(Frame::MainThreadLoop, this, i);
	}

	std::shared_ptr<boost::any> gc = Bazaar::Get("Global/Clock");
	LOG_DEBUG << "UPDATED CLOCK TO: " << boost::any_cast<clock_t>(*gc);

	while (!this->is_dead) {
		// Check that all threads have both checked in and if they have, then schedule the alarm
		bool check = true;
		for (auto x = m_check.begin(); x != m_check.end(); x++) {
			if ((*x).second == false) {
				check = false;
				break;
			}
		}

		if (!check) {
			alarm(10);
			for (auto x = m_check.begin(); x != m_check.end(); x++) {
				(*x).second = false;
			}
		}

		// Update the global clock time
		(*gc) = clock();
		Bazaar::UpdateListing("Global/Clock", gc);
	}


	// Join the threads
	LOG_DEBUG << "Started joining threads...";
	for (auto i = threads.begin(); i != threads.end(); i++) {
		(*i).second->join();
	}
	LOG_DEBUG << "Finished joining threads...";	
}

void Frame::MainThreadLoop(Frame* myFrame, uint8_t thread_id) {
	while (!myFrame->dead()) {
		// Check in with the main frame
		myFrame->checkin(thread_id);
		// Run the next object from the scheduler
		myFrame->GetSchedule(thread_id)->GetNextAction()();
	}
}

RoundRobin* Frame::GetSchedule(uint8_t thread_id) {
	return schedules[thread_id];
}

void Frame::Cleanup() {
	LOG_DEBUG << "Frame Terminated... Cleaning up.";
}


/** It's possible to add some additional protection here */
void Frame::Kill() {
	LOG_DEBUG << "Terminating thread....";
	this->is_dead = true;
}