#include "include/modules/base/RArm.h"



RArm::RArm() 
{
	wrist_bounds = std::make_pair<float, float>(-1.8238, 1.8238);
	shoulder_roll_bounds = std::make_pair<float, float>(-1.3265, 0.3142);
	shoulder_pitch_bounds = std::make_pair<float, float>(-2.0857, 2.0857);
	elbow_roll_bounds = std::make_pair<float, float>(0.0349, 1.5446);
	elbow_yaw_bounds = std::make_pair<float, float>(-2.0857, 2.0857);
	
	set_shoulder(0.3, 0.5);
	set_elbow(0.2, 0.2);
	set_wrist(1.2);

	std::cout << "Right Arm Initialized: " << print_arm_status() << std::endl;
}


RArm::~RArm()
{
}

void RArm::set_shoulder(float roll, float pitch)
{
	if (checkBounds(shoulder_roll_bounds, roll)) ShoulderRoll = roll;
	if (checkBounds(shoulder_pitch_bounds, pitch)) ShoulderPitch = pitch;
}


void RArm::set_elbow(float roll, float yaw)
{
	if (checkBounds(elbow_roll_bounds, roll)) ElbowRoll = roll;
	if (checkBounds(elbow_yaw_bounds, yaw)) ElbowYaw = yaw;
}

void RArm::set_wrist(float yaw)
{
	if (checkBounds(wrist_bounds, yaw)) ElbowYaw = yaw;
	else std::cout << "Invalid wrist yaw value" << std::endl;
}

std::string RArm::test_message()
{
	return hellomsg;
}

