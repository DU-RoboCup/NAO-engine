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
-- Created by David Chan 7/7/16

// FILE DESCRIPTION

This file deals with the inter-module communication, in what I think 
is a relatively nice manner. It also holds a shared memory repository
for other modules

*/

#ifndef _INTENT_h_GUARD_
#define _INTENT_h_GUARD_

#include <string>


// Intents form the backbone of IMC, each intent is processed 
// by the modules, and so - any module can willingly ignore the
// pending intents. Each intent is string recognized and matched
struct Intent {
	Intent(std::string v) : value(v) {}
	std::string value;
};


#endif /*_INTENT_h_GUARD_*/