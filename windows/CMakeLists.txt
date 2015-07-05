#
# This is the CMake file for the gintonic project.
#
# To build the project, create a subdirectory in the main directory
# with any name. For the moment, we'll call the folder "build".
# cd into the build directory and type
#
#     cmake -DCMAKE_BUILD_TYPE=Debug ..
#
# to prepare a debug build. For a release build, type
#
#     cmake -DCMAKE_BUILD_TYPE=Release ..
#
# once that is done, you can type
#
#     cmake --build .
#
# This should work in any environment.

cmake_minimum_required(VERSION 2.8)

set(project_name gt)
set(project_version v0.1)
set(source_dir ../src)

set(WITH_PROFILING FALSE) # Enable this for math functions profiling
set(WITH_MEMORY_PROFILING FALSE) # Enable this for memory profiling

project(${project_name} C CXX)

file(GLOB source_files ${source_dir}/*.cpp)

configure_file(config.hpp.in config.hpp)
include_directories(${CMAKE_BINARY_DIR})

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)

# Find boost libraries
set(BOOST_ROOT "C:\\boost_1_58_0")
set(Boost_LIBRARY_DIR "${BOOST_ROOT}\\lib32-msvc-12.0")
find_package(Boost
	COMPONENTS
		program_options
		filesystem
		serialization
		system
	REQUIRED)

find_package(OpenGL REQUIRED)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")

# Set SDL in your enviroment to point to the SDL2 folder.
find_package(SDL2 REQUIRED)

# Set FBX_DIR in your environment to point to the FBX SDK root folder.
find_package(FBX REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${FBX_INCLUDE_DIR})

add_executable(${project_name} ${source_files})
target_link_libraries(
	${project_name} 
	windowscodecs.lib 
	${Boost_LIBRARIES} 
	${OPENGL_LIBRARIES} 
	${SDL2_LIBRARY} 
	${FBX_LIBRARY_DEBUG} 
	${CMAKE_THREAD_LIBS_INIT})
# add_definitions(-std=c++11)
# add_definitions(-Wall)
# add_definitions(-Werror)
# add_definitions(-msse3)