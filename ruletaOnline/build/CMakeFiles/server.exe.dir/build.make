# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build

# Include any dependencies generated for this target.
include CMakeFiles/server.exe.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/server.exe.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server.exe.dir/flags.make

CMakeFiles/server.exe.dir/src/server/server.c.o: CMakeFiles/server.exe.dir/flags.make
CMakeFiles/server.exe.dir/src/server/server.c.o: ../src/server/server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/server.exe.dir/src/server/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/server.exe.dir/src/server/server.c.o -c /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/src/server/server.c

CMakeFiles/server.exe.dir/src/server/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/server.exe.dir/src/server/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/src/server/server.c > CMakeFiles/server.exe.dir/src/server/server.c.i

CMakeFiles/server.exe.dir/src/server/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/server.exe.dir/src/server/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/src/server/server.c -o CMakeFiles/server.exe.dir/src/server/server.c.s

# Object files for target server.exe
server_exe_OBJECTS = \
"CMakeFiles/server.exe.dir/src/server/server.c.o"

# External object files for target server.exe
server_exe_EXTERNAL_OBJECTS =

server.exe: CMakeFiles/server.exe.dir/src/server/server.c.o
server.exe: CMakeFiles/server.exe.dir/build.make
server.exe: CMakeFiles/server.exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable server.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server.exe.dir/build: server.exe

.PHONY : CMakeFiles/server.exe.dir/build

CMakeFiles/server.exe.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server.exe.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server.exe.dir/clean

CMakeFiles/server.exe.dir/depend:
	cd /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build /home/ricardo/Universidad/Tercero/Redes/Practica/P2/ruletaOnline-main/ruletaOnline/build/CMakeFiles/server.exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server.exe.dir/depend

