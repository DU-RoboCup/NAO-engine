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

void watchdog(int sig) {
	std::cout << "The DOG DIED." << std::endl;
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

	// Simple watchdog timer using sigalrm which dies after 5 seconds
	signal(SIGALRM, watchdog);
	alarm(5);
	int i = 0;
	while (true) {
		sleep(i);
		std::cout << "Petting the dog...." << std::endl;
		alarm(5);
		i++;
	}
}
