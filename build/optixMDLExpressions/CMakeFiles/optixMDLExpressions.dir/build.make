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
include optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/depend.make

# Include the progress variables for this target.
include optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/progress.make

# Include the compile flags for this target's objects.
include optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/flags.make

optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.o: optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/flags.make
optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.o: ../optixMDLExpressions/optixMDLExpressions.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.o"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.o -c /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixMDLExpressions/optixMDLExpressions.cpp

optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.i"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixMDLExpressions/optixMDLExpressions.cpp > CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.i

optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.s"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixMDLExpressions/optixMDLExpressions.cpp -o CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.s

# Object files for target optixMDLExpressions
optixMDLExpressions_OBJECTS = \
"CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.o"

# External object files for target optixMDLExpressions
optixMDLExpressions_EXTERNAL_OBJECTS =

bin/optixMDLExpressions: optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/optixMDLExpressions.cpp.o
bin/optixMDLExpressions: optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/build.make
bin/optixMDLExpressions: lib/libsutil_sdk.so
bin/optixMDLExpressions: /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/lib64/liboptix.so
bin/optixMDLExpressions: ../support/mdl_wrapper/lib/libmdl_wrapper.so
bin/optixMDLExpressions: /usr/lib/libglut.so
bin/optixMDLExpressions: /usr/lib/libXmu.so
bin/optixMDLExpressions: /usr/lib/libXi.so
bin/optixMDLExpressions: /usr/lib/libGL.so
bin/optixMDLExpressions: /usr/lib/libGLU.so
bin/optixMDLExpressions: /opt/cuda/lib64/libnvrtc.so
bin/optixMDLExpressions: optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/optixMDLExpressions"
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/optixMDLExpressions.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/build: bin/optixMDLExpressions

.PHONY : optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/build

optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/clean:
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions && $(CMAKE_COMMAND) -P CMakeFiles/optixMDLExpressions.dir/cmake_clean.cmake
.PHONY : optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/clean

optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/depend:
	cd /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/optixMDLExpressions /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions /home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/build/optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : optixMDLExpressions/CMakeFiles/optixMDLExpressions.dir/depend

