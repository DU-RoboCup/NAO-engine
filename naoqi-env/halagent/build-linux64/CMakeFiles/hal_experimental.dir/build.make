# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/vagrant/NAO/NAO-engine/naoqi-env/halagent

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/build-linux64

# Include any dependencies generated for this target.
include CMakeFiles/hal_experimental.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/hal_experimental.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hal_experimental.dir/flags.make

CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o: CMakeFiles/hal_experimental.dir/flags.make
CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o: ../hal_experimental.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/build-linux64/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o"
	/home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/cross/bin/i686-aldebaran-linux-gnu-g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o -c /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/hal_experimental.cpp

CMakeFiles/hal_experimental.dir/hal_experimental.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hal_experimental.dir/hal_experimental.cpp.i"
	/home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/cross/bin/i686-aldebaran-linux-gnu-g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/hal_experimental.cpp > CMakeFiles/hal_experimental.dir/hal_experimental.cpp.i

CMakeFiles/hal_experimental.dir/hal_experimental.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hal_experimental.dir/hal_experimental.cpp.s"
	/home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/cross/bin/i686-aldebaran-linux-gnu-g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/hal_experimental.cpp -o CMakeFiles/hal_experimental.dir/hal_experimental.cpp.s

CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.requires:
.PHONY : CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.requires

CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.provides: CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.requires
	$(MAKE) -f CMakeFiles/hal_experimental.dir/build.make CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.provides.build
.PHONY : CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.provides

CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.provides.build: CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o

# Object files for target hal_experimental
hal_experimental_OBJECTS = \
"CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o"

# External object files for target hal_experimental
hal_experimental_EXTERNAL_OBJECTS =

sdk/bin/hal_experimental: CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o
sdk/bin/hal_experimental: CMakeFiles/hal_experimental.dir/build.make
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libalproxies.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libalcommon.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_signals-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/librttools.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libalvalue.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libalerror.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libqimessaging.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libqitype.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/libnaoqi/lib/libqi.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_chrono-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_filesystem-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_program_options-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_regex-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/cross/i686-aldebaran-linux-gnu/sysroot/usr/lib/libdl.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/cross/i686-aldebaran-linux-gnu/sysroot/usr/lib/librt.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_date_time-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_system-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_locale-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/boost/lib/libboost_thread-mt-1_55.so
sdk/bin/hal_experimental: /home/vagrant/NAO/devtools/ctc-linux64-atom-2.1.4.13/cross/i686-aldebaran-linux-gnu/sysroot/usr/lib/libpthread.so
sdk/bin/hal_experimental: CMakeFiles/hal_experimental.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable sdk/bin/hal_experimental"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hal_experimental.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hal_experimental.dir/build: sdk/bin/hal_experimental
.PHONY : CMakeFiles/hal_experimental.dir/build

CMakeFiles/hal_experimental.dir/requires: CMakeFiles/hal_experimental.dir/hal_experimental.cpp.o.requires
.PHONY : CMakeFiles/hal_experimental.dir/requires

CMakeFiles/hal_experimental.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hal_experimental.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hal_experimental.dir/clean

CMakeFiles/hal_experimental.dir/depend:
	cd /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/build-linux64 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/vagrant/NAO/NAO-engine/naoqi-env/halagent /home/vagrant/NAO/NAO-engine/naoqi-env/halagent /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/build-linux64 /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/build-linux64 /home/vagrant/NAO/NAO-engine/naoqi-env/halagent/build-linux64/CMakeFiles/hal_experimental.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hal_experimental.dir/depend
