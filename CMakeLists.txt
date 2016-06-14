cmake_minimum_required(VERSION 2.8)
project(NAO-engine)
include(/usr/share/cmake-2.8/Modules/ExternalProject.cmake)
find_package(qibuild)

# External Libraries
externalproject_add(luatables
    SOURCE_DIR            ${PROJECT_SOURCE_DIR}/lib/luatables   
    INSTALL_COMMAND       ""
 )



# Create a executable named n_a_o-engine
# with the source file: main.cpp
#qi_create_bin(NAO-engine "main.cpp")

# Add a simple test:
#qi_create_test(test_n_a_o-engine "test.cpp")
