cmake_minimum_required (VERSION 2.8)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-source build. Please create a build directory outside of the Gintonic source code and call CMake from there.")
endif()

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

project(gintonic C CXX)

if (WIN32)
	if ("$ENV{BOOST}" STREQUAL "")
		message(WARNING "The %BOOST% environment variable is not set.")
	else ()
		set(BOOST_ROOT $ENV{BOOST})
	endif ()

	# Various bugfixes
	add_definitions(/D_SCL_SECURE_NO_WARNINGS)
	add_definitions(/DNOMINMAX)
	add_definitions(/DBOOST_ALL_NO_LIB)
	add_definitions(/DBOOST_PROGRAM_OPTIONS_DYN_LINK)

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		# For some reason this still gives warnings about
		# linking to a wrong C runtime. No idea how to fix this.
		# add_definitions(/MD)
	else ()
		# For some reason this still gives warnings about
		# linking to a wrong C runtime. No idea how to fix this.
		# add_definitions(/MD)
	endif ()

	# WARNING: BIG HACK!!!
	# Remove this when the above is fixed.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4098" CACHE INTERNAL "Big hack")

	# Make the linker shut up about not inlining functions
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4710" CACHE INTERNAL "Shut up about inlining")

	# Enable multithreaded compilation
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
else ()
	add_definitions(-std=c++11)
	add_definitions(-Wall)
	add_definitions(-msse3)

	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=\$$ORIGIN" 
		CACHE INTERNAL "Set dynamic library path to the executable's folder.")
	
endif ()

add_subdirectory (lib)
add_subdirectory (examples)
add_subdirectory (test)
enable_testing()
add_test(NAME math COMMAND math)
add_test(NAME materials COMMAND materials)
add_test(NAME vec2 COMMAND vec2)