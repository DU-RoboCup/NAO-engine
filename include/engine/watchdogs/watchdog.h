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
-- Created by David Chan 6/14/16

// FILE DESCRIPTION

This is the header file for the main watchdog. All this does is launch the 
threads responsible for dealing with most of the problems on the robot,
and makes sure that they check in from time to time.

*/

#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

// INCLUDES
#include "engine/main/context.h"

#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <exception>
#include <ctime>

/*! The main entry point for the entire NAO-Engine. This main method starts
 * the threads in the proper order, as well as generates the necessary watchdogs
 * to keep the thread alive
 * @param  argc Number of command line args
 * @param  argv Command line args
 * @return      Error Code
 */
int main(int argc, char** argv);

/*! The function that pets the dog
 * @param sig The signal sent
 */
void watchdog(int sig);

#endif /*_WATCHDOG_H_*/
