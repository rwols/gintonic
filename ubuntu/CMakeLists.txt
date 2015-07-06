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
set(source_dir "../src")

set(WITH_PROFILING false) # Enable this for math functions profiling
set(WITH_MEMORY_PROFILING false) # Enable this for memory profiling

project(${project_name})

file(GLOB source_files "${source_dir}/*.cpp")

configure_file(config.hpp.in config.hpp)
include_directories(${CMAKE_BINARY_DIR})

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)

# Find boost libraries
find_package(Boost
	COMPONENTS
		unit_test_framework
		program_options
		filesystem
		serialization
		system
	REQUIRED)

find_package(OpenGL REQUIRED)
# find_package(GLEW REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)

INCLUDE(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)

set(FBX_ROOT "/usr/local")
set(FBX_INCLUDE_DIR "${FBX_ROOT}/include")
set(FBX_LIBRARIES "${FBX_ROOT}/lib/gcc4/x64/release/libfbxsdk.so")

# include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${FBX_INCLUDE_DIR})

add_executable(${project_name} ${source_files})
target_link_libraries(
	${project_name} 
	${Boost_LIBRARIES} 
	${OPENGL_LIBRARIES} 
	# ${GLEW_LIBRARIES} 
	${SDL2_LIBRARIES} 
	${JPEG_LIBRARIES} 
	${PNG_LIBRARIES} 
	${FREETYPE_LIBRARIES} 
	${FBX_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT}
	-ldl)
add_definitions(-std=c++11)
add_definitions(-Wall)
# add_definitions(-Werror)
add_definitions(-msse3)