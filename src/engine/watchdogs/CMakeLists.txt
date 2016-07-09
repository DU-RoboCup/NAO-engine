SET(SOURCE_FILES 	watchdog.cpp
					../main/mloader.cpp
					../../debug/debug.cpp
					../../memory/memory.cpp
					../main/frame.cpp
					../main/schedule.cpp
)

SET(CROSS_SOURCE_FILES 	watchdog.cpp
						../main/mloader.cpp
						../../debug/debug.cpp
						../../memory/memory.cpp
						../main/frame.cpp
						../main/schedule.cpp
)

if (${IS_CROSS} STREQUAL "TRUE")
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	qi_create_bin(pineapple ${CROSS_SOURCE_FILES})
	qi_use_lib(pineapple luatables lua dl boost_system boost_log boost_log_setup boost_thread)
	message("Boost Directory:" ${BOOST_LIBRARY_DIRS})

else()
	set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
	qi_create_bin(pineapple ${SOURCE_FILES})
    qi_use_lib(pineapple luatables lua dl PTHREAD)
    target_link_libraries(pineapple boost_system boost_log_setup boost_log boost_thread dl)
    set(Boost_USE_MULTITHREADED ON)
endif()
