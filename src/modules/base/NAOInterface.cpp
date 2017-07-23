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
#include "include/modules/base/NAOInterface.h"
//Macro for semaphore name
#ifndef PINEAPPLE_SEMAPHORE
    #define PINEAPPLE_SEMAPHORE "pineapple_semaphore"
#endif

REGISTER_MODULE(NAOInterface);
//Singleton stuff
NAOInterface* NAOInterface::instance;
NAOInterface* NAOInterface::Instance()
{
	if(UNLIKELY(NAOInterface::instance==0))
	{
		NAOInterface::instance = new NAOInterface();
	}
	return NAOInterface::instance;
}

bool NAOInterface::RunFrame()
{
	BOOST_LOG_FUNCTION(); //Gets function traces
	if(!shared_memory_setup)
	{
		//bool res = access_shared_memory();
		//create_shared_memory();
		LOG_WARNING << "Shared mem not setup!" << std::endl;
	}
	LOG_DEBUG << "Shared Memory Setup! Doing a hardware test...";
	read_shared_memory();
    hardware_write_test();
	LOG_DEBUG << "Hardware Test Done!";
	if(!pending_intents.empty())
	{
		LOG_DEBUG << "Popping intent...";
		const ParsedIntent new_val = pending_intents.pop_front().Parse();
		auto map_val = hardware_map[new_val[2]]; //K,V pair. 
		if(LIKELY(map_val.second != "NULL"))
		{
			//Do intent stuff here
			LOG_DEBUG << "New Hardware Value [" << map_val.first << "]: " << map_val.second;
		
		}
		else
			LOG_WARNING << "The Hardware Component " << new_val[2] << "Does not exist!" << std::endl;
	}
	else
	{
		LOG_WARNING << "No new intents for NAOInterface!";
	}
	return true;
}
bool NAOInterface::ProcessIntent(Intent &i)
{
	//DO NOT MODIFY
	pending_intents.push_back(i);
	return true;
}
bool NAOInterface::Install()
{
	LOG_DEBUG << "Installed 'NAOInterface' Module";
	return true;
}

bool NAOInterface::Uninstall()
{
	LOG_DEBUG << "Uninstalled 'NAOInterface' Module. The robot should be shutting down...";
}

using namespace boost::interprocess;

NAOInterface::NAOInterface() 
	/*:
		actuator_semaphore(open_or_create, "actuator_semaphore", 1),
        sensor_semaphore(open_or_create, "sensor_semaphore", 1)*/
{
	LOG_WARNING << "Initializing NAOInterface";

	shared_memory_setup = false;

	LOG_DEBUG << "NAOInterface Initialized";

	LOG_DEBUG << "NAOInterface is attempting to open shared memory object";
	bool shared_mem_access_ret_code = access_shared_memory();
	if(!shared_mem_access_ret_code) LOG_FATAL << "NAOInterface could not access shared memory";

	LOG_DEBUG << "Calling bazaar init";
	bool bazaar_ret_code = initialize_bazaar_data();

}


bool NAOInterface::initialize_bazaar_data()
{
	LOG_WARNING << "Initializing the Bazaar data";
	std::vector<float> t_sensor_values(NumOfSensorIds);
	std::vector<float> t_actuator_values(NumOfActuatorIds);

	try
	{
		LOG_DEBUG << "NAOInterface is accessing the semaphore";
		while(!sem_trywait(semaphore))
			LOG_DEBUG << "Waiting for semaphore access...";

		//Create a NAO-Engine local copy of the data stored in shared memory
		std::copy(&pineappleJuice->sensor_values[0], &pineappleJuice->sensor_values[NumOfSensorIds], t_sensor_values.begin());
		LOG_DEBUG << "sensor values copied";
		std::copy(&pineappleJuice->actuator_values[0], &pineappleJuice->actuator_values[NumOfActuatorIds], t_actuator_values.begin());
		LOG_DEBUG << "actuator values copied";
		sem_post(semaphore);
		LOG_DEBUG << "Semaphore posted";
	}
	catch (const interprocess_exception &e)
	{
		LOG_WARNING << "Could not access robo_semaphore due to: " << e.what() << ". NaoQI Probably crashed.";
		return false;
	}
	LOG_DEBUG << "Adding data to Bazaar...";
	Bazaar::Vend("sensor_data", std::make_shared<boost::any>(t_sensor_values));
	Bazaar::Vend("actuator_data", std::make_shared<boost::any>(t_actuator_values));
	sensor_values_dt = Bazaar::Get("sensor_data");
	actuator_values_dt = Bazaar::Get("actuator_data");

	try
	{
		sensor_vals = boost::any_cast<std::vector<float>>(&*sensor_values_dt);
		actuator_vals = boost::any_cast<std::vector<float>>(&*actuator_values_dt);
	}
	catch(const boost::bad_any_cast &e)
	{
		LOG_WARNING << "Failed to cast boost any values: " << e.what();
	}

	LOG_DEBUG << "Done adding data to the Bazaar...";
	return true;
}

void NAOInterface::print_commands_list()
{
	if (command_list.size() != 0)
	{
		for (int i = 0; i < command_list.size(); ++i)
			std::cout << "command[" << i << "]: " << command_list[i] << std::endl;
	}
}


bool NAOInterface::set_hardware_value(const int &hardware_component, float value, QPRIORITY_FLAG FLAG=DEFAULT)
{
	if(hardware_component < NumOfActuatorIds && hardware_component >= 0)
	{
		WriteRequests.push(std::make_tuple(hardware_component, value, FLAG));
	}
	else
	{
		LOG_WARNING << "Invalid Actuator ID: " << hardware_component; 
		return false;
	}
	return true;
}
bool NAOInterface::speak(const std::string &text)
{
	if(text.size() > 34)
	{
		//Realistically the NAO will overheat if the text is too long (apparently...have not tested yet)
		LOG_WARNING << "The Text you have asked NAO to say is too long for it to remember.";
		return false;
	}
	// pineappleJuice->speak_semaphore.wait();
	// std::strcpy(pineappleJuice->text_to_speak_unsafe, text.c_str());
	// //std::strncpy(pineappleJuice->text_to_speak_unsafe, text, sizeof(pineappleJuice->text_to_speak_unsafe)-1);
	// pineappleJuice->speak_semaphore.post();
	return true;
}
bool NAOInterface::set_hardware_value(const unsigned int &hardware_component, const float value)
{
	//LOG_DEBUG << "Attempting to do a direct hardware value write.";
	bool all_success = false;
	if(! hardware_component >= NumOfActuatorIds)
	{
		try
		{
		 std::cout << "temp" << std::endl;
		}
		catch (const interprocess_exception &e)
		{
			LOG_WARNING << "Value failed to be set due to: " << e.what();
			all_success = false;
				
		}
	}
	return all_success;
}

bool NAOInterface::access_shared_memory()
{
	LOG_DEBUG << "Accessing shared memory...";
	try
	{
        shm = managed_shared_memory(open_only, "PineappleJuice"); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
		if(shm.get_size() < 65536)
		{
			LOG_WARNING << "Error, there managed memory regions size is incorrect!";
		} else {
			LOG_DEBUG << "Found share memory region PineappleJuice!";
		}
		auto shmem_region_ptr = shm.find_no_lock<hal_data>("juicyData");
		if(!shmem_region_ptr.first)
		{
			LOG_FATAL << "Error, could not find shared memory object juicyData!!!";
		} else {
			LOG_WARNING << "Found shared memory object juicyData!";
			pineappleJuice = shmem_region_ptr.first;
		}
		LOG_WARNING << "Shared Memory should be setup!";
	}
	catch (const interprocess_exception &e)
	{
		LOG_WARNING << "NAOInterface could not open required shared memory objects. Error Code: " << e.what() << ".";
		return false;
	}
	semaphore = sem_open(PINEAPPLE_SEMAPHORE, O_RDWR);
	if(semaphore == SEM_FAILED)
	{
		LOG_WARNING << "Named semaphore does not exist, creating now...";
		semaphore = sem_open(PINEAPPLE_SEMAPHORE, O_CREAT, 0666, 1);
	} else {
		LOG_WARNING << "Semaphore already created!";
	}
	shared_memory_setup = true;

	// read_shared_memory();	
	return true;
}

void NAOInterface::create_shared_memory()
{
	try
    {
        permissions shm_permissions;
        LOG_DEBUG << "[create_int_memory]: Current permissions: " << shm_permissions.get_permissions();
        shm_permissions.set_unrestricted();
        LOG_DEBUG << "[create_int_memory]: Updated permissions: " << shm_permissions.get_permissions() << std::endl;
        LOG_DEBUG << "Initializing Shared Memory with Boost.Interprocess. [PineappleJuice]\n";
        shm = boost::interprocess::managed_shared_memory(open_or_create, "PineappleJuice", 65536, 0, 0666/*shm_permissions*/); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
        LOG_DEBUG << "Opened pineapple juice" << std::endl;
        if(shm.get_size() != 65536)
        {
            LOG_DEBUG << "ERROR: Did not allocate enough memory. SHM size: " << shm.get_size() << "\n";
        } else {
            LOG_DEBUG << "PineappleJuice created with size of: " << shm.get_size() << std::endl;
        }
        pineappleJuice = shm.find_or_construct<hal_data>("juicyData")(/*Constructor, assuming use of default constructor*/);
        shared_data_ptr = shm.find<hal_data>("juicyData");
        if(!shared_data_ptr.first)
        {
            LOG_DEBUG << "[FATAL] juicyData failed to be found!";
            return;
        } else {
            LOG_DEBUG << "juicyData Created!";
            pineappleJuice = shared_data_ptr.first; 
        }
		shared_memory_setup = true;
        ///< Dumb hack to stop boost interprocess from causing segfaults
       LOG_DEBUG << "pineappleJuice created in shared memory!";
       std::cout << "Interprocess memory setup" << std::endl;
    }
    catch(const boost::interprocess::interprocess_exception &e) 
    {
        LOG_DEBUG << "[FATAL] An Interprocess error: " << e.what() << ". Cleaning up shared memory..." << std::endl;
        shm.destroy<hal_data>("juicyData");
        boost::interprocess::shared_memory_object::remove("PineappleJuice");
    }
    LOG_DEBUG << "Interprocess memory created" << std::endl;
}
bool NAOInterface::sync_pineapple()
{
	//ToDo: Post a semaphore and read value in shared memory using Boost.Interprocess
	
	
	// Fast Write
	// pineappleJuice->actuator_semaphore.wait();
	// for(int i = 0; i < WriteRequests.size(); ++i)
	// {
	// 	auto &top_intent = WriteRequests.pop_top();
	// 	pineappleJuice->actuators_unsafe[std::get<0>(top_intent)] = std::get<1>(top_intent);
	// 	//pineappleJuice->actuators[pineappleJuice->actuators_newest_update][std::get<0>(top_intent)] = std::get<1>(top_intent);
	
	// }
	// pineappleJuice->actuator_semaphore.post();
	return true;
}
bool NAOInterface::read_shared_memory()
{
	/** Unfortunately this operation is quite slow due to the time it takes to loop through
		the list of sensors and actuators and update their values. It is much faster to use the 
		other provided method for reading a single value from the sensors and actuators.
	**/
	LOG_DEBUG << "Reading shared memory";
	try
	{
		LOG_DEBUG << "NAOInterface is accessing the semaphore";
		while(!sem_trywait(semaphore))
			LOG_DEBUG << "Waiting for semaphore access...";

		//Create a NAO-Engine local copy of the data stored in shared memory
		std::copy(&pineappleJuice->sensor_values[0], &pineappleJuice->sensor_values[NumOfSensorIds], (*sensor_vals).begin());
		std::copy(&pineappleJuice->actuator_values[0], &pineappleJuice->actuator_values[NumOfActuatorIds], (*actuator_vals).begin());
		sem_post(semaphore);
		LOG_DEBUG << "Semaphore posted";
	}
	catch (const interprocess_exception &e)
	{
		LOG_WARNING << "Could not access robo_semaphore due to: " << e.what() << ". NaoQI Probably crashed.";
		return false;
	}
		// LOG_DEBUG << "NAOInterface is accessing the semaphore";
		// while(!sem_trywait(semaphore))
		// {
		// 	LOG_DEBUG << "Waiting for semaphore access...";
		// }
		// //Create a NAO-Engine local copy of the data stored in shared memory
		// std::copy(pineappleJuice->sensor_values, pineappleJuice->sensor_values + NumOfSensorIds, sensor_vals->begin());
		// LOG_DEBUG << "sensor values copied";
		// std::copy(pineappleJuice->actuator_values, pineappleJuice->actuator_values + NumOfActuatorIds, actuator_vals->begin());
		// LOG_DEBUG << "actuator values copied";
		// sem_post(semaphore);
		// LOG_DEBUG << "Semaphore posted";
	return true;
}
bool NAOInterface::write_shared_memory()
{
	try
	{
		LOG_DEBUG << "NAOInterface is accessing the semaphore";
		// pineappleJuice->actuator_semaphore.wait();
		// while(!WriteRequests.size() == 0)
		// {
		// 	auto &request = WriteRequests.pop_top();
		// 	pineappleJuice->actuators_unsafe[std::get<0>(request)] = std::get<1>(request);
		// 	//pineappleJuice->actuators[0][std::get<0>(request)] = std::get<1>(request);
		// 	pineappleJuice->actuators_current_read = std::get<0>(request);
		// }
		
		// pineappleJuice->actuator_semaphore.post();
	}
	catch (const interprocess_exception &e)
	{
		LOG_WARNING << "Could not access robo_semaphore due to: " << e.what() << ". NaoQI Probably crashed.";
		return false;
	}
	return true;
}
void NAOInterface::hardware_write_test()
{
	BOOST_LOG_FUNCTION();
	std::vector<float> testValues;
	testValues.resize(4);
	try
	{
		//LOG_DEBUG << "Waiting on sensor value semaphore...";
		//pineappleJuice->sensor_semaphore.wait();
		// sem_wait(semaphore);
		while(!sem_trywait(semaphore))
		{
			LOG_DEBUG << "Still Waiting...";
		}
		testValues[0] = pineappleJuice->sensor_values[rShoulderPitchPositionSensor];
		LOG_DEBUG << "rShoulderPitchPositionSensor: " << testValues[0];
		testValues[1] = -pineappleJuice->sensor_values[rShoulderRollPositionSensor];
		LOG_DEBUG << "rShoulderRollPositionSensor: " << testValues[1];
		testValues[2] = -pineappleJuice->sensor_values[rElbowYawPositionSensor];
		LOG_DEBUG << "rElbowYawPositionSensor: " << testValues[2];
		testValues[3] = -pineappleJuice->sensor_values[rElbowRollPositionSensor];
		LOG_DEBUG << "rElbowRollPositionSensor: " << testValues[3];
		//pineappleJuice->semaphore.post();
		//pineappleJuice->sensor_semaphore.post();
		//LOG_DEBUG << "Done with sensor value semaphore!";

		//LOG_DEBUG << "Waiting on actuator semaphore...";
		//pineappleJuice->actuator_semaphore.wait();
		pineappleJuice->actuator_values[lShoulderPitchPositionActuator] = testValues[0];
		pineappleJuice->actuator_values[lShoulderRollPositionActuator] = testValues[1];
		pineappleJuice->actuator_values[lElbowYawPositionActuator] = testValues[2];
		pineappleJuice->actuator_values[lElbowRollPositionActuator] = testValues[3];
		//pineappleJuice->actuator_semaphore.post();
		//LOG_DEBUG << "Done with Actuator Semaphore! New joint values set!";
	}
	catch(const interprocess_exception &e)
	{
		LOG_WARNING << "FAILED to set new hardware value!";
	}
	sem_post(semaphore);

}