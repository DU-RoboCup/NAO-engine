
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

void TestModuleOne::Reconfigure(uint16_t id, uint16_t fps, uint8_t priority, std::string name) {
	this->ModuleID = id;
	this->ModuleName = name;
	this->ModuleFPS = fps;
	this->ModulePriority = priority;
}

bool TestModuleOne::RunFrame() {
	std::cout << ".";
	return true;
}

bool TestModuleOne::Install() {
	std::cout << "Installed Test Module One" << std::endl;
	return true;
}

bool TestModuleOne::Uninstall() {
	std::cout << "Uninstalled Test Module One" << std::endl;
	return true;
}