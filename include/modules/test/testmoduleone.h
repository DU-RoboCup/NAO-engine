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

		void Reconfigure(std::string config_file, uint16_t id);
		
		bool RunFrame();
		bool ProcessIntent(Intent &i);
		bool Install();
		bool Uninstall();
	private:
		static TestModuleOne* instance;

};


#endif