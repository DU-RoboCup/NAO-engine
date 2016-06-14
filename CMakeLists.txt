cmake_minimum_required(VERSION 2.8)
project(NAO-engine)
include(/usr/share/cmake-2.8/Modules/ExternalProject.cmake)
include(/usr/share/cmake-2.8/Modules/FindLua51.cmake)

find_package(qibuild)
find_package(Lua51)


# External Libraries
externalproject_add(luatables
    SOURCE_DIR            ${PROJECT_SOURCE_DIR}/lib/luatables   
    INSTALL_COMMAND       ""
 )

# Include directories
include_directories("${PROJECT_SOURCE_DIR}" "${LUA_INCLUDE_DIR}")
link_directories(
	${CMAKE_BINARY_DIR}/luatables-prefix/src/luatables-build/ 
	/usr/lib/x86_64-linux-gnu/
)

# Create a executable named n_a_o-engine
# with the source file: main.cpp
#qi_create_bin(NAO-engine "main.cpp")

# Add a simple test:
qi_create_bin(test_luatables "test/test_luatables.cpp")
target_link_libraries(test_luatables libluatables.so lua5.1) 
