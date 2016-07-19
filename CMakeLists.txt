Cmake_minimum_required(VERSION 2.8)

# Define the toolchain location
option(USE_CROSS "Whether or not to build using the cross-toolchain" "ON")

# Define different compilation routes if cross-atom toolchain
if(USE_CROSS)
    set(IS_CROSS "TRUE")
    set(IS_SDK "FALSE")
    SET(CMAKE_C_COMPILER "/var/persistent/cross-atom/bin/i686-nptl-linux-gnu-gcc")
    SET(CMAKE_CXX_COMPILER "/var/persistent/cross-atom/bin/i686-nptl-linux-gnu-g++")
else()
    set(IS_CROSS "FALSE")
    set(IS_SDK "TRUE")
    SET(CMAKE_C_COMPILER "/usr/bin/gcc")
    SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

project(NAO-engine)
include(/usr/share/cmake-2.8/Modules/ExternalProject.cmake)

# Setup the cross compiler 
set(CROSS_RPATH_LOCATION "/var/persistent/cross-atom/lib:/var/persistent/cross-atom/i686-nptl-linux-gnu/lib:/var/persistent/cross-atom/lib/gcc/i686-nptl-linux-gnu/5.2.0")
set(CROSS_DYN_LINKER_LOCATION "/var/persistent/cross-atom/i686-nptl-linux-gnu/lib/ld-linux.so.2")
set(CROSS_INC_PATH "/var/persistent/cross-atom/include")
set(CROSS_SYS_INC_PATH "/var/persistent/cross-atom/i686-nptl-linux-gnu/include/")

# Add the library location for boost
set(CROSS_LIB_LOCATION "/var/persistent/cross-atom/lib")
set(CROSS_SYS_LIB_LOCATION "/var/persistent/cross-atom/i686-nptl-linux-gnu/lib")

# Add header location for main project
set(INC_DIRECTORIES "${CMAKE_SOURCE_DIR}/include")
include_directories(${INC_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR})

# Set version information
add_definitions(-DPINEAPPLE_VERSION_0_0_2 -DNAO_SDK_VERSION_2_1_4_13 -D_GLIBCXX_USE_NANOSLEEP)

# Main build system
if (${IS_CROSS} STREQUAL "TRUE")

	message("Building with cross-atom compilation...")

    message("Configuring compiler flags...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -g -march=atom -mtune=atom -Wl,-rpath=${CROSS_RPATH_LOCATION} -Wl,-dynamic-linker=${CROSS_DYN_LINKER_LOCATION} -Wl,-L${CROSS_LIB_LOCATION} -Wno-deprecated-declarations")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -march=atom -mtune=atom -Wl,-rpath=${CROSS_RPATH_LOCATION} -Wl,-dynamic-linker=${CROSS_DYN_LINKER_LOCATION} -Wl,-L${CROSS_LIB_LOCATION}")

    message("Configuring cmake options...")
    set(CMAKE_LIBRARY_PATH "${CROSS_SYS_LIB_LOCATION}:${CROSS_LIB_LOCATION}:${CMAKE_LIBRARY_PATH}")

    message("Adding header folders...")
    include_directories(${CROSS_INC_PATH} ${CROSS_SYS_INC_PATH})

    message("Adding library folders...")
    link_directories(${CROSS_LIB_LOCATION})

    message("Creating install location...")
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build-cross/bin" CACHE INTERNAL "exoutpath" FORCE)
    set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build-cross/lib" CACHE INTERNAL "liboutpath" FORCE)

	# Build the luatables library
	add_library(luatables SHARED lib/luatables/luatables.h lib/luatables/luatables.cc)
    target_link_libraries(luatables lua)

    add_subdirectory(src)

    # Install configuration files
    file( COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_SOURCE_DIR}/build-cross)  

else()
    
    message("Building with SDK compilation...")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -march=native -Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -g -march=native -Wno-deprecated-declarations")

    # Include boost
    find_package(Boost 1.43 COMPONENTS log log_setup system thread date_time REQUIRED)
    if(Boost_FOUND)
        message(STATUS "Boost found!")
    endif()

    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})

    # Include Lua
    find_package(Lua51)
    if(LUA_FOUND)
        message(STATUS "Lua found!")
    endif()

    include_directories(${LUA_INCLUDE_DIR})
    link_directories(${LUA_LIBRARIES})

    message("Creating install location...")
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build-local/bin" CACHE INTERNAL "exoutpath" FORCE)
    set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build-local/lib" CACHE INTERNAL "liboutpath" FORCE)

	# Build the luatables library
	add_library(luatables SHARED lib/luatables/luatables.h lib/luatables/luatables.cc)
    target_link_libraries(luatables lua5.1)
    
    add_subdirectory(src)  

    # Install configuration files
    file( COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_SOURCE_DIR}/build-local)  

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

