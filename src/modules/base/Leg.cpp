#include "include/modules/base/Leg.h"


auto check_stiffness = [](float val) {return true ? (val >= 0 && val <= 1) : false; };
typedef std::vector<std::pair<float, float>> BoundList; //Typing this is annoying and screenspace is limited

void Leg::set_hip_stiffness(float roll_stiffness, float yaw_stiffness, float pitch_stiffness)
{
	HipRoll_Stiffness = roll_stiffness;
	HipYaw_Stiffness = yaw_stiffness;
	HipPitch_Stiffness = pitch_stiffness;
}

void Leg::set_knee_stiffness(float pitch_stiffness)
{
	KneePitch_Stiffness = pitch_stiffness;
}

void Leg::set_ankle_stiffness(float roll_stiffness, float pitch_stiffness)
{
	AnkleRoll_Stiffness = roll_stiffness;
	AnklePitch_Stiffness = pitch_stiffness;
}
//void Leg::set_bounds();
void Leg::set_hip_roll_stiffness(float roll_stiffness) { if (check_stiffness(roll_stiffness)) HipRoll_Stiffness = roll_stiffness; }
void Leg::set_hip_yaw_stiffness(float yaw_stiffness) { if (check_stiffness(yaw_stiffness)) HipYaw_Stiffness = yaw_stiffness; }
//void Leg::set_hip_pitch_stiffness(float pitch_stiffness) { if (check_stiffness(pitch_stiffness)) HipPitch_Stiffness = pitch_stiffness; }
void Leg::set_knee_pitch_stiffness(float knee_stiffness) { if (check_stiffness(knee_stiffness)) KneePitch_Stiffness = knee_stiffness; }
void Leg::set_ankle_roll_stiffness(float roll_stiffness) { if (check_stiffness(roll_stiffness)) AnkleRoll_Stiffness = roll_stiffness; }
void Leg::set_ankle_pitch_stiffness(float pitch_stiffness) { if (check_stiffness(pitch_stiffness)) AnklePitch_Stiffness = pitch_stiffness; }

float Leg::get_hip_roll(){ return HipRoll_A; }
float Leg::get_hip_yaw() { return HipYaw_A; }
float Leg::get_hip_pitch() { return HipPitch_A; }
float Leg::get_knee_pitch() { return KneePitch_A; }
float Leg::get_ankle_pitch() { return AnklePitch_A; }
float Leg::get_ankle_roll() { return AnkleRoll_A; }
// BoundList get_actuator_bounds()
// {
// 	return BoundList {HipRollBounds, HipPitchBounds, HipYawBounds, KneePitchBounds, AnklePitchBounds, AnkleRollBounds};
// }

bool Leg::checkBounds(std::pair<float, float> bounds, float val)
{
	return true ? (val >= bounds.first && val <= bounds.second) : false;
}
void Leg::set_hip_roll(float roll){set_hip(roll, HipYaw_A, HipPitch_A);}
void Leg::set_hip_yaw(float yaw){set_hip(HipRoll_A, yaw, HipPitch_A);}
void Leg::set_hip_pitch(float pitch){set_hip(HipRoll_A, HipYaw_A, pitch);}
void Leg::set_knee_pitch(float pitch){set_knee(pitch);}
void Leg::set_ankle_roll(float roll){set_ankle(roll, AnklePitch_A);}
void Leg::set_ankle_pitch(float pitch){set_ankle(AnkleRoll_A, pitch);}
