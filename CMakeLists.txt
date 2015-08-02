cmake_minimum_required (VERSION 2.8)

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
if (DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel.")
else()
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel.")
endif()

project(gintonic)

add_subdirectory (lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_subdirectory (examples)
	add_subdirectory (test)
	enable_testing()
	add_test(NAME math COMMAND math)
	add_test(NAME materials COMMAND materials)
endif ()