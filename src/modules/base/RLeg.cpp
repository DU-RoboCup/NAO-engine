#include "include/base/RLeg.h"
typedef std::vector<std::pair<double, double>> BoundList; //Typing this is annoying and screenspace is limited


RLeg::RLeg()
{
	//Initialize Joints
	RHip = std::make_unique<Hip>();
	RKnee = std::make_unique<Knee>();
	RAnkle = std::make_unique<Ankle>();

	//Set Actuator Bounds
	//TODO: Double check pitch/yaw...they make it weird
	//TODO IMPORTANT: Apparently theres anticollision limits. I forgot to implement that
	RHip->HipRollBounds = std::make_pair<double, double>(-0.790477, 0.379472);
	RHip->HipPitchBounds = std::make_pair<double, double>(-1.535889, 0.484090);
	RHip->HipYawBounds = std::make_pair<double, double>(-1.145303, 0.740810);
	RKnee->KneePitchBounds = std::make_pair<double, double>(-0.103083, 2.120198);
	RAnkle->AnklePitchBounds = std::make_pair<double, double>(-1.186448, 0.932056);
	RAnkle->AnkleRollBounds = std::make_pair<double, double>(-0.768992, 0.397935);
	std::cout << "Right Leg initialized" << std::endl;
}


RLeg::~RLeg()
{
}


void RLeg::set_hip(double roll, double yaw, double pitch)
{
	if (checkBounds(RHip->HipRollBounds, roll) && checkBounds(RHip->HipYawBounds, yaw) && checkBounds(RHip->HipPitchBounds, pitch))
		RHip->update_actuators(roll, yaw, pitch);
	else std::cout << "Error: Invalid value was set for the RHip actuators. Did you remember to enter in Radians?" << std::endl;
}
void RLeg::set_knee(double pitch)
{
	if (checkBounds(RKnee->KneePitchBounds, pitch)) RKnee->update_actuators(pitch);
	else std::cout << "Error: Invalid value was set for RKnee actuators (only yaw). Did you remember to enter in Radians?" << std::endl;
}

void RLeg::set_ankle(double roll, double pitch)
{
	if (checkBounds(RAnkle->AnkleRollBounds, roll) && checkBounds(RAnkle->AnklePitchBounds, pitch))
		RAnkle->update_actuators(roll, pitch);
	else std::cout << "Error: Invalid value was set for the RAnkle actuators. Did you remember to enter in Radians?" << std::endl;
}

BoundList RLeg::get_actuator_bounds()
{
	//Design mistakes were made..
	BoundList bounds  = { RHip->HipPitchBounds, RHip->HipRollBounds, RHip->HipYawBounds, 
		RKnee->KneePitchBounds, 
		RAnkle->AnkleRollBounds, RAnkle->AnklePitchBounds };
	return bounds;
}
