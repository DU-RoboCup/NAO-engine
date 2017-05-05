/* TestModules header */

#ifndef _plzworkmodule_H_
#define _plzworkmodule_H_

#include <iostream>
#include "lib/luatables/luatables.h"
#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

class plzworkmodule : public Module {
	public:
		static plzworkmodule* Instance();	
		bool RunFrame();
		bool ProcessIntent(Intent &i);
		bool Install();
		bool Uninstall();
	private:
		static plzworkmodule* instance;

};


#endif
