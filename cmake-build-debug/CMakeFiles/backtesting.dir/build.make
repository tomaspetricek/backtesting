# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/backtesting.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/backtesting.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/backtesting.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/backtesting.dir/flags.make

CMakeFiles/backtesting.dir/src/main.cpp.o: CMakeFiles/backtesting.dir/flags.make
CMakeFiles/backtesting.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/backtesting.dir/src/main.cpp.o: CMakeFiles/backtesting.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/backtesting.dir/src/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/backtesting.dir/src/main.cpp.o -MF CMakeFiles/backtesting.dir/src/main.cpp.o.d -o CMakeFiles/backtesting.dir/src/main.cpp.o -c /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/src/main.cpp

CMakeFiles/backtesting.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/backtesting.dir/src/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/src/main.cpp > CMakeFiles/backtesting.dir/src/main.cpp.i

CMakeFiles/backtesting.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/backtesting.dir/src/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/src/main.cpp -o CMakeFiles/backtesting.dir/src/main.cpp.s

# Object files for target backtesting
backtesting_OBJECTS = \
"CMakeFiles/backtesting.dir/src/main.cpp.o"

# External object files for target backtesting
backtesting_EXTERNAL_OBJECTS =

backtesting: CMakeFiles/backtesting.dir/src/main.cpp.o
backtesting: CMakeFiles/backtesting.dir/build.make
backtesting: /usr/local/lib/libboost_date_time-mt.dylib
backtesting: /usr/local/lib/libfmt.8.1.1.dylib
backtesting: CMakeFiles/backtesting.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable backtesting"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/backtesting.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/backtesting.dir/build: backtesting
.PHONY : CMakeFiles/backtesting.dir/build

CMakeFiles/backtesting.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/backtesting.dir/cmake_clean.cmake
.PHONY : CMakeFiles/backtesting.dir/clean

CMakeFiles/backtesting.dir/depend:
	cd /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug /Users/tomaspetricek/CVUT/cvut-2022-2023/diplomova-prace/backtesting/cmake-build-debug/CMakeFiles/backtesting.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/backtesting.dir/depend

