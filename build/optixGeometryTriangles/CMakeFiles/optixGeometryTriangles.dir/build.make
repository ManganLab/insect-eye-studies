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
include optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/depend.make

# Include the progress variables for this target.
include optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/progress.make

# Include the compile flags for this target's objects.
include optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/flags.make

optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.o: optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/flags.make
optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.o: ../optixGeometryTriangles/optixGeometryTriangles.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.o"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.o -c /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixGeometryTriangles/optixGeometryTriangles.cpp

optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.i"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixGeometryTriangles/optixGeometryTriangles.cpp > CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.i

optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.s"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixGeometryTriangles/optixGeometryTriangles.cpp -o CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.s

# Object files for target optixGeometryTriangles
optixGeometryTriangles_OBJECTS = \
"CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.o"

# External object files for target optixGeometryTriangles
optixGeometryTriangles_EXTERNAL_OBJECTS =

bin/optixGeometryTriangles: optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/optixGeometryTriangles.cpp.o
bin/optixGeometryTriangles: optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/build.make
bin/optixGeometryTriangles: lib/libsutil_sdk.so
bin/optixGeometryTriangles: /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/lib64/liboptix.so
bin/optixGeometryTriangles: /usr/lib/libglut.so
bin/optixGeometryTriangles: /usr/lib/libXmu.so
bin/optixGeometryTriangles: /usr/lib/libXi.so
bin/optixGeometryTriangles: /usr/lib/libGL.so
bin/optixGeometryTriangles: /usr/lib/libGLU.so
bin/optixGeometryTriangles: /opt/cuda/lib64/libnvrtc.so
bin/optixGeometryTriangles: optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/optixGeometryTriangles"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/optixGeometryTriangles.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/build: bin/optixGeometryTriangles

.PHONY : optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/build

optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/clean:
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles && $(CMAKE_COMMAND) -P CMakeFiles/optixGeometryTriangles.dir/cmake_clean.cmake
.PHONY : optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/clean

optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/depend:
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixGeometryTriangles /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : optixGeometryTriangles/CMakeFiles/optixGeometryTriangles.dir/depend

