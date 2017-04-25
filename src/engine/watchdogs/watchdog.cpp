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

Context* myContext;

void watchdog(int sig) {
	BOOST_LOG_FUNCTION();
    LOG_FATAL << "The main context is not responding...";
	myContext->Kill();
}

void terminate_context(int dummy) {
    BOOST_LOG_FUNCTION();
    LOG_WARNING << "SIGINT Caught. Terminating Frame.";
    myContext->Kill();
    LOG_DEBUG << "Context Terminated. Ending Program.";
    exit(dummy);
}

int main(int argc, char** argv) {

	// Version String information
	LOG_INFO << "Now running Pineapple, the University of Denver robocup NAO-Engine";
	#ifdef PINEAPPLE_VERSION_0_0_1
		LOG_INFO << "Current Pineapple version: 0.0.2";
	#endif
	#ifdef NAO_SDK_VERSION_2_1_4_13
		LOG_INFO << "Compiled NAO-SDK version: 2.1.4.13";
	#endif

	// Setup the global clock
	std::shared_ptr<boost::any> global_clock(new boost::any(clock_t()));
	*global_clock = clock();
	Bazaar::Vend("Global/Clock", global_clock);

	// Setup the base priority
	struct sched_param param;
	param.sched_priority = sched_get_priority_max(SCHED_FIFO) - 3;
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        LOG_ERROR << "Setting the priority of the base thread failed. Are you running as root?";
        LOG_DEBUG << "Finished running";
 		return 1;
    }


    BOOST_LOG_FUNCTION();
    LOG_DEBUG << "Starting watchdog...";
    signal(SIGALRM, watchdog);
	alarm(30);

    // Capture CTRL-C
    signal(SIGINT, terminate_context);
	
	LOG_DEBUG << "Running the frame...";
	try {
        myContext = new Context();
	    myContext->Run();
	    delete myContext;
    } catch (int e) {
        // Disable the watchdog
        LOG_ERROR << "An error occurred while running the main context: " << e;
        LOG_ERROR << "Exiting...";
    }
    //cleanup_logger();
    return 0;
}
