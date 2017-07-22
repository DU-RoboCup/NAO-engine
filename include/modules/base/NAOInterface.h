
/*
Copyright (c) 2017 "University of Denver"
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
-- Created by Paul Heinen 2/2/17
// FILE DESCRIPTION
Pineapple module for communication with the NAO hardware. This module does not directly set/read values
from the NAO hardware, instead it communicates via shared memory with a seperate module that uses
Aldebaran's API for hardware communication. The advantage of keeping the processes seperate is that it allows
us to optimize how we communicate with the hardware, and it allows us to create our own API. Another advantage
is that we can simulate modules communicating with the NAO hardware, so for development purposes people won't even
need to physically work with a NAO robot.
*/

#ifndef _NAO_INTERFACE_H_
#define _NAO_INTERFACE_H_

//INCLUDES

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <queue>
#include <semaphore.h>
// Add some dynamic programming abilities
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp> //Implemented in STL in C++17 :(
// Interprocess
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/algorithm/string.hpp>


#include "HALPriorityQueue.h"
#include "include/common.h"
#include "include/debug/debugging.h"
#include "include/memory/hal_data.h"
#include "include/util/hardwaremap.h"


/**
  * \class NAOInterface
  * Main class/module class for the NAOInterface. A number of other classes
  * are what are really doing most of the work. 
  **/
using namespace boost::interprocess;
class NAOInterface : public Module
{
public:
	

	
	//Debug Functions
	void print_commands_list(); ///< Lists all available function calls

	std::vector<std::string> command_list; ///< Stores the commands. 
	/**
	  * \brief Writes a new value to a hardware component.
	  * \param hardware_component - The hardware component's name to be written to.
	  * \param value - A float containing the value to be set.
	  * \return - boolean which is true if operation was succesful or false if a failure occured.
	  *
	  * set_hardware_value first checks if the given harware component string is valid
	  * and a real hardware component, then updates the joint_and_sensor_data map.
	  * After all values have been written to the local map of values, we send that data
	  * to the Shared Memory Map using the synch_pineapple() method.
	  */
	bool set_hardware_value(const int &hardware_component, float value, QPRIORITY_FLAG FLAG);
	bool set_hardware_value(const unsigned int &hardware_component, const float value);




	/**
	  * \brief - Helper method for initializing access to shared memory. 
	  *
	  * This is not in the constructor because if NaoQI crashes, or our processes
	  * crashes in some odd way, it could be useful to re-open the shared memory.  
	  **/
	bool access_shared_memory();
	void create_shared_memory();
	/** \brief - Synchronizes hardware data values with the module running in NAOqi
	  * \return - boolean which is true if operation was succesful or false if a failure occured.
	  *
	  * This method is the most important of NAOInterface, it synchronizes data by reading and writing 
	  * from/to Shared Memory. There are safety mechanisims built in (namely a semaphore) to prevent
	  * data corruption from occuring when both this module and the libpineapple module running in NAOqi
	  * attempt to both read/write to the shared memory. 
	  *
	  * This method is called after all of the write intents occur and before any read intent occurs to 
	  * ensure that both processes have the most up to date hardware values.
	  */
	bool sync_pineapple();
	/**
	  *\brief - uses a semaphore to safely read values from the NaoQI Module
	  **/
	bool read_shared_memory();
	/**
	  * \brief - uses a semaphore to safely write values to shared memory,
	  * and finally to the NaoQI module.
	  **/
	bool write_shared_memory(); 

	/**
	  * \brief - make the NAO say the desired text. 
	  * Text MUST be 35 characters or less
	  **/
	bool speak(const std::string &text);

	// Hardware Write Tests
	void hardware_write_test();
	
	//required module functions
	static NAOInterface* Instance();
	void Reconfigure(std::string config_file, uint16_t id);
	bool RunFrame();
	bool ProcessIntent(Intent &i);
	bool Install();
	bool Uninstall();

private:
	static NAOInterface* instance;
	bool shared_memory_setup;
	NAOInterface();
	//Intent processing
  PendingIntents pending_intents;
	//Interprocess communication variables
	boost::interprocess::managed_shared_memory shm;
	hal_data *pineappleJuice; // Object stored in interprocess memory
	std::pair<hal_data *, std::size_t> shared_data_ptr;
	sem_t *semaphore;
	//named_semaphore actuator_semaphore, sensor_semaphore;
	bool read_shm, write_shm; // Return Values for interprocess rw operations
	std::vector<float> sensor_vals, actuator_vals;
	/* Items to be written stored as tuples: <ActuatorIDnumber, Value, FLAG> */
	HAL_PQ WriteRequests;

	//Contains a hashmap with all actuator and sensor values
	HardwareMap hardware_map;

};

#endif //NAOInterface_H
