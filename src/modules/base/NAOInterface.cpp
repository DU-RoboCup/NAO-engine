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
void NAOInterface::Reconfigure(std::string config_file, uint16_t id) {
	this->ModuleID = id;
	LuaTable mconfig = LuaTable::fromFile(config_file.c_str());
	this->ModuleName = mconfig["name"].get<std::string>();
	this->ModuleFPS = static_cast<int>(mconfig["rfps"].get<float>());
	this->ModulePriority = static_cast<int>(mconfig["mprio"].get<float>());
	this->ModuleThread = static_cast<int>(mconfig["mthr"].get<float>());
}
bool NAOInterface::RunFrame()
{
    hardware_write_test();
	if(!pending_intents.empty()) 
	{
		//Do intent stuff: NOTE - This should be replaced with a ProcessIntent feature in the Intent object
		try 
		{
			ParsedIntent pi = pending_intents.pop_front().Parse();
			if(pi[2] == "set_hardware_value"){
				if(pi.size() < 5)
					set_hardware_value(pi[3], std::stof(pi[4]));

				//TODO: Write Dispatch Table for ENUM C++
				// else 
				// {
				// 	try{
				// 		
				// 		std::string hw_cmp = pi[3];
				// 		QPRIORITY_FLAG c_flag = static_cast<QPRIORITY_FLAG>(std::stoi(pi[5]));
				// 		set_hardware_value(hw_cmp, std::stof(pi[4]), c_flag);
				// 	}
				// 	catch(const std::exception &e)
				// 	{
				// 		std::cout << "An exception has occured: " << e.what() << "\n";
				// 	}
				// }
			}
			else if(pi[2] == "get_hardware_value")
				get_hardware_value(pi[3]);
			else
			{
				LOG_WARNING << "No accessable function named " << pi[3] << " in NAOInterface";
				return false; 
			}
		} catch(const std::exception &e) {
			LOG_WARNING << "Intent Parsing Exception: " << e.what() << "";
		}
		LOG_DEBUG << "Doing stuff with intents";
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
{
	BOOST_LOG_FUNCTION();

	head = std::make_shared<Head>();
	RightArm = std::make_shared<RArm>();
	LeftArm = std::make_shared<LArm>();
	LeftLeg = std::make_shared<LLeg>();
	RightLeg = std::make_shared<RLeg>();

    randomly_set_joints();
	//Sets up map of get function pointer and set function pointers
	initialize_function_map();

	LOG_DEBUG << "NAOInterface Initialized";
	// print_hardware_map();

	LOG_DEBUG << "NAOInterface is attempting to open shared memory object";
	if(!access_shared_memory()) LOG_FATAL << "NAOInterface could not access shared memory";
	else
	{
		// Pre-allocate our vectors with values. This should help provide better Caching performance.
		sensor_vals.assign(NumOfSensorIds, 0);
		actuator_vals.assign(NumOfActuatorIds, 0);
	}

}

void NAOInterface::print_commands_list()
{
	if (command_list.size() != 0)
	{
		for (int i = 0; i < command_list.size(); ++i)
			std::cout << "command[" << i << "]: " << command_list[i] << std::endl;
	}
}

float NAOInterface::generate_random_bound_val(float min, float max)
{
	assert(max > min);
	float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	float r = rand * (max - min);
	return min + r;
}
float NAOInterface::generate_random_bound_val(std::pair<float, float> bounds)
{
	auto min = bounds.first;
	auto max = bounds.second;
	assert(max > min);
	float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	float r = rand * (max - min);
	return min + r;
}

[[deprecated("please use the other set_hardware_value functions, which allow prioritization")]]
bool NAOInterface::set_hardware_value(const std::string & hardware_component, float value)
{
	if (hardware_set_functions.find(hardware_component) == hardware_set_functions.end())
	{
		LOG_WARNING << "Invalid Hardware Component Name! Please refer to the Wiki for the valid names";
		return false;
	}
	else 
	{
		hardware_set_functions[hardware_component](value);
		LOG_DEBUG << hardware_component << " Updated!";
		return true;
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
bool NAOInterface::set_hardware_value(const unsigned int &hardware_component, const float value)
{
	LOG_DEBUG << "Attempting to do a direct hardware value write.";
	bool all_success = false;
	if(! hardware_component >= NumOfActuatorIds)
	{
		try
		{
			pineappleJuice->actuator_semaphore.wait();
			//The write
			pineappleJuice->actuators[0][hardware_component] = value;
			pineappleJuice->actuators_newest_update = hardware_component;
			LOG_DEBUG << actuatorNames[hardware_component] << " set to value " << value;

			pineappleJuice->actuator_semaphore.post();
			all_success = true;
		}
		catch (const interprocess_exception &e)
		{
			LOG_WARNING << "Value failed to be set due to: " << e.what();
			all_success = false;
				
		}
	}
	return all_success;
}
bool NAOInterface::fast_write()
{
	/*
	LOG_DEBUG << "FAST_SET_REF_COUNT: " << pineappleJuice->fast_access_value.use_count();
	if(pineappleJuice->fast_access_value.use_count() != pineappleJuice->fast_actuator_id.use_count())
	{
		LOG_WARNING << "Ref count of fast_actuator_id and fast_access_value are different. It be dangerous to write in this volatility!";
		return false;
	}
	if(pineappleJuice->fast_access_value.use_count() <= 2 && WriteRequests.size() != 0)
	{
		if(pineappleJuice->fast_access_value == 0)
		{
			LOG_DEBUG << "We can successfully quickly write data to IPC";
			auto data_request = WriteRequests.pop_top();
			pineappleJuice->fast_access_value = boost::make_shared<float>(std::get<1>(data_request));
			pineappleJuice->fast_actuator_id = boost::make_shared<int>(std::get<0>(data_request));
			return true;
		}
		else return false;
	}
	LOG_WARNING << "Fast Write Failed.";
	*/
	return false;
}
bool NAOInterface::get_hardware_value(const std::string &hardware_component)
{
	if(hardware_get_map.find(hardware_component) == hardware_get_map.end())
	{
		LOG_WARNING << "Invalid Hardware Component Name! Please refer to the Wiki for the valid names";
	}
	else
	{
		//Todo: Implement Return Intent with requested value
		auto val = hardware_get_map[hardware_component]();
		LOG_DEBUG << hardware_component << ": " << val;
	}
}
bool NAOInterface::access_shared_memory()
{
	try
	{
        shm = managed_shared_memory(open_or_create, "PineappleJuice", 65536); /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
		pineappleJuice = shm.find<hal_data>("juicyData").first;
		if(!pineappleJuice) LOG_FATAL << "Could not open shared memory object pineappleJuice!";
	}
	catch (const interprocess_exception &e)
	{
		LOG_WARNING << "NAOInterface could not open required shared memory objects. Error Code: " << e.what() << ".";
		return false;
	}
	return true;
}
bool NAOInterface::sync_pineapple()
{
	//ToDo: Post a semaphore and read value in shared memory using Boost.Interprocess
	
	
	// Fast Write
	pineappleJuice->actuator_semaphore.wait();
	for(int i = 0; i < WriteRequests.size(); ++i)
	{
		auto &top_intent = WriteRequests.pop_top();
		pineappleJuice->actuators[pineappleJuice->actuators_newest_update][std::get<0>(top_intent)] = std::get<1>(top_intent);
	
	}
	pineappleJuice->actuator_semaphore.post();
	return true;
}
bool NAOInterface::read_shared_memory()
{
	/** Unfortunately this operation is quite slow due to the time it takes to loop through
		the list of sensors and actuators and update their values. It is much faster to use the 
		other provided method for reading a single value from the sensors and actuators.
	**/
	
	try
	{
		LOG_DEBUG << "NAOInterface is accessing the semaphore";
		pineappleJuice->sensor_semaphore.wait();
		for(int i = 0; i < NumOfSensorIds;++i)
		{
			sensor_vals[i] = pineappleJuice->sensors[pineappleJuice->sensors_newest_read][i];
		}
		pineappleJuice->sensor_semaphore.post();
		pineappleJuice->actuator_semaphore.wait();
		for(int i = 0; i < NumOfActuatorIds;++i)
		{
			actuator_vals[i] = pineappleJuice->actuators[pineappleJuice->sensors_newest_read][i];
		}
		pineappleJuice->actuator_semaphore.post();
		
	}
	catch (const interprocess_exception &e)
	{
		LOG_WARNING << "Could not access robo_semaphore due to: " << e.what() << ". NaoQI Probably crashed.";
		return false;
	}
	return true;
}
bool NAOInterface::write_shared_memory()
{
	try
	{
		LOG_DEBUG << "NAOInterface is accessing the semaphore";
		pineappleJuice->actuator_semaphore.wait();
		while(!WriteRequests.size() == 0)
		{
			auto &request = WriteRequests.pop_top();
			pineappleJuice->actuators[0][std::get<0>(request)] = std::get<1>(request);
			pineappleJuice->actuators_current_read = std::get<0>(request);
		}
		
		pineappleJuice->actuator_semaphore.post();
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
	std::vector<std::pair<const unsigned int, const float>> writeTests {
		{faceLedGreenLeft180DegActuator, 1.0},
		{faceLedGreenLeft225DegActuator, 1.0},
		{faceLedBlueLeft90DegActuator, 1.0},
		{faceLedRedLeft0DegActuator, 1.0},
		{lShoulderPitchStiffnessActuator, 0.9},
		{lShoulderRollStiffnessActuator, 0.9}

	};

	for (auto &val : writeTests)
	{
		if(set_hardware_value(val.first, val.second))
		{
			LOG_DEBUG << actuatorNames[val.first] << " written successfully!";
		} else {
			LOG_WARNING << actuatorNames[val.first] << " failed to write successfully.";
		}
	}
}
void NAOInterface::randomly_set_joints()
{
	//Todo: Stiffness values

	//Set random (Bounded) actuator values for the RightArm
	RightArm->set_elbow(generate_random_bound_val(RightArm->elbow_roll_bounds), generate_random_bound_val(RightArm->elbow_yaw_bounds));
	RightArm->set_shoulder(generate_random_bound_val(RightArm->shoulder_roll_bounds), generate_random_bound_val(RightArm->shoulder_pitch_bounds));
	RightArm->set_wrist(generate_random_bound_val(RightArm->wrist_bounds));
	RightArm->print_arm_status();
	RightArm->current_actuator_vals();

	//Set random (Bounded) actuator values for the LeftArm
	LeftArm->set_elbow(generate_random_bound_val(LeftArm->elbow_roll_bounds), generate_random_bound_val(LeftArm->elbow_yaw_bounds));
	LeftArm->set_shoulder(generate_random_bound_val(LeftArm->shoulder_roll_bounds), generate_random_bound_val(LeftArm->shoulder_pitch_bounds));
	LeftArm->set_wrist(generate_random_bound_val(LeftArm->wrist_bounds));
	LeftArm->current_actuator_vals();

	////Set random (Bounded) actuator values for the RightLeg
	//auto RLBounds = RightLeg->get_actuator_bounds();
	//RightLeg->set_hip(generate_random_bound_val(RLBounds[0]), generate_random_bound_val(RLBounds[1]), generate_random_bound_val(RLBounds[2]));
	//RightLeg->set_knee(generate_random_bound_val(RLBounds[4]));
	//RightLeg->set_ankle(generate_random_bound_val(RLBounds[5]), generate_random_bound_val(RLBounds[6]));

	////Set random (Bounded) actuator values for the LeftLeg
	//auto LLBounds = LeftLeg->get_actuator_bounds();
	//LeftLeg->set_hip(generate_random_bound_val(LLBounds[0]), generate_random_bound_val(LLBounds[1]), generate_random_bound_val(LLBounds[2]));
	//LeftLeg->set_knee(generate_random_bound_val(LLBounds[4]));
	//LeftLeg->set_ankle(generate_random_bound_val(LLBounds[5]), generate_random_bound_val(LLBounds[6]));
	
}

void NAOInterface::print_hardware_map()
{
	std::cout << "print hardware map. Number of keys: " << std::to_string(hardware_set_functions.size()) << std::endl;
	for (auto k : hardware_set_functions) {
		std::cout << k.first << std::endl;
	}
}

void NAOInterface::initialize_function_map()
{
	//auto head_all = boost::bind(&Head::set_head_actuators, head, boost::placeholders::_1);
	//auto head_yaw = boost::bind(&Head::set_yaw, head, boost::placeholders::_1);
	using namespace std::placeholders;
	hardware_set_functions = {
		{"HEAD::ACTUATORS::YAW",  std::bind(&Head::set_yaw, head, _1) },
		{"HEAD::ACTUATORS::PITCH", std::bind(&Head::set_pitch, head, _1) }, 
		//TODO: EyeLEDS, Stiffness, Body
		//RArm
		{"RARM::SHOULDER::ACTUATORS::PITCH::POSITION", std::bind(&RArm::set_shoulder_pitch, RightArm, _1)},
		{"RARM::SHOULDER::ACTUATORS::ROLL::POSITION", std::bind(&RArm::set_shoulder_roll, RightArm, _1)},
		{"RARM::ELBOW::ACTUATORS::YAW::POSITION", std::bind(&RArm::set_elbow_yaw, RightArm, _1)},
		{"RARM::ELBOW::ACTUATORS::ROLL::POSITION", std::bind(&RArm::set_elbow_roll, RightArm, _1)},
		{"RARM::WRIST::ACTUATORS::YAW::POSITION", std::bind(&RArm::set_wrist_yaw, RightArm, _1)},
		//{"RARM::HAND::POSITION", std::bind(&RArm::set_hand_position, RightArm, _1)},
		//LArm
		{ "LARM::SHOULDER::ACTUATORS::PITCH::POSITION", std::bind(&LArm::set_shoulder_pitch, LeftArm, _1) },
		{ "LARM::SHOULDER::ACTUATORS::ROLL::POSITION", std::bind(&LArm::set_shoulder_roll, LeftArm, _1) },
		{ "LARM::ELBOW::ACTUATORS::YAW::POSITION", std::bind(&LArm::set_elbow_yaw, LeftArm, _1) },
		{ "LARM::ELBOW::ACTUATORS::ROLL::POSITION", std::bind(&LArm::set_elbow_roll, LeftArm, _1) },
		{ "LARM::WRIST::ACTUATORS::YAW::POSITION", std::bind(&LArm::set_wrist_yaw, LeftArm, _1) },
		//{"LARM::HAND::POSITION", std::bind(&LArm::set_hand_position, LeftArm, _1)}
		//RLeg
		{"RLEG::HIP::ACTUATORS::ROLL::POSITION", std::bind(&RLeg::set_hip_roll, RightLeg, _1)},
		{"RLEG::HIP::ACTUATORS::YAW::POSITION", std::bind(&RLeg::set_hip_yaw, RightLeg, _1)},
		{"RLEG::HIP::ACTUATORS::PITCH::POSITION", std::bind(&RLeg::set_hip_pitch, RightLeg, _1)},
		//{ "RLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_hip_pitch_stiffness, RightLeg, _1) },
		{"RLEG::KNEE::ACTUATORS::PITCH::POSITION", std::bind(&RLeg::set_knee_pitch, RightLeg, _1) },
		{"RLEG::ANKLE::ACTUATORS::ROLL::POSITION", std::bind(&RLeg::set_ankle_roll, RightLeg, _1) },
		{"RLEG::ANKLE::ACTUATORS::PITCH::POSITION", std::bind(&RLeg::set_ankle_pitch, RightLeg, _1) },
		//LLeg
		//{"LLEG::HIP::ACTUATORS::YAWPITCH::POSITION", std::bind(...)}
		{ "LLEG::HIP::ACTUATORS::ROLL::POSITION", std::bind(&LLeg::set_hip_roll, LeftLeg, _1) },
		//TODO: CHANGE YAW->PITCH and add YawPitch
		{ "LLEG::HIP::ACTUATORS::YAW::POSITION", std::bind(&LLeg::set_hip_yaw, LeftLeg, _1) },
		{ "LLEG::KNEE::ACTUATORS::PITCH::POSITION", std::bind(&LLeg::set_knee_pitch, LeftLeg, _1) },
		{ "LLEG::ANKLE::ACTUATORS::PITCH::POSITION", std::bind(&LLeg::set_ankle_pitch, LeftLeg, _1) },
		{ "LLEG::ANKLE::ACTUATORS::ROLL::POSITION", std::bind(&LLeg::set_ankle_roll, LeftLeg, _1) },

		//STIFFNESS

		//{"HEAD::ACTUATORS::YAW::STIFFNESS", std::bind(&Head::set_yaw_stiffness, head, _1)},
		//{"HEAD::ACTUATORS::PITCH::STIFFNESS", std::bind(&Head::set_pitch_stiffness, head, _1)},
		
		//Left Arm
		//{"LARM::SHOULDER::ACTUATORS::PITCH::STIFFNESS", std::bind(&Arm::set_pitch_stiffness, LeftArm, _1)},
		//{"LARM::SHOULDER::ACTUATORS::ROLL::STIFFNESS", std::bind(&Arm::set_roll_stiffness, LeftArm, _1)},
		//{"LARM::ELBOW::ACTUATORS::YAW::STIFFNESS", std::bind(&Arm::set_elbow_yaw_stiffness, LeftArm, _1)},
		//{"LARM::ELBOW::ACTUATORS::ROLL::STIFFNESS", std::bind(&Arm::set_elbow_roll_stiffness, LeftArm, _1)},
		//{"LARM::WRIST::ACTUATORS::YAW::STIFFNESS", std::bind(&Arm::set_hand_yaw_stiffness, LeftArm, _1)},
		//{"LARM::HAND::ACTUATORS::STIFFNESS", std::bind(&Arm::set_hand_stiffness, LeftArm, _1)},
		//Right Arm
		//{"RARM::SHOULDER::ACTUATORS::PITCH::STIFFNESS", std::bind(&Arm::set_pitch_stiffness, RightArm, _1)},
		//{"RARM::SHOULDER::ACTUATORS::ROLL::STIFFNESS", std::bind(&Arm::set_roll_stiffness, RightArm, _1)},
		//{"RARM::ELBOW::ACTUATORS::YAW::STIFFNESS", std::bind(&Arm::set_elbow_yaw_stiffness, RightArm, _1)},
		//{"RARM::ELBOW::ACTUATORS::ROLL::STIFFNESS", std::bind(&Arm::set_elbow_roll_stiffness, RightArm, _1)},
		//{"RARM::WRIST::ACTUATORS::YAW::STIFFNESS", std::bind(&Arm::set_hand_yaw_stiffness, RightArm, _1)},
		//{"RARM::HAND::ACTUATORS::STIFFNESS", std::bind(&Arm::set_hand_stiffness, RightArm, _1)},
		//Left Leg
		//{"LLEG::HIP::ACTUATORS::YAWPITCH::STIFFNESS", std::bind(&LLeg::set_hip_yawpitch_stiffness, LeftLeg, _1)},
		{ "LLEG::HIP::ACTUATORS::ROLL::STIFFNESS", std::bind(&LLeg::set_hip_roll_stiffness, LeftLeg, _1) },
		//TODO: Remove Yaw from hips
		//{ "LLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&LLeg::set_hip_pitch_stiffness, LeftLeg, _1) },
		{ "LLEG::KNEE::ACTUATORS::PITCH::STIFFNESS", std::bind(&LLeg::set_knee_pitch_stiffness, LeftLeg, _1) },
		{ "LLEG::ANKLE::ACTUATORS::PITCH::STIFFNESS", std::bind(&LLeg::set_ankle_pitch_stiffness, LeftLeg, _1) },
		{ "LLEG::ANKLE::ACTUATORS::ROLL::STIFFNESS", std::bind(&LLeg::set_ankle_roll_stiffness, LeftLeg, _1) },
		//Right Leg
		//{"RLEG::HIP::ACTUATORS::YAWPITCH::STIFFNESS", std::bind(&RLeg::set_hip_yawpitch_stiffness, RightLeg, _1)},
		{ "RLEG::HIP::ACTUATORS::ROLL::STIFFNESS", std::bind(&RLeg::set_hip_roll_stiffness, RightLeg, _1) },
		//TODO: Remove Yaw from hips
		//{ "RLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_hip_pitch_stiffness,RightLeg, _1) },
		{ "RLEG::KNEE::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_knee_pitch_stiffness, RightLeg, _1) },
		{ "RLEG::ANKLE::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_ankle_pitch_stiffness, RightLeg, _1) },
		{ "RLEG::ANKLE::ACTUATORS::ROLL::STIFFNESS", std::bind(&RLeg::set_ankle_roll_stiffness, RightLeg, _1) },
	};

	hardware_get_map = {
		{ "HEAD::ACTUATORS::YAW::POSITION",  std::bind(&Head::get_head_yaw, head) },
		{ "HEAD::ACTUATORS::PITCH::POSITION", std::bind(&Head::get_head_pitch, head) },
		//TODO: EyeLEDS, Stiffness, Body
		//RArm
		{ "RARM::SHOULDER::ACTUATORS::ROLL::POSITION", std::bind(&Arm::get_shoulder_roll, RightArm) },
		{ "RARM::SHOULDER::ACTUATORS::PITCH::POSITION", std::bind(&Arm::get_shoulder_pitch, RightArm) },
		{ "RARM::ELBOW::ACTUATORS::ROLL::POSITION", std::bind(&Arm::get_elbow_roll, RightArm) },
		{ "RARM::ELBOW::ACTUATORS::YAW::POSITION", std::bind(&Arm::get_elbow_yaw, RightArm) },
		{ "RARM::WRIST::ACTUATORS::YAW::POSITION", std::bind(&Arm::get_wrist_yaw, RightArm) },
		//LArm
		{ "LARM::SHOULDER::ACTUATORS::ROLL::POSITION", std::bind(&Arm::get_shoulder_roll, LeftArm) },
		{ "LARM::SHOULDER::ACTUATORS::PITCH::POSITION", std::bind(&Arm::get_shoulder_pitch, LeftArm) },
		{ "LARM::ELBOW::ACTUATORS::ROLL::POSITION", std::bind(&Arm::get_elbow_roll, LeftArm) },
		{ "LARM::ELBOW::ACTUATORS::YAW::POSITION", std::bind(&Arm::get_elbow_yaw, LeftArm) },
		{ "LARM::WRIST::ACTUATORS::YAW::POSITION", std::bind(&Arm::get_wrist_yaw, LeftArm) },
		//RLeg
		{ "RLEG::HIP::ACTUATORS::ROLL::POSITION", std::bind(&Leg::get_hip_roll, RightLeg) },
		{ "RLEG::HIP::ACTUATORS::ROLL::STIFFNESS", std::bind(&Leg::get_hip_roll_stiffness, RightLeg) },

		{ "RLEG::HIP::ACTUATORS::YAW::POSITION", std::bind(&Leg::get_hip_yaw, RightLeg) },
		{ "RLEG::HIP::ACTUATORS::YAW::STIFFNESS", std::bind(&Leg::get_hip_yaw_stiffness, RightLeg) },

		{ "RLEG::HIP::ACTUATORS::PITCH::POSITION", std::bind(&Leg::get_hip_pitch, RightLeg) },
		{ "RLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&Leg::get_hip_pitch_stiffness, RightLeg) },

		{ "RLEG::KNEE::ACTUATORS::PITCH::POSITION", std::bind(&Leg::get_knee_pitch, RightLeg) },
		{ "RLEG::KNEE::ACTUATORS::PITCH::STIFFNESS", std::bind(&Leg::get_knee_pitch_stiffness, RightLeg)},

		{ "RLEG::ANKLE::ACTUATORS::ROLL::POSITION", std::bind(&Leg::get_ankle_roll, RightLeg) },
		{ "RLEG::ANKLE::ACTUATORS::ROLL::STIFFNESS", std::bind(&Leg::get_ankle_roll_stiffness, RightLeg) },

		{ "RLEG::ANKLE::ACTUATORS::PITCH::POSITION", std::bind(&Leg::get_ankle_pitch, RightLeg) },
		{ "RLEG::ANKLE::ACTUATORS::PITCH::STIFFNESS", std::bind(&Leg::get_ankle_pitch_stiffness, RightLeg) },

		//LLeg
		{ "LLEG::HIP::ACTUATORS::ROLL::POSITION", std::bind(&Leg::get_hip_roll, LeftLeg) },
		{ "LLEG::HIP::ACTUATORS::ROLL::STIFFNESS", std::bind(&Leg::get_hip_roll_stiffness, LeftLeg) },

		{ "LLEG::HIP::ACTUATORS::YAW::POSITION", std::bind(&Leg::get_hip_yaw, LeftLeg) },
		{ "LLEG::HIP::ACTUATORS::YAW::STIFFNESS", std::bind(&Leg::get_hip_yaw_stiffness, LeftLeg) },

		{ "LLEG::HIP::ACTUATORS::PITCH::POSITION", std::bind(&Leg::get_hip_pitch, LeftLeg) },
		{ "LLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&Leg::get_hip_pitch_stiffness, LeftLeg) },

		{ "LLEG::KNEE::ACTUATORS::PITCH::POSITION", std::bind(&Leg::get_knee_pitch, LeftLeg) },
		{ "LLEG::KNEE::ACTUATORS::PITCH::STIFFNESS", std::bind(&Leg::get_knee_pitch_stiffness, LeftLeg) },

		{ "LLEG::ANKLE::ACTUATORS::ROLL::POSITION", std::bind(&Leg::get_ankle_roll, LeftLeg) },
		{ "LLEG::ANKLE::ACTUATORS::ROLL::STIFFNESS", std::bind(&Leg::get_ankle_roll_stiffness, LeftLeg) },

		{ "LLEG::ANKLE::ACTUATORS::PITCH::POSITION", std::bind(&Leg::get_ankle_pitch, LeftLeg) },
		{ "LLEG::ANKLE::ACTUATORS::PITCH::STIFFNESS", std::bind(&Leg::get_ankle_pitch_stiffness, LeftLeg) },
	};
}

