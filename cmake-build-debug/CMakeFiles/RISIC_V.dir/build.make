# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/desktop/CLionProjects/RISIC_V

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/RISIC_V.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/RISIC_V.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/RISIC_V.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RISIC_V.dir/flags.make

CMakeFiles/RISIC_V.dir/main.cpp.o: CMakeFiles/RISIC_V.dir/flags.make
CMakeFiles/RISIC_V.dir/main.cpp.o: ../main.cpp
CMakeFiles/RISIC_V.dir/main.cpp.o: CMakeFiles/RISIC_V.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/RISIC_V.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RISIC_V.dir/main.cpp.o -MF CMakeFiles/RISIC_V.dir/main.cpp.o.d -o CMakeFiles/RISIC_V.dir/main.cpp.o -c /mnt/d/desktop/CLionProjects/RISIC_V/main.cpp

CMakeFiles/RISIC_V.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RISIC_V.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/desktop/CLionProjects/RISIC_V/main.cpp > CMakeFiles/RISIC_V.dir/main.cpp.i

CMakeFiles/RISIC_V.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RISIC_V.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/desktop/CLionProjects/RISIC_V/main.cpp -o CMakeFiles/RISIC_V.dir/main.cpp.s

# Object files for target RISIC_V
RISIC_V_OBJECTS = \
"CMakeFiles/RISIC_V.dir/main.cpp.o"

# External object files for target RISIC_V
RISIC_V_EXTERNAL_OBJECTS =

RISIC_V: CMakeFiles/RISIC_V.dir/main.cpp.o
RISIC_V: CMakeFiles/RISIC_V.dir/build.make
RISIC_V: CMakeFiles/RISIC_V.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable RISIC_V"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RISIC_V.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RISIC_V.dir/build: RISIC_V
.PHONY : CMakeFiles/RISIC_V.dir/build

CMakeFiles/RISIC_V.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RISIC_V.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RISIC_V.dir/clean

CMakeFiles/RISIC_V.dir/depend:
	cd /mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/desktop/CLionProjects/RISIC_V /mnt/d/desktop/CLionProjects/RISIC_V /mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug /mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug /mnt/d/desktop/CLionProjects/RISIC_V/cmake-build-debug/CMakeFiles/RISIC_V.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/RISIC_V.dir/depend

