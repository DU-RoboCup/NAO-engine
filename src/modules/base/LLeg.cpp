#include "include/modules/base/LLeg.h"


typedef std::vector<std::pair<float, float>> BoundList; //Typing this is annoying and screenspace is limited

LLeg::LLeg()
{
	//Initialize Joints
	// LHip = std::make_unique<Hip>();
	// LKnee = std::make_unique<Knee>();
	// LAnkle = std::make_unique<Ankle>();
	//Set Actuator Bounds
	//TODO: float check pitch/yaw...they make it weird
	//TODO IMPORTANT: Apparently theres anticollision limits. I forgot to implement that
	HipRollBounds = std::make_pair<float, float>(-0.379472, 0.790477);
	HipPitchBounds = std::make_pair<float, float>(-1.535889, 0.484090);
	HipYawBounds = std::make_pair<float, float>(-1.145303, 0.740810);
	KneePitchBounds = std::make_pair<float, float>(-0.092346, 2.112528);
	AnklePitchBounds = std::make_pair<float, float>(-1.189516, 0.922747);
	AnkleRollBounds = std::make_pair<float, float>(-0.397880, 0.769001);
	
	std::cout << "Left Leg initialized" << std::endl;
}


LLeg::~LLeg()
{
}

void LLeg::set_hip(float roll, float yaw, float pitch)
{
	if (checkBounds(HipRollBounds, roll) && checkBounds(HipYawBounds, yaw) && checkBounds(HipPitchBounds, pitch))
	{
		set_hip_roll(roll);
		set_hip_yaw(yaw);
		set_hip_pitch(pitch);
	}
	else std::cout << "Error: Invalid value was set for the LHip actuators. Did you remember to enter in Radians?" << std::endl;
}

void LLeg::set_knee(float pitch)
{
	if (checkBounds(KneePitchBounds, pitch)) set_knee_pitch(pitch);
	else std::cout << "Error: Invalid value was set for LKnee actuators (only yaw). Did you remember to enter in Radians?" << std::endl;
}

void LLeg::set_ankle(float roll, float pitch)
{
	if (checkBounds(AnkleRollBounds, roll) && checkBounds(AnklePitchBounds, pitch)) {
		set_ankle_roll(roll);
		set_ankle_pitch(pitch);
	} else std::cout << "Error: Invalid value was set for the LAnkle actuators. Did you remember to enter in Radians?" << std::endl;
}
