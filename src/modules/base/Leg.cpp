#include "include/base/Leg.h"


auto check_stiffness = [](auto val) {return true ? (val >= 0 && val <= 1) : false; };
typedef std::vector<std::pair<double, double>> BoundList; //Typing this is annoying and screenspace is limited

void Leg::set_hip_stiffness(double roll_stiffness, double yaw_stiffness, double pitch_stiffness)
{
	HipRoll_Stiffness = roll_stiffness;
	HipYaw_Stiffness = yaw_stiffness;
	HipPitch_Stiffness = pitch_stiffness;
}

void Leg::set_knee_stiffness(double pitch_stiffness)
{
	KneePitch_Stiffness = pitch_stiffness;
}

void Leg::set_ankle_stiffness(double roll_stiffness, double pitch_stiffness)
{
	AnkleRoll_Stiffness = roll_stiffness;
	AnklePitch_Stiffness = pitch_stiffness;
}

void Leg::set_hip_roll_stiffness(double roll_stiffness) { if (check_stiffness(roll_stiffness)) HipRoll_Stiffness = roll_stiffness; }
void Leg::set_hip_yaw_stiffness(double yaw_stiffness) { if (check_stiffness(yaw_stiffness)) HipYaw_Stiffness = yaw_stiffness; }
void Leg::set_hip_pitch_stiffness(double pitch_stiffness) { if (check_stiffness(pitch_stiffness)) HipPitch_Stiffness = HipPitch_Stiffness; }
void Leg::set_knee_pitch_stiffness(double knee_stiffness) { if (check_stiffness(knee_stiffness)) KneePitch_Stiffness = knee_stiffness; }
void Leg::set_ankle_roll_stiffness(double roll_stiffness) { if (check_stiffness(roll_stiffness)) AnkleRoll_Stiffness = roll_stiffness; }
void Leg::set_ankle_pitch_stiffness(double pitch_stiffness) { if (check_stiffness(pitch_stiffness)) AnklePitch_Stiffness = pitch_stiffness; }

double Leg::get_hip_roll(){ return HipRoll_A; }
double Leg::get_hip_yaw() { return HipYaw_A; }
double Leg::get_hip_pitch() { return HipPitch_A; }
double Leg::get_knee_pitch() { return KneePitch_A; }
double Leg::get_ankle_pitch() { return AnklePitch_A; }
double Leg::get_ankle_roll() { return AnkleRoll_A; }

bool Leg::checkBounds(std::pair<double, double> bounds, double val)
{
	return true ? (val >= bounds.first && val <= bounds.second) : false;
}


Hip::Hip()
{
	std::cout << "A hip was initialized" << std::endl;
}

Hip::Hip(std::pair<double, double> &hrBounds, std::pair<double, double> &hyBounds, std::pair<double, double> &hpBounds)
{
	HipRollBounds = hrBounds;
	HipYawBounds = hyBounds;
	HipPitchBounds = hpBounds;
}

Hip::~Hip()
{
}

void Hip::set_stiffness(double roll_stiffness, double yaw_stiffness, double pitch_stiffness)
{
	if (check_stiffness(roll_stiffness) && check_stiffness(yaw_stiffness) && check_stiffness(pitch_stiffness)) {
		HipRoll_Stiffness = roll_stiffness;
		HipYaw_Stiffness = yaw_stiffness;
		HipPitch_Stiffness = pitch_stiffness;
	}
	else {
		std::cout << "Invalid Hip Actuator Stiffness Value (remember its 0-100%, or 0 < x < 1)" << std::endl;
	}
}

void Hip::update_actuators(double roll, double yaw, double pitch)
{
	HipRoll_A = roll;
	HipYaw_A = yaw;
	HipPitch_A = pitch;
}
std::vector<double> Hip::get_actuator_values()
{
	return std::vector<double>() = { HipRoll_A, HipYaw_A, HipPitch_A };
}

std::vector<double> Hip::get_sensor_values()
{
	return std::vector<double>() = { HipRoll_S, HipYaw_S, HipPitch_S, Temperature };
}

//bool Hip::checkStiffness(double val)
//{
//	return true ? (val >= 0 && val <= 1) : false;
//}

void Knee::set_stiffness(double pitch_stiffness)
{
	if (check_stiffness(pitch_stiffness)) KneePitch_Stiffness = pitch_stiffness;
	else std::cout << "Invalid Knee Actuator Stiffness Value (remember its 0-100%, or 0 < x < 1)" << std::endl;
}

void Knee::update_actuators(double pitch)
{
	KneePitch_A = pitch;
}

double Knee::get_actuator_values()
{
	return KneePitch_A;
}



std::vector<double> Knee::get_sensor_values()
{
	return std::vector<double>() = { KneePitch_S, Temperature };
}

void Ankle::set_stiffness(double roll_stiffness, double pitch_stiffness)
{
	if (check_stiffness(roll_stiffness) && check_stiffness(pitch_stiffness)) {
		AnkleRoll_Stiffness = roll_stiffness;
		AnklePitch_Stiffness = pitch_stiffness;
	}
	else {
		std::cout << "Invalid Ankle Actuator Stiffness Value (remember its 0-100%, or 0 < x < 1)" << std::endl;
	}
}

void Ankle::update_actuators(double &roll_stiffness, double &pitch_stiffness)
{
	AnkleRoll_A = roll_stiffness;
	AnklePitch_A = pitch_stiffness;
}

std::vector<double> Ankle::get_actuator_values(){ return std::vector<double>() = { AnkleRoll_A, AnkleRoll_S }; }
std::vector<double> Ankle::get_sensor_values(){return std::vector<double>() = { AnkleRoll_S, AnklePitch_S, Temperature };}
void Leg::set_hip_roll(double roll){set_hip(roll, HipYaw_A, HipPitch_A);}
void Leg::set_hip_yaw(double yaw){set_hip(HipRoll_A, yaw, HipPitch_A);}
void Leg::set_hip_pitch(double pitch){set_hip(HipRoll_A, HipYaw_A, pitch);}
void Leg::set_knee_pitch(double pitch){set_knee(pitch);}
void Leg::set_ankle_roll(double roll){set_ankle(roll, AnklePitch_A);}
void Leg::set_ankle_pitch(double pitch){set_ankle(AnkleRoll_A, pitch);}

