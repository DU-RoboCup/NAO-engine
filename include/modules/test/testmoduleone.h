/* TestModules header */

#ifndef _TESTMODULEONE_H_
#define _TESTMODULEONE_H_

#include <iostream>
#include "lib/luatables/luatables.h"
#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

class TestModuleOne : public Module {
	public:
		static TestModuleOne* Instance();	
		bool RunFrame();
		bool ProcessIntent(Intent &i);
		bool Install();
		bool Uninstall();
	private:
		static TestModuleOne* instance;

};


#endif
