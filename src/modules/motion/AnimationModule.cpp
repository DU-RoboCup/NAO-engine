
#include "include/modules/motion/AnimationModule.h"

// Registers the module with the dynamic loader
REGISTER_MODULE(AnimationModule)
INITIALIZE_SINGLETON_MODULE(AnimationModule)


void AbimationModule::Reconfigure(std::string config_file, uint16_t id) {
	this->ModuleID = id;

	LuaTable mconfig = LuaTable::fromFile(config_file.c_str());
	this->ModuleName = mconfig["name"].get<std::string>();
	this->ModuleFPS = (int)mconfig["rfps"].get<double>();
	this->ModulePriority = (int)mconfig["mprio"].get<double>();
	this->ModuleThread = (int)mconfig["mthr"].get<double>();
}

bool AnimationModule::RunFrame() {
	// Check if there's any new keyframes that we need to make it to

	// If there are
		// Process the keyframe, and figure out the joint data
		// Add it to the next keyframe priority queue
	
	// Solve motion to the current keyframe if no plan already exists
	// Solve any balance issues if the robot is becoming imbalanced
	
	// Read information back from the joints, compare to current motion plan
	// If on track, we're done. We may have to replan to the next key-frame
	
	// Update all joints to where they should be at the current time. 
	// Hopefully there's no lag

	return true;
}

bool AnimationModule::ProcessIntent(Intent &i)
{
	// Joint setting information will be processed. Gives a keyframe and time
}

bool AnimationModule::Install() {
	LOG_DEBUG << "Installed Animation Module";
	return true;
}

bool AnimationModule::Uninstall() {
	LOG_DEBUG << "Uninstalled Animation Module";
	return true;
}
