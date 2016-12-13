cmake_minimum_required (VERSION 3.0)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-source build. \
  	Please create a build directory outside of the \
  	Gintonic source code and call CMake from there.")
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type \(Debug/Release\)")
	set(BUILD_SHARED_LIBS ON CACHE BOOL "Wether to build shared or static libraries.")
endif()

project(gintonic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type. Can be Debug or Release or whatnot")

enable_testing()

add_subdirectory(Resources)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tools)
add_subdirectory(examples)
