# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build

# Include any dependencies generated for this target.
include CMakeFiles/spdexam.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/spdexam.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/spdexam.dir/flags.make

CMakeFiles/spdexam.dir/Logger.cpp.o: CMakeFiles/spdexam.dir/flags.make
CMakeFiles/spdexam.dir/Logger.cpp.o: ../Logger.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/spdexam.dir/Logger.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/spdexam.dir/Logger.cpp.o -c /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/Logger.cpp

CMakeFiles/spdexam.dir/Logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/spdexam.dir/Logger.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/Logger.cpp > CMakeFiles/spdexam.dir/Logger.cpp.i

CMakeFiles/spdexam.dir/Logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/spdexam.dir/Logger.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/Logger.cpp -o CMakeFiles/spdexam.dir/Logger.cpp.s

CMakeFiles/spdexam.dir/Logger.cpp.o.requires:

.PHONY : CMakeFiles/spdexam.dir/Logger.cpp.o.requires

CMakeFiles/spdexam.dir/Logger.cpp.o.provides: CMakeFiles/spdexam.dir/Logger.cpp.o.requires
	$(MAKE) -f CMakeFiles/spdexam.dir/build.make CMakeFiles/spdexam.dir/Logger.cpp.o.provides.build
.PHONY : CMakeFiles/spdexam.dir/Logger.cpp.o.provides

CMakeFiles/spdexam.dir/Logger.cpp.o.provides.build: CMakeFiles/spdexam.dir/Logger.cpp.o


CMakeFiles/spdexam.dir/Main.cpp.o: CMakeFiles/spdexam.dir/flags.make
CMakeFiles/spdexam.dir/Main.cpp.o: ../Main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/spdexam.dir/Main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/spdexam.dir/Main.cpp.o -c /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/Main.cpp

CMakeFiles/spdexam.dir/Main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/spdexam.dir/Main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/Main.cpp > CMakeFiles/spdexam.dir/Main.cpp.i

CMakeFiles/spdexam.dir/Main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/spdexam.dir/Main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/Main.cpp -o CMakeFiles/spdexam.dir/Main.cpp.s

CMakeFiles/spdexam.dir/Main.cpp.o.requires:

.PHONY : CMakeFiles/spdexam.dir/Main.cpp.o.requires

CMakeFiles/spdexam.dir/Main.cpp.o.provides: CMakeFiles/spdexam.dir/Main.cpp.o.requires
	$(MAKE) -f CMakeFiles/spdexam.dir/build.make CMakeFiles/spdexam.dir/Main.cpp.o.provides.build
.PHONY : CMakeFiles/spdexam.dir/Main.cpp.o.provides

CMakeFiles/spdexam.dir/Main.cpp.o.provides.build: CMakeFiles/spdexam.dir/Main.cpp.o


# Object files for target spdexam
spdexam_OBJECTS = \
"CMakeFiles/spdexam.dir/Logger.cpp.o" \
"CMakeFiles/spdexam.dir/Main.cpp.o"

# External object files for target spdexam
spdexam_EXTERNAL_OBJECTS =

spdexam: CMakeFiles/spdexam.dir/Logger.cpp.o
spdexam: CMakeFiles/spdexam.dir/Main.cpp.o
spdexam: CMakeFiles/spdexam.dir/build.make
spdexam: CMakeFiles/spdexam.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable spdexam"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/spdexam.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/spdexam.dir/build: spdexam

.PHONY : CMakeFiles/spdexam.dir/build

CMakeFiles/spdexam.dir/requires: CMakeFiles/spdexam.dir/Logger.cpp.o.requires
CMakeFiles/spdexam.dir/requires: CMakeFiles/spdexam.dir/Main.cpp.o.requires

.PHONY : CMakeFiles/spdexam.dir/requires

CMakeFiles/spdexam.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/spdexam.dir/cmake_clean.cmake
.PHONY : CMakeFiles/spdexam.dir/clean

CMakeFiles/spdexam.dir/depend:
	cd /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build /home/evan/Desktop/EvanWorkspace/C++/Tips/logging_with_spdlog/logger/build/CMakeFiles/spdexam.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/spdexam.dir/depend

