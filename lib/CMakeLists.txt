cmake_minimum_required(VERSION 2.8)

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
if (WIN32)
	# do nothing
else ()
	find_package(JPEG REQUIRED)
	find_package(PNG REQUIRED)
endif ()

find_package(Freetype REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..;${CMAKE_MODULE_PATH}")
find_package(FBX REQUIRED)
find_package(SDL2 REQUIRED)
# if (APPLE OR WIN32)
# 	find_package(FBX REQUIRED)
# else ()
# 	set(FBX_ROOT "/usr/local")
# 	set(FBX_INCLUDE_DIR "${FBX_ROOT}/include")
# 	set(FBX_LIBRARY "${FBX_ROOT}/lib/gcc4/x64/release/libfbxsdk.so")
# 	set(FBX_LIBRARY_DEBUG "${FBX_ROOT}/lib/gcc4/x64/debug/libfbxsdk.so")
# endif ()

include_directories(
	${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR})

if (WIN32)
	set(found_correct_library OFF)
	set(boost_lib_token "optimized")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(boost_lib_token "debug")
	endif ()
	foreach (list_item IN LISTS Boost_LIBRARIES)
		if (list_item STREQUAL boost_lib_token)
			set(found_correct_library ON)
		elseif (found_correct_library)
			string(REPLACE ".lib" ".dll" list_item ${list_item})
			list(APPEND Boost_DLLS ${list_item})
			set(found_correct_library OFF)
		endif ()
	endforeach()
	list(GET SDL2_LIBRARY 1 SDL2_DLL)
	string(REPLACE ".lib" ".dll" SDL2_DLL ${SDL2_DLL})
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
	filesystem.cpp
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
	timers.cpp
	vec2.cpp)

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

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# set(CMAKE_MACOSX_RPATH ON)
# add_library(gintonic SHARED ${src_files})

add_library(gintonic ${src_files})

# if ( MSVC )
#     	set_target_properties( gintonic PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
#     	set_target_properties( gintonic PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR} )
#    		set_target_properties( gintonic PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR} )
#     # etc for the other available configuration types (MinSizeRel, RelWithDebInfo)
# endif ( MSVC )

# add_library(gintonic_static STATIC ${src_files})

if (WIN32)
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY} 
		${CMAKE_THREAD_LIBS_INIT})
	# foreach (DLL IN LISTS Boost_DLLS)
	# 	add_custom_command(
	# 		TARGET gintonic 
	# 		POST_BUILD
	# 		COMMAND ln -sf ${DLL} $<TARGET_FILE_DIR:gintonic>)
	# endforeach()
	# add_custom_command(
	# 	TARGET gintonic
	# 	POST_BUILD
	# 	COMMAND ln -sf ${SDL2_DLL} $<TARGET_FILE_DIR:gintonic>)
elseif (APPLE)
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${JPEG_LIBRARIES} 
		${PNG_LIBRARIES} 
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
endif ()

file(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
install(FILES ${header_files} DESTINATION include/gintonic)
install(TARGETS gintonic DESTINATION lib)