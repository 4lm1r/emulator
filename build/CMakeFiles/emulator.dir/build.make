# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_SOURCE_DIR = /home/abr/PROJECTS/C++Projects/emulator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/abr/PROJECTS/C++Projects/emulator/build

# Include any dependencies generated for this target.
include CMakeFiles/emulator.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/emulator.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/emulator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/emulator.dir/flags.make

CMakeFiles/emulator.dir/codegen:
.PHONY : CMakeFiles/emulator.dir/codegen

CMakeFiles/emulator.dir/src/Screen.cpp.o: CMakeFiles/emulator.dir/flags.make
CMakeFiles/emulator.dir/src/Screen.cpp.o: /home/abr/PROJECTS/C++Projects/emulator/src/Screen.cpp
CMakeFiles/emulator.dir/src/Screen.cpp.o: CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/emulator.dir/src/Screen.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/emulator.dir/src/Screen.cpp.o -MF CMakeFiles/emulator.dir/src/Screen.cpp.o.d -o CMakeFiles/emulator.dir/src/Screen.cpp.o -c /home/abr/PROJECTS/C++Projects/emulator/src/Screen.cpp

CMakeFiles/emulator.dir/src/Screen.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/Screen.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abr/PROJECTS/C++Projects/emulator/src/Screen.cpp > CMakeFiles/emulator.dir/src/Screen.cpp.i

CMakeFiles/emulator.dir/src/Screen.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/Screen.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abr/PROJECTS/C++Projects/emulator/src/Screen.cpp -o CMakeFiles/emulator.dir/src/Screen.cpp.s

CMakeFiles/emulator.dir/src/Registers.cpp.o: CMakeFiles/emulator.dir/flags.make
CMakeFiles/emulator.dir/src/Registers.cpp.o: /home/abr/PROJECTS/C++Projects/emulator/src/Registers.cpp
CMakeFiles/emulator.dir/src/Registers.cpp.o: CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/emulator.dir/src/Registers.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/emulator.dir/src/Registers.cpp.o -MF CMakeFiles/emulator.dir/src/Registers.cpp.o.d -o CMakeFiles/emulator.dir/src/Registers.cpp.o -c /home/abr/PROJECTS/C++Projects/emulator/src/Registers.cpp

CMakeFiles/emulator.dir/src/Registers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/Registers.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abr/PROJECTS/C++Projects/emulator/src/Registers.cpp > CMakeFiles/emulator.dir/src/Registers.cpp.i

CMakeFiles/emulator.dir/src/Registers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/Registers.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abr/PROJECTS/C++Projects/emulator/src/Registers.cpp -o CMakeFiles/emulator.dir/src/Registers.cpp.s

CMakeFiles/emulator.dir/src/Memory.cpp.o: CMakeFiles/emulator.dir/flags.make
CMakeFiles/emulator.dir/src/Memory.cpp.o: /home/abr/PROJECTS/C++Projects/emulator/src/Memory.cpp
CMakeFiles/emulator.dir/src/Memory.cpp.o: CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/emulator.dir/src/Memory.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/emulator.dir/src/Memory.cpp.o -MF CMakeFiles/emulator.dir/src/Memory.cpp.o.d -o CMakeFiles/emulator.dir/src/Memory.cpp.o -c /home/abr/PROJECTS/C++Projects/emulator/src/Memory.cpp

CMakeFiles/emulator.dir/src/Memory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/Memory.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abr/PROJECTS/C++Projects/emulator/src/Memory.cpp > CMakeFiles/emulator.dir/src/Memory.cpp.i

CMakeFiles/emulator.dir/src/Memory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/Memory.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abr/PROJECTS/C++Projects/emulator/src/Memory.cpp -o CMakeFiles/emulator.dir/src/Memory.cpp.s

CMakeFiles/emulator.dir/src/CPU.cpp.o: CMakeFiles/emulator.dir/flags.make
CMakeFiles/emulator.dir/src/CPU.cpp.o: /home/abr/PROJECTS/C++Projects/emulator/src/CPU.cpp
CMakeFiles/emulator.dir/src/CPU.cpp.o: CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/emulator.dir/src/CPU.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/emulator.dir/src/CPU.cpp.o -MF CMakeFiles/emulator.dir/src/CPU.cpp.o.d -o CMakeFiles/emulator.dir/src/CPU.cpp.o -c /home/abr/PROJECTS/C++Projects/emulator/src/CPU.cpp

CMakeFiles/emulator.dir/src/CPU.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/CPU.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abr/PROJECTS/C++Projects/emulator/src/CPU.cpp > CMakeFiles/emulator.dir/src/CPU.cpp.i

CMakeFiles/emulator.dir/src/CPU.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/CPU.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abr/PROJECTS/C++Projects/emulator/src/CPU.cpp -o CMakeFiles/emulator.dir/src/CPU.cpp.s

CMakeFiles/emulator.dir/src/Emulator.cpp.o: CMakeFiles/emulator.dir/flags.make
CMakeFiles/emulator.dir/src/Emulator.cpp.o: /home/abr/PROJECTS/C++Projects/emulator/src/Emulator.cpp
CMakeFiles/emulator.dir/src/Emulator.cpp.o: CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/emulator.dir/src/Emulator.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/emulator.dir/src/Emulator.cpp.o -MF CMakeFiles/emulator.dir/src/Emulator.cpp.o.d -o CMakeFiles/emulator.dir/src/Emulator.cpp.o -c /home/abr/PROJECTS/C++Projects/emulator/src/Emulator.cpp

CMakeFiles/emulator.dir/src/Emulator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/Emulator.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abr/PROJECTS/C++Projects/emulator/src/Emulator.cpp > CMakeFiles/emulator.dir/src/Emulator.cpp.i

CMakeFiles/emulator.dir/src/Emulator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/Emulator.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abr/PROJECTS/C++Projects/emulator/src/Emulator.cpp -o CMakeFiles/emulator.dir/src/Emulator.cpp.s

CMakeFiles/emulator.dir/src/main.cpp.o: CMakeFiles/emulator.dir/flags.make
CMakeFiles/emulator.dir/src/main.cpp.o: /home/abr/PROJECTS/C++Projects/emulator/src/main.cpp
CMakeFiles/emulator.dir/src/main.cpp.o: CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/emulator.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/emulator.dir/src/main.cpp.o -MF CMakeFiles/emulator.dir/src/main.cpp.o.d -o CMakeFiles/emulator.dir/src/main.cpp.o -c /home/abr/PROJECTS/C++Projects/emulator/src/main.cpp

CMakeFiles/emulator.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abr/PROJECTS/C++Projects/emulator/src/main.cpp > CMakeFiles/emulator.dir/src/main.cpp.i

CMakeFiles/emulator.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abr/PROJECTS/C++Projects/emulator/src/main.cpp -o CMakeFiles/emulator.dir/src/main.cpp.s

# Object files for target emulator
emulator_OBJECTS = \
"CMakeFiles/emulator.dir/src/Screen.cpp.o" \
"CMakeFiles/emulator.dir/src/Registers.cpp.o" \
"CMakeFiles/emulator.dir/src/Memory.cpp.o" \
"CMakeFiles/emulator.dir/src/CPU.cpp.o" \
"CMakeFiles/emulator.dir/src/Emulator.cpp.o" \
"CMakeFiles/emulator.dir/src/main.cpp.o"

# External object files for target emulator
emulator_EXTERNAL_OBJECTS =

emulator: CMakeFiles/emulator.dir/src/Screen.cpp.o
emulator: CMakeFiles/emulator.dir/src/Registers.cpp.o
emulator: CMakeFiles/emulator.dir/src/Memory.cpp.o
emulator: CMakeFiles/emulator.dir/src/CPU.cpp.o
emulator: CMakeFiles/emulator.dir/src/Emulator.cpp.o
emulator: CMakeFiles/emulator.dir/src/main.cpp.o
emulator: CMakeFiles/emulator.dir/build.make
emulator: CMakeFiles/emulator.dir/compiler_depend.ts
emulator: /usr/lib/libcurses.so
emulator: /usr/lib/libform.so
emulator: CMakeFiles/emulator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable emulator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/emulator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/emulator.dir/build: emulator
.PHONY : CMakeFiles/emulator.dir/build

CMakeFiles/emulator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/emulator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/emulator.dir/clean

CMakeFiles/emulator.dir/depend:
	cd /home/abr/PROJECTS/C++Projects/emulator/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abr/PROJECTS/C++Projects/emulator /home/abr/PROJECTS/C++Projects/emulator /home/abr/PROJECTS/C++Projects/emulator/build /home/abr/PROJECTS/C++Projects/emulator/build /home/abr/PROJECTS/C++Projects/emulator/build/CMakeFiles/emulator.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/emulator.dir/depend

