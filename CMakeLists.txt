cmake_minimum_required (VERSION 2.8)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-source build. Please create a build directory outside of the Gintonic source code and call CMake from there.")
endif()

if (WIN32)
	# This flattens all directory structures in the build directory.
	# We do this so that we don't have to copy DLLs around for each
	# directory that contains executables.
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
	    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
	    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
	endforeach()
endif ()

project(gintonic C CXX)

if (WIN32)
	if ("$ENV{BOOST_ROOT}" STREQUAL "")
		message(WARNING "The %BOOST_ROOT% environment variable is not set.")
	endif ()

	# Various bugfixes
	add_definitions(/D_SCL_SECURE_NO_WARNINGS)
	add_definitions(/DNOMINMAX)
	add_definitions(/D_USE_MATH_DEFINES)
	add_definitions(/DBOOST_ALL_NO_LIB) # THIS FIXES STUFF FOR SOME MAGICAL REASON
	# add_definitions(/DBOOST_PROGRAM_OPTIONS_DYN_LINK)
	# add_definitions(/MT)

	# WARNING: BIG HACK!!!
	# Remove this when the above is fixed.
	# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4098" CACHE INTERNAL "Big hack")

	# Make the linker shut up about not inlining functions
	# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4710" CACHE INTERNAL "Shut up about inlining")

	# Enable multithreaded compilation
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
else ()
	add_definitions(-std=c++11)
	add_definitions(-Wall)
	add_definitions(-msse3)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=\$$ORIGIN" 
			CACHE INTERNAL "Set dynamic library path to the executable's folder.")
	endif ()
	
endif ()

find_package(Boost
	COMPONENTS
		system
		filesystem
		serialization
		program_options
		unit_test_framework
	REQUIRED)

find_package(OpenGL REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
if (WIN32)
	# do nothing
else ()
	find_package(JPEG REQUIRED)
	find_package(PNG REQUIRED)
endif ()

find_package(Freetype REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")
find_package(FBX REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR}
	${CMAKE_BINARY_DIR} # For the config file
	)

if (WIN32)
	set(next_item_has_optimized_tag OFF)
	set(next_item_has_debug_tag OFF)
	foreach (list_item IN LISTS Boost_LIBRARIES)
		if (list_item STREQUAL "optimized")
			set(next_item_has_optimized_tag ON)
			set(next_item_has_debug_tag OFF)
		elseif (list_item STREQUAL "debug")
			set(next_item_has_debug_tag ON)
			set(next_item_has_optimized_tag OFF)
		elseif (next_item_has_debug_tag)
			string(REPLACE ".lib" ".dll" list_item ${list_item})
			list(APPEND Boost_DEBUG_DLLS ${list_item})
			set(next_item_has_optimized_tag OFF)
			set(next_item_has_debug_tag OFF)
		elseif (next_item_has_optimized_tag)
			string(REPLACE ".lib" ".dll" list_item ${list_item})
			list(APPEND Boost_DLLS ${list_item})
			set(next_item_has_optimized_tag OFF)
			set(next_item_has_debug_tag OFF)
		endif ()
	endforeach()
	list(GET SDL2_LIBRARY 1 SDL2_DLL)
	string(REPLACE ".lib" ".dll" SDL2_DLL ${SDL2_DLL})
	string(REPLACE ".lib" ".dll" FBX_DLL ${FBX_LIBRARY})

	set (DEBUG_DLLS ${Boost_DEBUG_DLLS} ${SDL2_DLL} ${FBX_DLL})
	set (RELEASE_DLLS ${Boost_DLLS} ${SDL2_DLL} ${FBX_DLL})

	message(STATUS "Here's the list of debug DLLs:")
	foreach (DLL ${DEBUG_DLLS})
		message(STATUS "${DLL}")
		file (COPY ${DLL} DESTINATION ${CMAKE_BINARY_DIR})
	endforeach()

	message(STATUS "Here's the list of release DLLs:")
	foreach (DLL ${RELEASE_DLLS})
		message(STATUS "${DLL}")
		file (COPY ${DLL} DESTINATION ${CMAKE_BINARY_DIR})
	endforeach()

else ()
	include_directories(
		${JPEG_INCLUDE_DIR}
		${PNG_INCLUDE_DIRS})

	if (APPLE)
		# Need to copy fbxsdk
		list(GET FBX_LIBRARY 0 FBX_DLL)
		file (COPY ${FBX_DLL} DESTINATION ${CMAKE_BINARY_DIR}/examples)
		file (COPY ${FBX_DLL} DESTINATION ${CMAKE_BINARY_DIR}/test)
	endif ()
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
add_subdirectory (tools)
add_subdirectory (examples)
add_subdirectory (test)

enable_testing()

add_test(NAME materials COMMAND materials)
add_test(NAME SQT COMMAND SQT)
add_test(NAME box2f COMMAND box2f)
add_test(NAME vec2f COMMAND vec2f)
add_test(NAME vec3f COMMAND vec3f)
add_test(NAME vec4f COMMAND vec4f)
add_test(NAME quatf COMMAND quatf)
add_test(NAME mat2f COMMAND mat2f)
add_test(NAME mat3f COMMAND mat3f)
add_test(NAME mat4f COMMAND mat4f)