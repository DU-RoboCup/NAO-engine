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
#include <string>
#include <stdint>


class Module {
    public:
        /** Called every time that the module gets frame time */
        virtual bool RunFrame() = 0;

        /** Publish an API of calls to the bazzar, which will 
         * allow us to throw errors if other modules try to 
         * call something in it */
        virtual std::vector<std::string> PublishRequestAPI() = 0;

        /** Run when installing the module to the robot locally */
        virtual bool Install() = 0;

        /** Run when uninstalling the module to the robot locally */
        virtual bool Uninstall() = 0;

        /** Get the name of the module */
        const std::string GetName() const {return ModuleName;} 

        /** Get the ID of the module */
        const uint32_t GetID() const {return ModuleID;}

        /** Get the requested FPS for the module */
        const uint32_t GetFPS() const {return ModuleFPS;}

        /** Get the priority of the module */
        const uint8_t GetPriority() const {return ModulePriority;}

    protected:
        const std::string ModuleName; /** The module's name */
        const uint32_t ModuleID = 0x00000000; /** The module's unique ID number. See config for more info */
        const uint32_t ModuleFPS = 100; /** The FPS that the module requests to run on the robot */
        const uint8_t ModulePriority = 0; /** The priority that the module runs at. 8 is lowest. */
    private:
        // Nothing here! Modules are pretty public people.

}


#endif /*_MODULE_h_GUARD
