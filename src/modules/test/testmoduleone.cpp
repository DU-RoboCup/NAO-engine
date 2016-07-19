
#include "include/modules/test/testmoduleone.h"


extern "C"
{
	Module* mkrfn() {
		return TestModuleOne::Instance();
	}
}

TestModuleOne* TestModuleOne::instance;

TestModuleOne* TestModuleOne::Instance() {
	if (UNLIKELY(TestModuleOne::instance == 0))
	{
		TestModuleOne::instance = new TestModuleOne();
	}
	return TestModuleOne::instance;
}

void TestModuleOne::Reconfigure(std::string config_file, uint16_t id) {
	this->ModuleID = id;

	LuaTable mconfig = LuaTable::fromFile(config_file.c_str());
	this->ModuleName = mconfig["name"].get<std::string>();
	this->ModuleFPS = (int)mconfig["rfps"].get<double>();
	this->ModulePriority = (int)mconfig["mprio"].get<double>();
	this->ModuleThread = (int)mconfig["mthr"].get<double>();
}

bool TestModuleOne::RunFrame() {

	LuaTable ltable = LuaTable::fromFile("../config/modules/myfile.lua");
	std::string name = ltable["name"].get<std::string>();
	double height = ltable["height"].get<double>();
	std::string country = ltable["address"]["country"].get<std::string> ();
	
	LOG_DEBUG << "Name: " << name << " Height: " << height << " Country: " << country; 

	return true;
}

bool TestModuleOne::ProcessIntent(Intent &i)
{
	// Ignore it.
}

bool TestModuleOne::Install() {
	LOG_DEBUG << "Installed Test Module One";
	return true;
}

bool TestModuleOne::Uninstall() {
	LOG_DEBUG << "Uninstalled Test Module One";
	return true;
}