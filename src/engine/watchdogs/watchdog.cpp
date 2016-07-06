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
-- Created by David Chan 6/8/16
-- Modified by Mark Whalburg 7/14/18
-- Modified by Hugh Jass 7/14/18

// FILE DESCRIPTION
Put a description of your cpp file here. 
*/


// Includes
#include "engine/watchdogs/watchdog.h"
#include "debug/debugging.h"
#include "engine/main/mloader.h"

uint16_t id = 0;

void watchdog(int sig) {
	BOOST_LOG_FUNCTION();
    LOG_FATAL << "The dog is dead";
    std::cout << "The DOG DIED." << std::endl;
	ModuleLoader::Instance()->UnloadModule(id);
	alarm(5);
}

int main(int argc, char**argv) {

	// Version String information
	std::cout << "Now running Pineapple, the University of Denver robocup NAO-Engine" << std::endl;	
	#ifdef PINEAPPLE_VERSION_0_0_1
		std::cout << "Current Pineapple version: 0.0.1" << std::endl;
	#endif
	#ifdef NAO_SDK_VERSION_2_1_4_13
		std::cout << "Compiled NAO-SDK version: 2.1.4.13" << std::endl;
	#endif

    BOOST_LOG_FUNCTION();
    LOG_DEBUG << "Starting watchdog...";
	// Load a module
	try {
        id = ModuleLoader::Instance()->LoadModule("testmoduleone.module");
    } catch (std::exception &ex) {
        LOG_FATAL << ex.what() << ", the dog is dead on arrival.";
    }
    std::cout << "NAME: " << ModuleLoader::Instance()->GetModule(id)->GetName() << std::endl;
	std::cout << "FPS:  " << ModuleLoader::Instance()->GetModule(id)->GetFPS() << std::endl;
	std::cout << "PRIO: " << unsigned(ModuleLoader::Instance()->GetModule(id)->GetPriority()) << std::endl;
	std::cout << "ID:   " << ModuleLoader::Instance()->GetModule(id)->GetID() << std::endl;


	// Simple watchdog timer using sigalrm which dies after 5 seconds
	signal(SIGALRM, watchdog);
	alarm(5);
	int i = 1;
	while (true) {
		sleep(i);
		LOG_DEBUG << "Petting the dog";
		if (ModuleLoader::Instance()->GetModule(id))
			ModuleLoader::Instance()->GetModule(id)->RunFrame();
		std::cout << "Petting the dog...." << std::endl;
		i++;
		alarm(5);
	}
  //  cleanup_logger();
    return 0;
}
