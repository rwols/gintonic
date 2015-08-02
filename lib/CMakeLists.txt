cmake_minimum_required(VERSION 2.8)



# # First for the generic no-config case (e.g. with mingw)
# set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
# set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
# set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
# # Second, for multi-config builds (e.g. msvc)
# foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
#     string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
#     set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
#     set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
#     set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
# endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Configure Boost libraries
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME OFF)
# set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost
	COMPONENTS
		system
		filesystem
		serialization
		program_options
	REQUIRED)

find_package(OpenGL REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
if (NOT WIN32)
	find_package(JPEG REQUIRED)
	find_package(PNG REQUIRED)
endif ()
find_package(Freetype REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..;${CMAKE_MODULE_PATH}")
find_package(SDL2 REQUIRED)
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

include_directories(
	${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR})

if (WIN32)
	set(DLLs_are_copied OFF CACHE INTERNAL "Only copy DLLs once.")
	if (NOT DLLs_are_copied)
		list(GET SDL2_LIBRARY 1 SDL2_dll)
		string(REPLACE ".lib" ".dll" SDL2_dll ${SDL2_dll})
		file(COPY ${SDL2_dll} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		if (NOT Boost_USE_STATIC_LIBS)
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
					# message(STATUS "Copying ${list_item} to ${CMAKE_CURRENT_BINARY_DIR}")
					if (boost_lib_token STREQUAL "debug")
						file(COPY ${list_item} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}\Debug)
					elseif (boost_lib_token STREQUAL "optimized")
						file(COPY ${list_item} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}\Release)
					endif ()
					set(found_correct_library OFF)
				endif ()
			endforeach()
			set(DLLs_are_copied ON CACHE INTERNAL "Only copy DLLs once." FORCE)
		endif ()
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

set(src_files actors.cpp
	basic_shapes.cpp
	exception.cpp
	fonts.cpp
	gintonic.cpp
	glad.cpp
	lights.cpp
	locks.cpp
	materials.cpp
	math.cpp
	mesh.cpp
	opengl.cpp
	profiler.cpp
	renderer.cpp
	shaders.cpp
	textures.cpp
	timers.cpp)

option(WITH_PROFILING "Profile various math functions. Does nothing at the moment." OFF)
option(WITH_PROFILING "Profile various math functions." OFF)
option(WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." ON)
option(HIDE_CONSOLE "Hide the console on Windows." OFF)
option(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE "Redirect the standard output streams to files." OFF)

configure_file(config.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp)

if (HIDE_CONSOLE)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(ENABLE_DEBUG_TRACE ON CACHE BOOL "Enable debug tracing via the renderer.")	
endif ()

add_library(gintonic ${src_files})
add_library(gintonic_static STATIC ${src_files})

# if (WIN32)
# 	set_target_properties(gintonic PROPERTIES OUTPUT_NAME gintonic)
# 	set_target_properties(gintonic_static PROPERTIES OUTPUT_NAME gintonic_static)
# else ()
# 	set_target_properties(gintonic PROPERTIES OUTPUT_NAME gintonic)
# 	set_target_properties(gintonic_static PROPERTIES OUTPUT_NAME gintonic)
# endif ()


if (WIN32)
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY_DEBUG} 
		${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(
		gintonic_static 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY_DEBUG} 
		${CMAKE_THREAD_LIBS_INIT})
else ()
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${JPEG_LIBRARIES} 
		${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY_DEBUG} 
		${CMAKE_THREAD_LIBS_INIT}
		-ldl)
	target_link_libraries(
		gintonic_static 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${JPEG_LIBRARIES} 
		${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY_DEBUG} 
		${CMAKE_THREAD_LIBS_INIT}
		-ldl)
endif ()

file(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
install(FILES ${header_files} DESTINATION include/gintonic)
install(TARGETS gintonic DESTINATION lib)
# install(TARGETS gintonic_static ARCHIVE DESTINATION lib)