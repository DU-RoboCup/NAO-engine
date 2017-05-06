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

This file contains a number of includes which are necessary for most of the projects we are developing. It allows us to include with only one step.
*/

#ifndef _COMMON_H_
#define _COMMON_H_

// Necessary headers for luatables
#include "lib/luatables/luatables.h"
#include <string>

// Macros
#include "util/branch_macros.h"
#include "modules/module_macros.h"
//#include "util/compile_macros.h"

// Constants
#include "util/constants.h"

// Memory
#include "memory/memory.h"

// Debugging
#include "debug/debugging.h"

#endif /*_COMMON_H_ */
