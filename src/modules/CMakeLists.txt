SET(SOURCE_FILES 	watchdog.cpp
					../main/mloader.cpp
					../../debug/debug.cpp
					../../memory/memory.cpp
)

set(LIBNAOINTERFACE_SOURCES "base/NAOInterface.cpp" "base/Head.cpp" "base/Arm.cpp" "base/Leg.cpp" "base/LArm.cpp" "base/LLeg.cpp" "base/RArm.cpp" "base/RLeg.cpp" "base/HALPriorityQueue.cpp" "../util/hardwaremap.cpp" "../../../include/modules/hardwaremap.h" "../../include/modules/base/NAOInterface.h")

if (${IS_CROSS} STREQUAL "TRUE")
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	add_definitions(-DBETTER_ENUMS_MACRO_FILE="../../include/util/enum_macros.h")
	add_library(tmoduleone SHARED "test/testmoduleone.cpp" "../../include/modules/test/testmoduleone.h")
	add_library(NAOInterface SHARED "base/NAOInterface.cpp" "base/Head.cpp" "base/Arm.cpp" "base/Leg.cpp" "base/LArm.cpp" "base/LLeg.cpp" "base/RArm.cpp" "base/RLeg.cpp" "base/HALPriorityQueue.cpp" "../util/hardwaremap.cpp" "../../../include/util/hardwaremap.h" "../../include/modules/base/NAOInterface.h")
	add_library(networking SHARED "com/networking.cpp" "com/udp_handler.cpp" "../../include/modules/com/networking.h" "../../include/modules/com/udp_handler.h")
	target_link_libraries(networking luatables lua pthread boost_system-mt boost_log-mt boost_log_setup-mt boost_thread-mt)
	target_link_libraries(tmoduleone luatables lua pthread boost_system-mt boost_log-mt boost_log_setup-mt boost_thread-mt)
	target_link_libraries(NAOInterface luatables lua pthread boost_system-mt boost_log-mt boost_log_setup-mt boost_thread-mt)
	
	set(Boost_USE_MULTITHREADED ON)
else()
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	add_library(tmoduleone SHARED "test/testmoduleone.cpp" "../../include/modules/test/testmoduleone.h")
	add_definitions(-DBETTER_ENUMS_MACRO_FILE="../../include/util/enum_macros.h")
	add_library(networking SHARED "com/networking.cpp" "com/udp_handler.cpp" "../../include/modules/com/networking.h" "../../include/modules/com/udp_handler.h")
	add_library(NAOInterface SHARED "base/NAOInterface.cpp" "base/Head.cpp" "base/Arm.cpp" "base/Leg.cpp" "base/LArm.cpp" "base/LLeg.cpp" "base/RArm.cpp" "base/RLeg.cpp" "base/HALPriorityQueue.cpp" "../util/hardwaremap.cpp" "../../../include/util/hardwaremap.h" "../../include/modules/base/NAOInterface.h")
	target_link_libraries(tmoduleone luatables lua5.1 dl pthread boost_system boost_log_setup boost_log boost_thread dl)
	target_link_libraries(NAOInterface luatables lua5.1 dl pthread boost_system boost_log_setup boost_log boost_thread dl)
	target_link_libraries(networking luatables lua5.1 dl pthread boost_system boost_log_setup boost_log boost_thread dl)

    set(Boost_USE_MULTITHREADED ON)
endif()
