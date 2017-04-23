
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

#ifndef NAO_INTERFACE_H
#define NAO_INTERFACE_H

//INCLUDES

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <queue>
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

#include "Head.h"
#include "RArm.h"
#include "LArm.h"
#include "LLeg.h"
#include "RLeg.h"
#include "HALPriorityQueue.h"
#include "include/common.h"
#include "include/debug/debugging.h"
#include "include/memory/hal_data.h"



/**
  * \class NAOInterface
  * Main class/module class for the NAOInterface. A number of other classes
  * are what are really doing most of the work. 
  **/

class NAOInterface : public Module
{
public:
	

	/*
	* hardware_datatypes - boost::variant datatype that can be set to a variety of different datatypes.
	**/
	typedef boost::variant < std::pair<float, float>, float, std::string, std::unordered_map<std::string, Head::EyeLED> > hardware_datatypes;
	void initialize_hardware_map(); ///< Initially sets all of the values of our absurd hashmap.
	void initialize_function_map(); ///< Initializes a map for calling functions.
	
	//Debug Functions
	void print_commands_list(); ///< Lists all available function calls
	void randomly_set_joints(); ///< This function randomly sets all of the joint values of the NAO's hardware.
	void print_hardware_map(); ///< Will probably be deprecated


	float generate_random_bound_val(float min, float max); ///< Generates a random number between a lower and upper limit
	float generate_random_bound_val(std::pair<float, float> bounds); ///< Generates a random value given a actuators bounds (in radians, don't use degrees).
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
	bool set_hardware_value(const std::string &hardware_component, float  value); 
	bool set_hardware_value(const int &hardware_component, float value, QPRIORITY_FLAG FLAG);
	bool set_hardware_value(const unsigned int &hardware_component, const float value);
	/**
	  * \brief: A dangerous way to quickly send queued requests to NAOqi
	  * 
	  * This method allows for lock/semaphore free passsing of a single value
	  * to NAOqi. The value sent is simply the top of the priority queue.
	  * Only use this for super high priority items.
	  **/
	bool fast_write();

	/**
	  * \brief Reads a hardware component value.
	  * \param hardware_component - The hardware component's name to be read from.
	  * 
	  */
	bool get_hardware_value(const std::string &hardware_component);

	/**
	  * \brief - Helper method for initializing access to shared memory. 
	  *
	  * This is not in the constructor because if NaoQI crashes, or our processes
	  * crashes in some odd way, it could be useful to re-open the shared memory.  
	  **/
	bool access_shared_memory();
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

	// Hardware Write Tests
	void hardware_write_test();

	typedef std::function<hardware_datatypes(hardware_datatypes)> generic_function;
	/*typedef std::map<std::string, generic_function> function_map;*/
	

	static NAOInterface* Instance();
    void Reconfigure(std::string config_file, uint16_t id);
    bool RunFrame();
    bool ProcessIntent(Intent &i);
    bool Install();
    bool Uninstall();
	/**
	  * shared_ptr's for Ligaments: Due to these being shared pointers, be careful with Ref Counts and how destruction is handled (it's not automatic).
	  * Need to be shared_ptrs for function pointers and binding.
	  **/
	std::shared_ptr<Head> head;
	std::shared_ptr<RArm> RightArm;
	std::shared_ptr<LArm> LeftArm;
	std::shared_ptr<LLeg> LeftLeg;
	std::shared_ptr<RLeg> RightLeg;

private:
    static NAOInterface* instance;
	NAOInterface();
    //Intent processing
    PendingIntents pending_intents;
	boost::interprocess::managed_shared_memory shm;
	hal_data *pineappleJuice; // Object stored in interprocess memory
	bool read_shm, write_shm; // Return Values for interprocess rw operations
	std::vector<float> sensor_vals, actuator_vals;
	
	/* Items to be written stored as tuples: <ActuatorIDnumber, Value, FLAG> */
	HAL_PQ WriteRequests;
protected: 
	std::unordered_map<std::string, std::function<void(float)>> hardware_set_functions; ///< unordered_map of API calls and value set function pointers
	std::unordered_map<std::string, std::function<float(void)>> hardware_get_map;///< unordered_map of API calls and value get function pointers
};

#endif //NAOInterface_H
