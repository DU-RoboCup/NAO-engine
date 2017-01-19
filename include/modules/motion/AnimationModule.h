/* Module managing animation and motion */

#ifndef AnimationModule
#define AnimationModule

#include <iostream>
#include "lib/luatables/luatables.h"
#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

class AnimationModule : public Module {
	public:
		static AnimationModule* Instance();

		void Reconfigure(std::string config_file, uint16_t id);
		bool RunFrame();
		bool ProcessIntent(Intent &i);
		bool Install();
		bool Uninstall();
	private:
		static AnimationModule* instance;

};


#endif // END #ifdef AnimationModule