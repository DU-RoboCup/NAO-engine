#include "include/modules/base/Head.h"

Head::Head()
{
	//Set Initial Yaw and Pitch range: Values from aldabaran docs
	YawRange = std::make_pair(-2.0857, 2.0857);
	PitchRange = std::make_pair(-0.6720, 0.5149);

	//Set map of occurences where Pitch range is limited to Yaw according to http://doc.aldebaran.com/2-1/family/robots/joints_robot.html#robot-joints-v4-head-joints
	//Key = yaw_value
	//Value = std::pair<float,float> limited_pitch_value
	pitch_limits_map = {
		{ -2.086017, { -0.449073, 0.330041 }},
		{ -1.526988, { -0.330041, 0.200015 }},
		{-0.903033 , {-0.479965,	0.330041}},
		{-0.756077, { -0.548033,	0.37001 }},
		{-0.486074, {-0.671951,	0.422021} },
		{0, {-0.671951,	0.515047}},
		{0.486074, {-0.671951,	0.422021}},
		{0.756077, {-0.548033,	0.37001}},
		{0.903033, {-0.479965,	0.330041}},
		{1.089958, {-0.430049,	0.300022}},
		{1.526988, {-0.330041,	0.200015}},
		{2.086017, {-0.449073,	0.330041}}

	}; 
	head_pitch = 1.0;
	head_yaw = 1.0;

	//Initialize eye leds
	eye_led_map = {
		{"R0", EyeLED()}, {"R1", EyeLED()}, { "R2", EyeLED() }, { "R3", EyeLED() },
		{ "R4", EyeLED() }, { "R5", EyeLED() }, { "R6", EyeLED() }, { "R7", EyeLED() },

		{ "L0", EyeLED() }, { "L1", EyeLED() }, { "L2", EyeLED() }, { "L3", EyeLED() }, 
		{ "L4", EyeLED() }, { "L5", EyeLED() }, { "L6", EyeLED() }, { "L7", EyeLED() }
	};
	std::cout << "Head Initialized" << std::endl;
}

Head::~Head()
{
}

bool Head::checkBounds(std::pair<float, float> bounds, float val)
{
	return true ? (val > bounds.first && val < bounds.second) : false;
}

void Head::set_head_actuators(float yaw, float pitch)
{
	//First check that the pitch and yaw are within the actuator bounds
	if (checkBounds(YawRange, yaw) && checkBounds(PitchRange, pitch))
	{
		//Now we deal with the pitch limitations
		auto yaw_key_search = pitch_limits_map.find(yaw);
		if (yaw_key_search != pitch_limits_map.end())
		{
			std::cout << "Head Position Set: Yaw value has a pitch associated with it... ";
			//yaw value has pitch limit associated to it
			if (checkBounds(yaw_key_search->second, pitch)) {
				std::cout << "Invalid Pitch Value!" << std::endl;
			}
			else {
				std::cout << "Valid Pitch Value." << std::endl;
				head_yaw = yaw;
				head_pitch = pitch;
			}
		}
		else {
			head_yaw = yaw;
			head_pitch = pitch;
		}
	}
	else std::cout << "Head Position Set: Invalid Yaw or Pitch." << std::endl;
	//Pitch Motion Range is limited according to Yaw Value


}

void Head::set_yaw(float yaw)
{
	set_head_actuators(yaw, head_pitch); ///< Lazy way to not repeat bounds check code
}
void Head::set_pitch(float pitch)
{
	set_head_actuators(head_yaw, pitch); ///< Lazy way to not repeat bounds check code
}
std::pair<float, float> Head::get_actuators()
{
	return std::pair<float, float>(head_yaw, head_pitch);
}
float Head::get_head_yaw()
{
	return head_yaw;
}
float Head::get_head_pitch()
{
	return head_pitch;
}
std::unordered_map<std::string, Head::EyeLED> Head::get_Eye_LEDS()
{
	return eye_led_map;
}
Head::EyeLED Head::get_Eye_LEDS(std::string & eye_and_LED_number)
{
	try {
		auto led_val = eye_led_map.at(eye_and_LED_number);
		return led_val;
	}
	catch (const std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
}