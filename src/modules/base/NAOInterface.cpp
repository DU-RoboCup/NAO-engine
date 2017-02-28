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
	this->ModuleFPS = (int)mconfig["rfps"].get<float>();
	this->ModulePriority = (int)mconfig["mprio"].get<float>();
	this->ModuleThread = (int)mconfig["mthr"].get<float>();
}
bool NAOInterface::RunFrame()
{
//Processing the intent queue
    // if (!pendingIntents.empty())
    // {
    //     ParsedIntent pi = Intent::Parse(pendingIntents.pop_front());
    //     if (pi[1] = "execute_intent_write")
    //     {
    //         execute_intent_write(pi[2], (pi.size() != 3) ? std::vector<float>{pi[3], pi.end()} : std::vector<float>{ pi[3] });
    //     }
    //         else if (pi[1] = "execute_intent_read")
    //     {
    //         execute_intent_read(pi[3], pi[4], pi[5]);
    //     }
    //     else {
    //         std::cout << "Error: NAOInterface does not contain function  " << pi[2] << std::endl;
    //         return false;
    //     }	
    //     //pi is now an array of split strings...Function calls for getting/setting can go here
	//     return true;
	// }
	if(!pendingIntents.empty()) 
	{
		//Do intent stuff
		//auto pi = Intent::Parse(pendingIntents.pop_front())
		sanity_test("0.o");
		LOG_DEBUG << "Doing stuff with intents";
	}
	return true;
}
bool NAOInterface::ProcessIntent(Intent &i)
{
	//DO NOT MODIFY
	pendingIntents.push_back(i);
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

NAOInterface::NAOInterface()
{

	head = std::make_shared<Head>();
	RightArm = std::make_shared<RArm>();
	LeftArm = std::make_shared<LArm>();
	LeftLeg = std::make_shared<LLeg>();
	RightLeg = std::make_shared<RLeg>();

    randomly_set_joints();
	//Sets up map of get function pointer and set function pointers
	initialize_hardware_map();
	initialize_function_map();

	std::cout << "NAOInterface Initialized" << std::endl;
	std::cout << "Testing..." << std::endl;
	print_hardware_map();
	sanity_test(":/");

}


void NAOInterface::parse_intent(const std::string &input)
{
	if (command_list.size() != 0) command_list.clear(); //clear list
	size_t pos = 0;
	std::string cpy = input;
	std::string delimiter = "/";
	while ((pos = cpy.find(delimiter)) != std::string::npos) {
		command_list.push_back(cpy.substr(0, pos));
		cpy.erase(0, pos + delimiter.length());
	}
	command_list.push_back(cpy);
	std::cout << "Intents parsed, command list size: " << command_list.size() << std::endl;
	//TODO: Stuff with intent list. Only intent[3] and [4] matter
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
	float rand = ((float)std::rand()) / (float) RAND_MAX;
	float r = rand * (max - min);
	return min + r;
}
float NAOInterface::generate_random_bound_val(std::pair<float, float> bounds)
{
	auto min = bounds.first;
	auto max = bounds.second;
	assert(max > min);
	float rand = ((float)std::rand()) / (float)RAND_MAX;
	float r = rand * (max - min);
	return min + r;
}

bool NAOInterface::set_hardware_value(const std::string & hardware_component, float  value)
{
	if (hardware_set_functions.find(hardware_component) == hardware_set_functions.end())
	{
		std::cout << "Invalid Hardware Component Name! Please refer to the Wiki for the valid names" << std::endl;
	}
	else 
	{
		hardware_set_functions[hardware_component](value);
		std::cout << hardware_component << " Updated!" << std::endl;
		return true;
	}
	
}
bool NAOInterface::get_hardware_value(const std::string &hardware_component)
{
	if(hardware_get_map.find(hardware_component) == hardware_get_map.end())
	{
		std::cout << "Invalid Hardware Component Name! Please refer to the Wiki for the valid names" << std::endl;
	}
	else
	{
		//Todo: Implement Return Intent with requested value
		auto val = hardware_get_map[hardware_component]();
		std::cout << hardware_component << ": " << val << std::endl;
	}
}

// Currently a half-assed solution.
bool NAOInterface::execute_intent_read(std::string & request_module, std::string & hardware_component, std::string & requested_value)
{
	//Checks if the hardware component referenced even exists.
	if (joint_and_sensor_data.find(hardware_component) == joint_and_sensor_data.end())
	{
		std::cout << "Invalid Hardware Component Name! Please refer to the Wiki for the valid names" << std::endl;
		return false;
	}
	else {
		std::string return_values;
		switch (joint_and_sensor_data[hardware_component].which())
		{
		case 0:
		{
			/// typeID = std::pair<float,float>
			std::pair<float, float> val = boost::get<std::pair<float, float>>(joint_and_sensor_data[hardware_component]);
			return_values = "(" + std::to_string(val.first) + ", " + std::to_string(val.second) + ")";
			break;
		}
		case 1:
		{
			/// typeID = float
			return_values = std::to_string(boost::get<float>(joint_and_sensor_data[hardware_component]));
			break;
		}
		case 2:
		{
			/// typeID = std::string
			return_values = boost::get<std::string>(joint_and_sensor_data[hardware_component]);
			break;
		}
		case 3:
		{
			/// typeID = std::unordered_map<std::string, Head::EyeLEDS>
			// I don't know how to deal with this yet...I'm thinking writing an unordered_map to_string method 
			// in the form: m = {{key1, val1,val2,val3},{key2, val1,val2,val3},...};
			return_values = "Eye LEDs map....";
			break;
		}
		
		}
	}
	return true;
}

bool NAOInterface::sync_pineapple()
{
	//ToDo: Post a semaphore and read value in shared memory using Boost.Interprocess
	return true;
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

void NAOInterface::initialize_hardware_map()
{
	/**
	* Initialize hashmap of everything....I really should make a common data structure
	* NOTE: This is probably going to be migrated to using luatables...or a database
	* This will (probably TM) work but it kind of sucks.
	**/
	// std::cout << "Initializing joint and sensor map. Test: " << head->get_actuators().first << " " << head->get_actuators().second << std::endl;
	// joint_and_sensor_data = {
	// 	{ "HEAD::ACTUATORS::POSITION", head->get_actuators() },
	// 	{ "HEAD::ACTUATORS::YAW", head->get_head_yaw()},
	// 	{ "HEAD::ACTUATORS::PITCH", head->get_head_pitch()},
	// 	{ "HEAD::LEDS", head->get_Eye_LEDS() },
	// 	{ "POTATO", "HEY! I am a pineapple"},
	// };
	// auto test_val = joint_and_sensor_data["HEAD::ACTUATORS::POSITION"];
	// if (joint_and_sensor_data.find("test_FAIL") == joint_and_sensor_data.end())
	// 	std::cout << "fail test passed. awesome" << std::endl;
	// else std::cout << "Fail test failed" << std::endl;

	// std::cout << "Hardware Map/Database/Cancer has been spawned. It's so easy to call values like this-> " << boost::get<std::pair<float, float>>(test_val).first << " " << boost::get<std::pair<float, float>>(test_val).second << std::endl;
	// std::cout << "Another Test: " << boost::get<std::string>(joint_and_sensor_data["POTATO"]) << std::endl;
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
		{"RARM::SHOULDER::ACTUATORS::ROLL", std::bind(&RArm::set_shoulder_roll, RightArm, _1)},
		{"RARM::SHOULDER::ACTUATORS::PITCH", std::bind(&RArm::set_shoulder_pitch, RightArm, _1)},
		{"RARM::ELBOW::ACTUATORS::ROLL", std::bind(&RArm::set_elbow_roll, RightArm, _1)},
		{"RARM::ELBOW::ACTUATORS::YAW", std::bind(&RArm::set_elbow_yaw, RightArm, _1)},
		{"RARM::WRIST::ACTUATORS::YAW", std::bind(&RArm::set_wrist_yaw, RightArm, _1)},
		//LArm
		{ "LARM::SHOULDER::ACTUATORS::ROLL", std::bind(&LArm::set_shoulder_roll, LeftArm, _1) },
		{ "LARM::SHOULDER::ACTUATORS::PITCH", std::bind(&LArm::set_shoulder_pitch, LeftArm, _1) },
		{ "LARM::ELBOW::ACTUATORS::ROLL", std::bind(&LArm::set_elbow_roll, LeftArm, _1) },
		{ "LARM::ELBOW::ACTUATORS::YAW", std::bind(&LArm::set_elbow_yaw, LeftArm, _1) },
		{ "LARM::WRIST::ACTUATORS::YAW", std::bind(&LArm::set_wrist_yaw, LeftArm, _1) },
		//RLeg
		{"RLEG::HIP::ACTUATORS::ROLL::POSITION", std::bind(&RLeg::set_hip_roll, RightLeg, _1)},
		{"RLEG::HIP::ACTUATORS::ROLL::STIFFNESS", std::bind(&RLeg::set_hip_roll_stiffness, RightLeg, _1)},

		{"RLEG::HIP::ACTUATORS::YAW::POSITION", std::bind(&RLeg::set_hip_yaw, RightLeg, _1)},
		{"RLEG::HIP::ACTUATORS::YAW::STIFFNESS", std::bind(&RLeg::set_hip_yaw_stiffness, RightLeg, _1) },

		{"RLEG::HIP::ACTUATORS::PITCH::POSITION", std::bind(&RLeg::set_hip_pitch, RightLeg, _1)},
		//{ "RLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_hip_pitch_stiffness, RightLeg, _1) },

		{"RLEG::KNEE::ACTUATORS::PITCH::POSITION", std::bind(&RLeg::set_knee_pitch, RightLeg, _1) },
		{ "RLEG::KNEE::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_knee_pitch_stiffness, RightLeg, _1) },

		{"RLEG::ANKLE::ACTUATORS::ROLL::POSITION", std::bind(&RLeg::set_ankle_roll, RightLeg, _1) },
		{ "RLEG::ANKLE::ACTUATORS::ROLL::STIFFNESS", std::bind(&RLeg::set_ankle_roll_stiffness, RightLeg, _1) },

		{"RLEG::ANKLE::ACTUATORS::PITCH::POSITION", std::bind(&RLeg::set_ankle_pitch, RightLeg, _1) },
		{ "RLEG::ANKLE::ACTUATORS::PITCH::STIFFNESS", std::bind(&RLeg::set_ankle_pitch_stiffness, RightLeg, _1) },
		//LLeg
		{ "LLEG::HIP::ACTUATORS::ROLL::POSITION", std::bind(&LLeg::set_hip_roll, LeftLeg, _1) },
		{ "LLEG::HIP::ACTUATORS::ROLL::STIFFNESS", std::bind(&LLeg::set_hip_roll_stiffness, LeftLeg, _1) },

		{ "LLEG::HIP::ACTUATORS::YAW::POSITION", std::bind(&LLeg::set_hip_yaw, LeftLeg, _1) },
		{ "LLEG::HIP::ACTUATORS::YAW::STIFFNESS", std::bind(&LLeg::set_hip_yaw_stiffness, LeftLeg, _1) },

		{ "LLEG::HIP::ACTUATORS::PITCH::POSITION", std::bind(&LLeg::set_hip_pitch, LeftLeg, _1) },
//		{ "LLEG::HIP::ACTUATORS::PITCH::STIFFNESS", std::bind(&LLeg::set_hip_pitch_stiffness, LeftLeg, _1) },

		{ "LLEG::KNEE::ACTUATORS::PITCH::POSITION", std::bind(&LLeg::set_knee_pitch, LeftLeg, _1) },
		{ "LLEG::KNEE::ACTUATORS::PITCH::STIFFNESS", std::bind(&LLeg::set_knee_pitch_stiffness, LeftLeg, _1) },

		{ "LLEG::ANKLE::ACTUATORS::ROLL::POSITION", std::bind(&LLeg::set_ankle_roll, LeftLeg, _1) },
		{ "LLEG::ANKLE::ACTUATORS::ROLL::STIFFNESS", std::bind(&LLeg::set_ankle_roll_stiffness, LeftLeg, _1) },

		{ "LLEG::ANKLE::ACTUATORS::PITCH::POSITION", std::bind(&LLeg::set_ankle_pitch, LeftLeg, _1) },
		{ "LLEG::ANKLE::ACTUATORS::PITCH::STIFFNESS", std::bind(&LLeg::set_ankle_pitch_stiffness, LeftLeg, _1) },
	};

	hardware_get_map = {
		{ "HEAD::ACTUATORS::YAW",  std::bind(&Head::get_head_yaw, head) },
		{ "HEAD::ACTUATORS::PITCH", std::bind(&Head::get_head_pitch, head) },
		//TODO: EyeLEDS, Stiffness, Body
		//RArm
		{ "RARM::SHOULDER::ACTUATORS::ROLL", std::bind(&Arm::get_shoulder_roll, RightArm) },
		{ "RARM::SHOULDER::ACTUATORS::PITCH", std::bind(&Arm::get_shoulder_pitch, RightArm) },
		{ "RARM::ELBOW::ACTUATORS::ROLL", std::bind(&Arm::get_elbow_roll, RightArm) },
		{ "RARM::ELBOW::ACTUATORS::YAW", std::bind(&Arm::get_elbow_yaw, RightArm) },
		{ "RARM::WRIST::ACTUATORS::YAW", std::bind(&Arm::get_wrist_yaw, RightArm) },
		//LArm
		{ "LARM::SHOULDER::ACTUATORS::ROLL", std::bind(&Arm::get_shoulder_roll, LeftArm) },
		{ "LARM::SHOULDER::ACTUATORS::PITCH", std::bind(&Arm::get_shoulder_pitch, LeftArm) },
		{ "LARM::ELBOW::ACTUATORS::ROLL", std::bind(&Arm::get_elbow_roll, LeftArm) },
		{ "LARM::ELBOW::ACTUATORS::YAW", std::bind(&Arm::get_elbow_yaw, LeftArm) },
		{ "LARM::WRIST::ACTUATORS::YAW", std::bind(&Arm::get_wrist_yaw, LeftArm) },
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

void NAOInterface::sanity_test(const std::string &foo)
{
	const std::string fooL = "HEAD::ACTUATORS::YAW";
	//auto t = hardware_set_functions[fooL];
	//t(1.21);
	set_hardware_value(fooL, 1.21);
	std::cout << "HEAD::ACTUATORS::YAW Value is now: " << get_hardware_value(fooL) << std::endl;
}

