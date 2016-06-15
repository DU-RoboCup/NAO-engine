Cmake_minimum_required(VERSION 2.8)
project(NAO-engine)
include(/usr/share/cmake-2.8/Modules/ExternalProject.cmake)

# Include the qibuild system
find_package(qibuild)

# Define the toolchain location
set(CROSS_QI_TOOLCHAIN "/home/vagrant/.local/share/qi/toolchains/cross-atom/toolchain-cross-atom.cmake")
set(SDK_QI_TOOLCHAIN "/home/vagrant/.local/share/qi/toolchains/linux64/toolchain-linux64.cmake")

# Define different compilation routes if cross-atom toolchain
if(${CROSS_QI_TOOLCHAIN} STREQUAL ${CMAKE_TOOLCHAIN_FILE})
	set(IS_CROSS "TRUE")
	set(IS_SDK "FALSE")
else()
	set(IS_CROSS "FALSE")
	set(IS_SDK "TRUE")
endif()

if (${IS_CROSS} STREQUAL "TRUE")
	
	message("Building with cross-atom compilation...")
	
	# Build the luatables library
	qi_create_lib(luatables lib/luatables/luatables.h lib/luatables/luatables.cc)
	qi_use_lib(luatables lua)
	qi_stage_lib(luatables)

	# Build the test for luatables
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	qi_create_bin(test_luatables "test/test_luatables.cpp")
	qi_use_lib(test_luatables luatables lua)

else()

	message("Building with SDK compilation...")

	# Build the luatables library
	qi_create_lib(luatables lib/luatables/luatables.h lib/luatables/luatables.cc)
	qi_use_lib(luatables lua)
	qi_stage_lib(luatables)

	# Built the test for luatables
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	qi_create_bin(test_luatables "test/test_luatables.cpp")
	qi_use_lib(test_luatables luatables lua)
endif()
	

