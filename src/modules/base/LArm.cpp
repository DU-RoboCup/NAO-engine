#include "include/modules/base/LArm.h"

LArm::LArm()
{
	//Note: Angle limits are in radians
	wrist_bounds = std::make_pair<float, float>(-1.8238, 1.8238);
	shoulder_roll_bounds = std::make_pair<float, float>(-0.3142, 1.3265);
	shoulder_pitch_bounds = std::make_pair<float, float>(-2.0857, 2.0857);
	elbow_roll_bounds = std::make_pair<float, float>(-1.5446, -0.0349);
	elbow_yaw_bounds = std::make_pair<float, float>(-2.0857, 2.0857);
	std::cout << "Left Arm Initialized" << std::endl;
}


LArm::~LArm()
{
}

void LArm::set_shoulder(float roll, float pitch)
{
	if (checkBounds(shoulder_roll_bounds, roll)) ShoulderRoll = roll;
	if (checkBounds(shoulder_pitch_bounds, pitch)) ShoulderPitch = pitch;
}

void LArm::set_elbow(float roll, float yaw)
{
	if (checkBounds(elbow_roll_bounds, roll)) ElbowRoll = roll;
	if (checkBounds(elbow_yaw_bounds, yaw)) ElbowYaw = yaw;
}

void LArm::set_wrist(float yaw)
{
	if (checkBounds(wrist_bounds, yaw)) ElbowYaw = yaw;
}