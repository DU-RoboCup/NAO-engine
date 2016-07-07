/* TestModules header */

#ifndef testmoduleone
#define testmoduleone

#include <iostream>
#include "lib/luatables/luatables.h"
#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

class TestModuleOne : public Module {
	public:
		static TestModuleOne* Instance();

		void Reconfigure(uint16_t id, uint16_t fps, uint8_t priority, std::string name);
		
		bool RunFrame();
		bool ProcessIntent(Intent &i);
		bool Install();
		bool Uninstall();
	private:
		static TestModuleOne* instance;

};


#endif