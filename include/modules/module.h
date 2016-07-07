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
-- Created by David Chan 6/20/16

// FILE DESCRIPTION

Module header file - presents the front interface for dynamic class loading
*/

#ifndef _MODULE_h_GUARD
#define _MODULE_h_GUARD

#include "common.h"
#include "memory/intent.h"
#include <string> 
#include <stdint.h>



class Module {
    public:
        
        /**
         * Reconfigure a module
         * @param   uint16_t    The ID of the module
         * @param   uint16_t    The FPS of the module
         * @param   uint8_t     The priority of the module
         * @param   std::string The Name of the module
         */
        virtual void Reconfigure(uint16_t id, uint16_t fps, uint8_t priority, std::string name) = 0;

        /**
         * Run the first time that the module is installed
         * @return bool If successful.
         */
        virtual bool Install() = 0;

        /**
         * Run the when the module is uninstalled
         * @return bool If successful.
         */
        virtual bool Uninstall() = 0;


        /**
         * Run every time that a module is given frame time
         * @return bool Whether or not there were errors
         */
        virtual bool RunFrame() = 0;


        /**
         * Called when an intent is submitted to a module. 
         * This function should really not do more than save the 
         * intent for later. If it is necessary to perform computation
         * then it can be done, but a warning will be thrown
         * @return bool If it was successful
         */
        virtual bool ProcessIntent(Intent &intent) = 0;


        /** Get the name of the module */
        const std::string GetName() const {return ModuleName;} 

        /** Get the ID of the module */
        const uint16_t GetID() const {return ModuleID;}

        /** Get the requested FPS for the module */
        const uint16_t GetFPS() const {return ModuleFPS;}

        /** Get the priority of the module */
        const uint8_t GetPriority() const {return ModulePriority;}

    protected:
        std::string ModuleName; /** The module's name */
        uint16_t ModuleID; /** The module's unique ID number. See config for more info */
        uint16_t ModuleFPS; /** The FPS that the module requests to run on the robot */
        uint8_t ModulePriority; /** The priority that the module runs at. 8 is lowest. */
    private:

};


#endif /*_MODULE_h_GUARD */