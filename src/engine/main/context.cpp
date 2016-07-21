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

#include "engine/main/context.h"

void Context::Run() {
	this->Initialize();
	this->Execute();
	this->Cleanup();
}

void Context::Initialize() {
	LOG_DEBUG << "Initializing the main context...";

	// We're no longer dead, so we run it
	this->is_dead = false;

	// Load the main conficuration
	LuaTable mconfig = LuaTable::fromFile("../config/main.config");

	// Figure out which modules that we need to load initially
	int num_modules = (int) mconfig["len_init_modules"].get<double>();
	std::vector<std::string> init_modules;
	for (int i = 1; i <= num_modules; i++) {
		init_modules.push_back(mconfig["init_modules"][i].get<std::string>());
	}

	// Load the Modules
	for (auto i = init_modules.begin(); i != init_modules.end(); i++) {

		// Load and configure the module
		std::string module = *i;
		LOG_DEBUG << "Loading initial module " << module;
		uint16_t module_id = ModuleLoader::Instance()->LoadModule("../config/modules/" + module);
		if (UNLIKELY(module_id == 0)) {
			// The module wasn't loaded properly
			LOG_WARNING << "Failed to load module " << module;
			continue;
		}
		
		// Create and push back a module record
		loaded_modules.push_back(ModuleRecord(0, 
											  ModuleLoader::Instance()->GetModule(module_id), 
											  module_id,  
											  static_cast<float>(ModuleLoader::Instance()->GetModule(module_id)->GetFPS())
											  )
								);


	}

	// Setup the checkin variables
	for (ModuleRecord& m : loaded_modules) {
		m_check[m.module_id] = false;
	}

}

void Context::Execute() {
	LOG_DEBUG << "Creating new threads in the frame...";
	for (ModuleRecord& m : loaded_modules) {
		// Create a new thread running this module
		m.thread_handle = new std::thread(MainThreadLoop, this, std::ref(m));
		LOG_DEBUG << "Started module: " << m.module_handle->GetName();
	}


	std::shared_ptr<boost::any> gc = Bazaar::Get("Global/Clock");
	LOG_DEBUG << "Created main reference clock: " << boost::any_cast<clock_t>(*gc);

	while (!this->is_dead) {
		// Check that all threads have both checked in and if they have, then schedule the alarm
		bool check = true;
		for (auto x = m_check.begin(); x != m_check.end(); x++) {
			if ((*x).second == false) {
				check = false;
				break;
			}
		}

		if (check) {
			alarm(10);
			for (auto x = m_check.begin(); x != m_check.end(); x++) {
				(*x).second = false;
			}
		}

		// Update the global clock time
		(*gc) = clock();
		Bazaar::UpdateListing("Global/Clock", gc);
	}
	// Set the alarm for some more time to deal with the cleanup
	alarm(30);
}

void Context::MainThreadLoop(Context* myFrame, ModuleRecord& m) {
	
	// Figure out the scheduling window (that is, what length chunks should we use)
	auto sched_window = std::chrono::nanoseconds(static_cast<uint64_t>((1.0/m.requested_fps)*1000000000));
	LOG_DEBUG << "Scheduling window is " << sched_window.count() << "ns for module: " << m.module_handle->GetName();

	// Declare as real time
	struct sched_param param;
	param.sched_priority = 49 - m.module_handle->GetPriority();
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        LOG_ERROR << "Setting the priority of module " << m.module_handle->GetName() << " failed. Are you running as root?";
        myFrame->Kill();
 		return;
    }

    /* There's no readon here to prefault the stack, or to lock the memory, as 
    	we're not a true realtime task. However, this could be added in later
    	versions in order to retain ns timing */

	// Start the internal thread clock
	while (!myFrame->dead()) {
		auto start_chrono = std::chrono::high_resolution_clock::now();
		// Check in with the main frame
		myFrame->checkin(m.module_id);
		// Run the module
		m.module_handle->RunFrame();
		// Figure out sleep time
		auto end_chrono = std::chrono::high_resolution_clock::now();
		if (LIKELY((end_chrono-start_chrono) < sched_window))
			std::this_thread::sleep_for(sched_window - (end_chrono-start_chrono));

		// Do FPS calculations
		auto ft_chrono = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> ft = ft_chrono - start_chrono;
		m.avg_fps = ((m.avg_fps*m.num_runs) + 1.0/ft.count())/(m.num_runs+1);
		m.num_runs += 1;
		//if (m.num_runs % 100 == 0) 
		//	LOG_DEBUG << "FPS of module " << m.module_handle->GetName() << " is " << m.avg_fps << ". Requested " << m.requested_fps;
	}
}

void Context::Cleanup() {
	LOG_DEBUG << "Frame Terminated... Cleaning up.";
	LOG_DEBUG << "Attempting to gracefully join module threads...";
	for (ModuleRecord& m : loaded_modules) {
		m.thread_handle->join();
		LOG_DEBUG << "Joined module: " << m.module_handle->GetName();
	}
	LOG_DEBUG << "Finished joining module threads...";	
	LOG_DEBUG << "Unloading modules...";
	for (ModuleRecord& m : loaded_modules) {
		ModuleLoader::Instance()->UnloadModule(m.module_id);
	}
	LOG_DEBUG << "Finished unloading modules...";
	LOG_DEBUG << "Finished cleanup...";
}


/** It's possible to add some additional protection here */
void Context::Kill() {
	LOG_DEBUG << "Terminating thread....";
	this->is_dead = true;
}