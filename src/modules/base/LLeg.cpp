#include "include/base/LLeg.h"


typedef std::vector<std::pair<double, double>> BoundList; //Typing this is annoying and screenspace is limited

LLeg::LLeg()
{
	//Initialize Joints
	LHip = std::make_unique<Hip>();
	LKnee = std::make_unique<Knee>();
	LAnkle = std::make_unique<Ankle>();
	//Set Actuator Bounds
	//TODO: Double check pitch/yaw...they make it weird
	//TODO IMPORTANT: Apparently theres anticollision limits. I forgot to implement that
	LHip->HipRollBounds = std::make_pair<double, double>(-0.379472, 0.790477);
	LHip->HipPitchBounds = std::make_pair<double, double>(-1.535889, 0.484090);
	LHip->HipYawBounds = std::make_pair<double, double>(-1.145303, 0.740810);
	LKnee->KneePitchBounds = std::make_pair<double, double>(-0.092346, 2.112528);
	LAnkle->AnklePitchBounds = std::make_pair<double, double>(-1.189516, 0.922747);
	LAnkle->AnkleRollBounds = std::make_pair<double, double>(-0.397880, 0.769001);
	
	std::cout << "Left Leg initialized" << std::endl;
}


LLeg::~LLeg()
{
}

void LLeg::set_hip(double roll, double yaw, double pitch)
{
	if (checkBounds(LHip->HipRollBounds, roll) && checkBounds(LHip->HipYawBounds, yaw) && checkBounds(LHip->HipPitchBounds, pitch))
	{
		LHip->update_actuators(roll, yaw, pitch);
	}
	else std::cout << "Error: Invalid value was set for the LHip actuators. Did you remember to enter in Radians?" << std::endl;
}

void LLeg::set_knee(double pitch)
{
	if (checkBounds(LKnee->KneePitchBounds, pitch)) LKnee->update_actuators(pitch);
	else std::cout << "Error: Invalid value was set for LKnee actuators (only yaw). Did you remember to enter in Radians?" << std::endl;
}

void LLeg::set_ankle(double roll, double pitch)
{
	if (checkBounds(LAnkle->AnkleRollBounds, roll) && checkBounds(LAnkle->AnklePitchBounds, pitch)) {
		LAnkle->update_actuators(roll, pitch);
	} else std::cout << "Error: Invalid value was set for the LAnkle actuators. Did you remember to enter in Radians?" << std::endl;
}
BoundList LLeg::get_actuator_bounds()
{
	//Design mistakes were made..
	BoundList bounds = { 
		LHip->HipPitchBounds, LHip->HipRollBounds, LHip->HipYawBounds,
		LKnee->KneePitchBounds,
		LAnkle->AnkleRollBounds, LAnkle->AnklePitchBounds 
	};
	return bounds;
}


