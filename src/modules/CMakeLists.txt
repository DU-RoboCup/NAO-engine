SET(SOURCE_FILES 	watchdog.cpp
					../main/mloader.cpp
					../../debug/debug.cpp
					../../memory/memory.cpp
)

set(LIBNAOINTERFACE_SOURCES "base/NAOInterface.cpp" "base/Head.cpp" "base/Arm.cpp" "base/Leg.cpp" 
							"base/LArm.cpp" "base/LLeg.cpp" "base/RArm.cpp" "base/RLeg.cpp"  
							"../../include/modules/base/NAOInterface.h"
)

if (${IS_CROSS} STREQUAL "TRUE")
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	add_library(tmoduleone SHARED "test/testmoduleone.cpp" "../../include/modules/test/testmoduleone.h")
	add_library(NAOInterface SHARED ${LIBNAOINTERFACE_SOURCES})
	target_link_libraries(tmoduleone luatables lua pthread boost_system-mt boost_log-mt boost_log_setup-mt boost_thread-mt)
	target_link_libraries(NAOInterface luatables lua pthread boost_system-mt boost_log-mt boost_log_setup-mt boost_thread-mt)
	set(Boost_USE_MULTITHREADED ON)
else()
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	add_library(tmoduleone SHARED "test/testmoduleone.cpp" "../../include/modules/test/testmoduleone.h")
	add_library(NAOInterface SHARED ${LIBNAOINTERFACE_SOURCES})
	target_link_libraries(tmoduleone luatables lua5.1 dl pthread boost_system boost_log_setup boost_log boost_thread dl)
	target_link_libraries(NAOInterface luatables lua5.1 dl pthread boost_system boost_log_setup boost_log boost_thread dl)
    set(Boost_USE_MULTITHREADED ON)
endif()