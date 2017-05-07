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

Module loader class - responsible for loading individual modules and maintaining the list of active modules

*/

#ifndef _MLOADER_H_
#define _MLOADER_H_

// Includes
#include "common.h"
#include "modules/module.h"

#include <dlfcn.h>
#include <stdint.h>
#include <map>


/** The module loader class is a singleton class that is responsible for loading and maintaining a list of 
 *  necessary modules. 
 */
class ModuleLoader {
	public:
		/** Return the singleton instance of the modloader class
		 * @return ModuleLoader* Singleton instance
		 */
		static ModuleLoader* Instance();

		/**
		 * Load a module into the code
		 * @param  std::string 	The module configuration file that describes the module 
		 * @return uint16_t 	The module ID of the newly loaded module. 0x0 is a failure condition
		 */
		uint16_t LoadModule(std::string module_file);

		/**
		 * Unload a module
		 * @param	uint16_t 	The ID of the module to be unloaded
		 * @return 	bool 		If the module was successfully unloaded
		 */
		bool UnloadModule(uint16_t MODULE_ID);

		/**
		 * Get a module
		 * @param	std::string The name of the module	
		 * @return  Module* 	The requested module. Null if unavailabe.
		 */
		Module* GetModule(std::string module_name);

		/**
		 * Get a module
		 * @param	uint16_t	The ID of the module	
		 * @return  Module* 	The requested module. Null if unavailabe.
		 */
		Module* GetModule(uint16_t MODULE_ID);

		/**
		 * Get the ID of a module from the name
		 * @param	std::string	The name of the module
		 * @return  uint16_t	The ID of the module
		 */
		uint16_t GetModuleID(std::string module_name);

	private:
		/**
		 * Construct a module loader
		 */
		ModuleLoader();
		
		/** Instance varaible for the module loader */
		static ModuleLoader* instance;

		/** Map holding the name -> module ID mapping */
		std::map<std::string, uint16_t> name_map;

		/** Map holding the ID -> module* mapping */
		std::map<uint16_t, Module*> modules_loaded;
		std::map<uint16_t, void*> handles;
};

#endif /*_MLOADER_H_*/
