SET(CROSS_SOURCE_FILES 	watchdog.cpp
						../main/mloader.cpp
						../../debug/debug.cpp
						../../memory/memory.cpp
						../main/context.cpp
						../main/schedule.cpp
)

SET(SOURCE_FILES 	watchdog.cpp
					../main/mloader.cpp
					../../debug/debug.cpp
					../../memory/memory.cpp
					../main/context.cpp
					../main/schedule.cpp
)

if (${IS_CROSS} STREQUAL "TRUE")
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	add_executable(pineapple ${CROSS_SOURCE_FILES})
	target_link_libraries(pineapple luatables lua dl pthread boost_system-mt boost_log-mt boost_log_setup-mt boost_thread-mt)
	set(Boost_USE_MULTITHREADED ON)
else()
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	add_executable(pineapple ${CROSS_SOURCE_FILES})
	target_link_libraries(pineapple luatables lua5.1 dl pthread boost_system boost_log boost_log_setup boost_thread)
	set(Boost_USE_MULTITHREADED ON)
endif()
