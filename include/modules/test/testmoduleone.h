/* TestModules header */

#ifndef testmoduleone
#define testmoduleone

#include <iostream>
//#include "lib/luatables/luatables.h"
#include "include/modules/module.h"

class TestModuleOne : public Module {
	public:
		static TestModuleOne* Instance();

		void Reconfigure(uint16_t id, uint16_t fps, uint8_t priority, std::string name);
		
		bool RunFrame();
		bool Install();
		bool Uninstall();
	private:
		static TestModuleOne* instance;

};


#endif