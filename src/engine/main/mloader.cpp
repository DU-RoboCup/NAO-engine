/*
Copyright (c) 2016 "University of Denver"

This file is part of Pineapple.

Pineapple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

VERSION HISTORY
-- Created by David Chan 6/30/16

// FILE DESCRIPTION
	Ccode for loading modules into the main code

*/


// Includes
#include "engine/main/mloader.h"

ModuleLoader* ModuleLoader::instance;

/**
 * Get the instance of the module loader, if there is not one, construct one
 */
ModuleLoader* ModuleLoader::Instance() {
	if (UNLIKELY(!ModuleLoader::instance)) {
		instance = new ModuleLoader();
	}
	return instance;
}

/** 
 * Load a module from a .module file
 */
uint16_t ModuleLoader::LoadModule(std::string module_file) {
	// Load the module file
	LuaTable mconfig = LuaTable::fromFile(module_file.c_str());

	// Construct the module
	void* hndl = dlopen(mconfig["liblocation"].get<std::string>().c_str(), RTLD_LAZY);
	if (!hndl) {
		// There's an error loading the module
		LOG_WARNING << "Error Loading Module DLOpen Null..." << std::endl;
		LOG_WARNING << dlerror() << std::endl;
		return 0x0;
	}
	void* inst = dlsym(hndl, "mkrfn");
	Module* loaded_module = reinterpret_cast<Module* (*)()>(inst)();

	// Get the first ID and store it there
	uint16_t id = 1;
	for (; id < 0xFF; id++) {
		if (!modules_loaded[id]){
			modules_loaded[id] = loaded_module;
			handles[id] = hndl;
			break;
		}
	}
	
	// Check for errors in space
	if (id == 0xFF) {
		
		// There's no space for the loaded module
		delete loaded_module; // Call the deconstructor
		dlclose(hndl); // Close the library

		return 0x0;
	}

	// Reconfigure the module
	loaded_module->Reconfigure(module_file, id);

	// Keep the string map updated
	name_map[loaded_module->GetName()] = 	id;

	// Install the module
	loaded_module->Install();

	// Return the ID of the module 
	return id;
}

/**
 * Unload the module given by a particular ID
 */
bool ModuleLoader::UnloadModule(uint16_t MODULE_ID) {
	// Get the module pointer
	Module* lmodule = modules_loaded[MODULE_ID];

	// If there's not module there, then we ignore it.
	if (!lmodule)
		return false;

	// Get the module name
	std::string mname = modules_loaded[MODULE_ID]->GetName();

	// Get the module handle
	void* hndl = handles[MODULE_ID];

	// Uninstall the module
	lmodule->Uninstall();

	// Clean up the module
	delete lmodule;
	// Close the library
	dlclose(hndl);

	// Remove the references
	modules_loaded[MODULE_ID] = 0;
	handles[MODULE_ID] = 0;
	name_map[mname] = 0;

	return true;
}


// Get modules from the module loader
Module* ModuleLoader::GetModule(std::string module_name) {
	return modules_loaded[name_map[module_name]];
}

Module* ModuleLoader::GetModule(uint16_t MODULE_ID) {
	return modules_loaded[MODULE_ID];
}

// Get the ID of a module by name
uint16_t ModuleLoader::GetModuleID(std::string module_name) {
	return name_map[module_name];
}

// Construct a module loader
ModuleLoader::ModuleLoader() {
	modules_loaded[0] = 0;
	name_map[""] = 0;
	handles[0] = 0;
}



