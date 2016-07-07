SET(SOURCE_FILES 	watchdog.cpp
					../main/mloader.cpp
					../../debug/debug.cpp
					../../memory/memory.cpp
)

if (${IS_CROSS} STREQUAL "TRUE")
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	qi_create_lib(tmoduleone SHARED "test/testmoduleone.cpp" "../../include/modules/test/testmoduleone.h")
	qi_use_lib(tmoduleone luatables lua boost_system boost_log boost_log_setup boost_thread)
else()
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	qi_create_lib(tmoduleone SHARED "test/testmoduleone.cpp" "../../include/modules/test/testmoduleone.h")
	qi_use_lib(tmoduleone luatables lua dl pthread)
	target_link_libraries(tmoduleone boost_system boost_log_setup boost_log boost_thread dl)
    set(Boost_USE_MULTITHREADED ON)
endif()