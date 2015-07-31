cmake_minimum_required (VERSION 2.8)

project(gintonic)

set(project_name Gintonic CACHE STRING "The name of the project")
set(project_version v0.1 CACHE STRING "The project version")

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
if (DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()

# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if (WIN32)
	if ("$ENV{BOOST}" STREQUAL "")
		message(FATAL_ERROR "Set up an environment variable called BOOST to point to the root Boost folder.")
	endif ()
	set(BOOST_ROOT $ENV{BOOST})
	
	set(HIDE_CONSOLE OFF CACHE BOOL "Hide the console on Windows." )
	set(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE OFF CACHE BOOL "Redirect the standard output streams to files.")
	
	if (HIDE_CONSOLE)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	endif ()

	# Various bugfixes
	add_definitions(/D_SCL_SECURE_NO_WARNINGS)
	add_definitions(/DNOMINMAX)
	add_definitions(/DBOOST_ALL_NO_LIB)
	add_definitions(/DBOOST_TEST_DYN_LINK) 
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

	# Enable multithreaded compilation
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
else ()
	add_definitions(-DBOOST_TEST_DYN_LINK) 
	add_definitions(-std=c++11)
	add_definitions(-Wall)
	add_definitions(-msse3)
endif ()

set(WITH_PROFILING OFF CACHE BOOL "Profile various math functions. ")
set(WITH_MEMORY_PROFILING OFF CACHE BOOL "Profile various memory allocations.")

include_directories("include")

# Configure Boost libraries
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(Boost_USE_STATIC_LIBS OFF)
	find_package(Boost
		COMPONENTS
			system
			filesystem
			serialization
			program_options
			unit_test_framework
		REQUIRED)
else ()
	set(Boost_USE_STATIC_LIBS ON)
	find_package(Boost
		COMPONENTS
			system
			filesystem
			serialization
			program_options
		REQUIRED)
endif ()

find_package(OpenGL REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
if (NOT WIN32)
	find_package(JPEG REQUIRED)
	find_package(PNG REQUIRED)
endif ()
find_package(Freetype REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")
# find_package(FBX REQUIRED)
if (APPLE)
	set(FBX_ROOT "/Applications/Autodesk/FBX\ SDK/2015.1")
	set(FBX_INCLUDE_DIR "${FBX_ROOT}/include")
	set(FBX_LIBRARY "${FBX_ROOT}/lib/clang/release/libfbxsdk.a")
	set(FBX_LIBRARY_DEBUG "${FBX_ROOT}/lib/clang/debug/libfbxsdk.a")
elseif (WIN32)
	find_package(FBX REQUIRED)
else ()
	set(FBX_ROOT "/usr/local")
	set(FBX_INCLUDE_DIR "${FBX_ROOT}/include")
	set(FBX_LIBRARY "${FBX_ROOT}/lib/gcc4/x64/release/libfbxsdk.so")
	set(FBX_LIBRARY_DEBUG "${FBX_ROOT}/lib/gcc4/x64/debug/libfbxsdk.so")
endif ()

find_package(SDL2 REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR})

if (WIN32)
	list(GET SDL2_LIBRARY 1 SDL2_dll)
	string(REPLACE ".lib" ".dll" SDL2_dll ${SDL2_dll})
	file(COPY ${SDL2_dll} DESTINATION ${CMAKE_BINARY_DIR})
	set(Boost_DLL_are_copied OFF CACHE INTERNAL "Only copy Boost DLLs once.")
	if (NOT Boost_USE_STATIC_LIBS AND NOT Boost_DLL_are_copied)
		message(STATUS "Copying Boost DLLs to build folder.")
		set(found_correct_library OFF)
		set(boost_lib_token "optimized")
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(boost_lib_token "debug")
		endif ()
		foreach (list_item IN LISTS Boost_LIBRARIES)
			# message(STATUS "Current list item: ${list_item}")
			if (list_item STREQUAL boost_lib_token)
				# message(STATUS "Found debug lib...")
				set(found_correct_library ON)
			elseif (found_correct_library)
				# message(STATUS "Replacing .lib with .dll")
				string(REPLACE ".lib" ".dll" list_item ${list_item})
				# message(STATUS "Copying ${list_item} to ${CMAKE_BINARY_DIR}")
				file(COPY ${list_item} DESTINATION ${CMAKE_BINARY_DIR})
				set(found_correct_library OFF)
			endif ()
		endforeach()
		set(Boost_DLL_are_copied ON CACHE INTERNAL "Only copy Boost DLLs once.")
	endif ()
else ()
	include_directories(
		${JPEG_INCLUDE_DIR}
		${PNG_INCLUDE_DIRS})
endif ()

mark_as_advanced(FBX_INCLUDE_DIR)
mark_as_advanced(FBX_LIBRARY)
mark_as_advanced(FBX_LIBRARY_DEBUG)
mark_as_advanced(SDL2MAIN_LIBRARY)
mark_as_advanced(SDL2_INCLUDE_DIR)
mark_as_advanced(SDL2_LIBRARY)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
mark_as_advanced(CMAKE_INSTALL_PREFIX)

add_subdirectory (lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_subdirectory (examples)
	add_subdirectory (test)
	enable_testing()
	add_test(NAME math COMMAND math)
	add_test(NAME materials COMMAND materials)
endif ()