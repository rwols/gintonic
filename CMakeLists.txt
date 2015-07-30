cmake_minimum_required (VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
if (WIN32)
	# set(BOOST_ROOT "C:\\boost_1_58_0")
	if ("$ENV{BOOST}" STREQUAL "")
		message(FATAL_ERROR "Set up an environment variable called BOOST to point to the root Boost folder.")
	endif ()
	set(BOOST_ROOT $ENV{BOOST})
	set(Boost_LIBRARY_DIR "${BOOST_ROOT}\\lib32-msvc-12.0")
	
	option(HIDE_CONSOLE "Hide the console on Windows." OFF)
	option(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE "Redirect the standard output streams to files." OFF)
	
	if (HIDE_CONSOLE)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	endif ()
else ()
	add_definitions(-std=c++11)
	add_definitions(-Wall)
	add_definitions(-msse3)
endif (WIN32)

option(WITH_PROFILING "Profile various math functions. " OFF)
option(WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
include_directories("include")
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

if (NOT WIN32)
	include_directories(
		${JPEG_INCLUDE_DIR}
		${PNG_INCLUDE_DIRS})
endif ()

# include_directories(${SDL2_INCLUDE_DIR})
# include_directories(${Boost_INCLUDE_DIR})
# if (NOT WIN32)
# 	include_directories(${JPEG_INCLUDE_DIR})
# 	include_directories(${PNG_INCLUDE_DIRS})
# endif ()
# include_directories(${FREETYPE_INCLUDE_DIRS})
# include_directories(${OPENGL_INCLUDE_DIRS})
# include_directories(${FBX_INCLUDE_DIR})

mark_as_advanced(FBX_INCLUDE_DIR)
mark_as_advanced(FBX_LIBRARY)
mark_as_advanced(FBX_LIBRARY_DEBUG)
mark_as_advanced(SDL2MAIN_LIBRARY)
mark_as_advanced(SDL2_INCLUDE_DIR)
mark_as_advanced(SDL2_LIBRARY)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
mark_as_advanced(CMAKE_INSTALL_PREFIX)

macro(target_link_gintonic targetname)
	target_link_libraries(
		${targetname} 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${JPEG_LIBRARIES} 
		${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY_DEBUG} 
		${CMAKE_THREAD_LIBS_INIT}
		-ldl
		gintonic)
endmacro(target_link_gintonic)

macro(target_link_no_lib targetname)
	target_link_libraries(
		${targetname} 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${JPEG_LIBRARIES} 
		${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY_DEBUG} 
		${CMAKE_THREAD_LIBS_INIT}
		-ldl)
endmacro(target_link_no_lib)

add_subdirectory (lib)
add_subdirectory (examples)
add_subdirectory (test)

enable_testing()

add_test(NAME math COMMAND math)
add_test(NAME materials COMMAND materials)