
#include "include/modules/test/plzworkmodule.h"

// Registers the module with the dynamic loader
REGISTER_MODULE(plzworkmodule)

plzworkmodule* plzworkmodule::instance;
plzworkmodule* plzworkmodule::Instance() {
	if (UNLIKELY(plzworkmodule::instance == 0))
	{
		plzworkmodule::instance = new plzworkmodule();
	}
	return plzworkmodule::instance;
}

bool plzworkmodule::RunFrame() {

	LuaTable ltable = LuaTable::fromFile("../config/modules/myfile.lua");
	std::string name = ltable["name"].get<std::string>();
	double height = ltable["height"].get<double>();
	std::string country = ltable["address"]["country"].get<std::string> ();
	
//	LOG_DEBUG << "Name: " << name << " Height: " << height << " Country: " << country; 

	return true;
}

bool plzworkmodule::ProcessIntent(Intent &i)
{
	// Ignore it.
}

bool plzworkmodule::Install() {
	LOG_DEBUG << "Installed Test Module One";
	return true;
}

bool plzworkmodule::Uninstall() {
	LOG_DEBUG << "Uninstalled Test Module One";
	return true;
}
