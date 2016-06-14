/**
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
-- Created by David Chan 6/8/16
-- Modified by Mark Whalburg 7/14/18
-- Modified by Hugh Jass 7/14/18

/// FILE DESCRIPTION
Put a description of your class/header file here. 
*/

#ifndef __SOMEHEADERFILE.h__GUARD
#define __SOMEHEADERFILE.h__GUARD

#ifdef PINEAPPLE_VERSION_0.0.1
// Put version specific code here
#endif

#ifdef NAO_SDK_VERSION_2.1.4.13
// Put Nao SDK version specific code here
#endif

/// INCLUDES
#include <algorithm>
#include <someclass>


namespace HeaderTemplateNamespace {
	/***
	 * Doxygen style documentation for the class
	 */
	class MyClass : public AnotherClass {
		public:
			/*** Doxygen documentation for the function */
			void someFunction();
			/*** Doxygen documentation for the function */
			someclass someOtherFunction(SomeParam c);
			/*** Doxygen docs for another function */
			void doSomething(int x) {
				if (some condition) {
					// code here
				}
				for (int i = 0; i < x; i++) {
					// code here
				}
				while (x < 5) {
					// code here
				}
			}

		private:
			/*** Docs for the member variable */
			int anInt = 0;
	}

}

/**
 * Note - Please don't forget to use 'deprecated' labels if your
 * code is deprecated
 */

#endif /*__SOMEHEADERFILE.h__GUARD