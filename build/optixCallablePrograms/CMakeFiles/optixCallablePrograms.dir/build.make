# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_SOURCE_DIR = /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build

# Include any dependencies generated for this target.
include optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/depend.make

# Include the progress variables for this target.
include optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/progress.make

# Include the compile flags for this target's objects.
include optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/flags.make

optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.o: optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/flags.make
optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.o: ../optixCallablePrograms/optixCallablePrograms.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.o"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.o -c /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixCallablePrograms/optixCallablePrograms.cpp

optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.i"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixCallablePrograms/optixCallablePrograms.cpp > CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.i

optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.s"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixCallablePrograms/optixCallablePrograms.cpp -o CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.s

# Object files for target optixCallablePrograms
optixCallablePrograms_OBJECTS = \
"CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.o"

# External object files for target optixCallablePrograms
optixCallablePrograms_EXTERNAL_OBJECTS =

bin/optixCallablePrograms: optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/optixCallablePrograms.cpp.o
bin/optixCallablePrograms: optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/build.make
bin/optixCallablePrograms: lib/libsutil_sdk.so
bin/optixCallablePrograms: /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/lib64/liboptix.so
bin/optixCallablePrograms: /usr/lib/libglut.so
bin/optixCallablePrograms: /usr/lib/libXmu.so
bin/optixCallablePrograms: /usr/lib/libXi.so
bin/optixCallablePrograms: /usr/lib/libGL.so
bin/optixCallablePrograms: /usr/lib/libGLU.so
bin/optixCallablePrograms: /opt/cuda/lib64/libnvrtc.so
bin/optixCallablePrograms: optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/optixCallablePrograms"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/optixCallablePrograms.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/build: bin/optixCallablePrograms

.PHONY : optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/build

optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/clean:
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms && $(CMAKE_COMMAND) -P CMakeFiles/optixCallablePrograms.dir/cmake_clean.cmake
.PHONY : optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/clean

optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/depend:
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixCallablePrograms /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : optixCallablePrograms/CMakeFiles/optixCallablePrograms.dir/depend

