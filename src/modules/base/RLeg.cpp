#include "include/modules/base/RLeg.h"
typedef std::vector<std::pair<float, float>> BoundList; //Typing this is annoying and screenspace is limited


RLeg::RLeg()
{
	//Initialize Joints
	// RHip = std::make_unique<Hip>();
	// RKnee = std::make_unique<Knee>();
	// RAnkle = std::make_unique<Ankle>();

	//Set Actuator Bounds
	//TODO: float check pitch/yaw...they make it weird
	//TODO: IMPORTANT: Apparently theres anticollision limits. I forgot to implement that
	HipRollBounds = std::make_pair<float, float>(-0.790477, 0.379472);
	HipPitchBounds = std::make_pair<float, float>(-1.535889, 0.484090);
	HipYawBounds = std::make_pair<float, float>(-1.145303, 0.740810);
	KneePitchBounds = std::make_pair<float, float>(-0.103083, 2.120198);
	AnklePitchBounds = std::make_pair<float, float>(-1.186448, 0.932056);
	AnkleRollBounds = std::make_pair<float, float>(-0.768992, 0.397935);
	std::cout << "Right Leg initialized" << std::endl;
}


RLeg::~RLeg()
{
}


void RLeg::set_hip(float roll, float yaw, float pitch)
{
	if (checkBounds(HipRollBounds, roll) && checkBounds(HipYawBounds, yaw) && checkBounds(HipPitchBounds, pitch))
	{
		set_hip_roll(roll);
		set_hip_yaw(yaw);
		set_hip_pitch(pitch);
	}
	else std::cout << "Error: Invalid value was set for the RHip actuators. Did you remember to enter in Radians?" << std::endl;
}
void RLeg::set_knee(float pitch)
{
	if (checkBounds(KneePitchBounds, pitch)) set_knee_pitch(pitch);
	else std::cout << "Error: Invalid value was set for RKnee actuators (only yaw). Did you remember to enter in Radians?" << std::endl;
}

void RLeg::set_ankle(float roll, float pitch)
{
	if (checkBounds(AnkleRollBounds, roll) && checkBounds(AnklePitchBounds, pitch)){
		set_ankle_roll(roll);
		set_ankle_pitch(pitch);
	}
	else std::cout << "Error: Invalid value was set for the RAnkle actuators. Did you remember to enter in Radians?" << std::endl;
}

