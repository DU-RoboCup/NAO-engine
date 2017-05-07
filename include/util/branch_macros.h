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

Define a bunch of GCC specific macros
*/

#ifndef _BRANCH_MACROS_H_
#define _BRANCH_MACROS_H_

#define LIKELY(x)        __builtin_expect(!!(x), 1)
#define UNLIKELY(x)      __builtin_expect(!!(x), 0)

#endif /*_BRANCH_MACROS_H_*/
