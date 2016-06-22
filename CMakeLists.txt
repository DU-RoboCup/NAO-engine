Cmake_minimum_required(VERSION 2.8)
project(NAO-engine)
include(/usr/share/cmake-2.8/Modules/ExternalProject.cmake)

# Include the qibuild system
find_package(qibuild)
# Include boost
find_package(Boost 1.43 COMPONENTS log REQUIRED)

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

# Setup some additional import locations
set(INC_DIRECTORIES "${CMAKE_SOURCE_DIR}/include")
include_directories(${INC_DIRECTORIES} ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

# Set version information
add_definitions(-DPINEAPPLE_VERSION_0_0_1 -DNAO_SDK_VERSION_2_1_4_13)


# Main build system
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

	add_subdirectory(src)

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

	add_subdirectory(src)

endif()
	
#export compilation flags for YouCompleteMe
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Setup docs

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based Pineapple documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR/docs/}
        COMMENT "Generating Pineapple documentation with Doxygen"
        VERBATIM)
endif()
