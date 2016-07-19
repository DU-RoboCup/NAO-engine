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


uint16_t id = 0;
bool timeout = false;

void watchdog(int sig) {
	BOOST_LOG_FUNCTION();
    LOG_FATAL << "The main frame is not responding...";
	LOG_FATAL << "We need to kill the frame...";
	LOG_FATAL << "WE DON'T KNOW HOW TO DO THAT YET!!!!!!!!!";
	LOG_FATAL << "AHHHHHHHHHHHHHHHHHHHH";
}

int main(int argc, char** argv) {

	// Version String information
	LOG_DEBUG << "Now running Pineapple, the University of Denver robocup NAO-Engine";
	#ifdef PINEAPPLE_VERSION_0_0_1
		LOG_DEBUG << "Current Pineapple version: 0.0.1";
	#endif
	#ifdef NAO_SDK_VERSION_2_1_4_13
		LOG_DEBUG << "Compiled NAO-SDK version: 2.1.4.13";
	#endif

	// Setup the global clock
	std::shared_ptr<boost::any> global_clock(new boost::any(clock_t()));
	*global_clock = clock();
	Bazaar::Vend("Global/Clock", global_clock);

    BOOST_LOG_FUNCTION();
    LOG_DEBUG << "Starting watchdog...";
    signal(SIGALRM, watchdog);
	alarm(30);
	
	LOG_DEBUG << "Running the frame...";
	Context f;
	f.Run();


	/*
	// Load a module
	try {
        id = ModuleLoader::Instance()->LoadModule("config/modules/testmoduleone.module");
    } catch (std::exception &ex) {
        LOG_FATAL << ex.what() << ", the dog is dead on arrival.";
    }
    LOG_DEBUG << "NAME: " << ModuleLoader::Instance()->GetModule(id)->GetName();
	LOG_DEBUG << "FPS:  " << ModuleLoader::Instance()->GetModule(id)->GetFPS();
	LOG_DEBUG << "PRIO: " << unsigned(ModuleLoader::Instance()->GetModule(id)->GetPriority());
	LOG_DEBUG << "ID:   " << ModuleLoader::Instance()->GetModule(id)->GetID();

	// Show off the bazaar
	std::string hw = "Hello World!";
	boost::any* a = new boost::any(hw);
	std::shared_ptr<boost::any> hello(a); 
	Bazaar::Vend("HelloWorldKey", hello);
	LOG_DEBUG <<  boost::any_cast<std::string>(*Bazaar::Get("HelloWorldKey"));


	// Simple watchdog timer using sigalrm which dies after 5 seconds
	
	int i = 1;
	while (true) {
		sleep(i);
		LOG_DEBUG << "Petting the dog";
		if (ModuleLoader::Instance()->GetModule(id))
			ModuleLoader::Instance()->GetModule(id)->RunFrame();
		i++;
		alarm(5);
	}
	*/
  //  cleanup_logger();
    return 0;
}
