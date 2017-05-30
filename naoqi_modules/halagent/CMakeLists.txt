cmake_minimum_required(VERSION 2.8)
project(hal_agent)

find_package(qibuild)
# Create a executable named hal_agent
# with the source file: main.cpp
# qi_create_bin(hal_agent "hal_access.cpp")

# qi_use_lib(hal_agent ALCOMMON ALPROXIES BOOST)
qi_create_lib(hal_experimental "hal_experimental.cpp" "hal_experimental.h"  SUBFOLDER naoqi)
qi_use_lib(hal_experimental ALCOMMON ALPROXIES BOOST)

# Add a simple test:
#qi_create_test(test_hal_agent "test.cpp")
