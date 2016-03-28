cmake_minimum_required (VERSION 2.8)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-source build. \
  	Please create a build directory outside of the \
  	Gintonic source code and call CMake from there.")
endif()

set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type. Can be Debug or Release or whatnot")

set(SSE_VERSION 30 CACHE STRING "The SSE version.")

set(shader_folder "${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics/Shaders/")

set(lua_src_dir "lua-5.3.1/src")
set(source_directory "${CMAKE_CURRENT_SOURCE_DIR}/Source")
file(GLOB_RECURSE gintonic_source_files "${source_directory}/*.cpp")
file(GLOB lua_source_files "${source_directory}/${lua_src_dir}/*.c")
file(GLOB shader_source_files "${source_directory}/Graphics/Shaders/*.vert" "${source_directory}/Graphics/Shaders/*.frag")
set(src_files ${gintonic_source_files} ${lua_source_files})

project(gintonic C CXX)

enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")

find_package(Boost
	COMPONENTS
		system
		filesystem
		serialization
		program_options
		unit_test_framework
	REQUIRED)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	set (BOOST_SYSTEM_DLL ${Boost_SYSTEM_LIBRARY_DEBUG})
	set (BOOST_FILESYSTEM_DLL ${Boost_FILESYSTEM_LIBRARY_DEBUG})
	set (BOOST_SERIALIZATION_DLL ${Boost_SERIALIZATION_LIBRARY_DEBUG})
	set (BOOST_PROGRAM_OPTIONS_DLL ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})
	set (BOOST_UNIT_TEST_FRAMEWORK_DLL ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG})
else ()
	set (BOOST_SYSTEM_DLL ${Boost_SYSTEM_LIBRARY_RELEASE})
	set (BOOST_FILESYSTEM_DLL ${Boost_FILESYSTEM_LIBRARY_RELEASE})
	set (BOOST_SERIALIZATION_DLL ${Boost_SERIALIZATION_LIBRARY_RELEASE})
	set (BOOST_PROGRAM_OPTIONS_DLL ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
	set (BOOST_UNIT_TEST_FRAMEWORK_DLL ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE})
endif ()

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
find_package(FBX REQUIRED)
find_package(SDL2 REQUIRED)

find_package(Doxygen)

if (NOT DOXYGEN_FOUND)
	message(WARNING "Doxygen is not present on this system. Documentation cannot be built.")
else ()
	option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
endif ()

option(WITH_PROFILING "Profile various math functions." OFF)
option(WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." ON)
else ()
	option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." OFF)
endif ()
option(HIDE_CONSOLE "Hide the console on Windows." ON)
option(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE "Redirect the standard output to the renderer's error stream." ${ENABLE_DEBUG_TRACE})

configure_file(Source/config.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)
add_library(gintonic ${src_files})
if (WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_WARNINGS /DNOMINMAX /D_USE_MATH_DEFINES /DBOOST_ALL_NO_LIB /arch:SSE")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -msse3")
endif ()
set_target_properties(gintonic PROPERTIES INSTALL_RPATH "")

if (WIN32 AND HIDE_CONSOLE)
	set_target_properties(gintonic PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(ENABLE_DEBUG_TRACE ON CACHE BOOL "Enable debug tracing via the renderer.")	
endif ()

if (BUILD_DOCUMENTATION)
	configure_file(Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target(doc ALL
		COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating documentation." VERBATIM)
endif()

include_directories(
	${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # For the config file
	"Source/lua-5.3.1/src" # For Lua
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
	# string(REPLACE ".lib" ".dll" FBX_DLL ${FBX_LIBRARY})

	set (DEBUG_DLLS ${Boost_DEBUG_DLLS} ${SDL2_DLL} ${FBX_DLL})
	set (RELEASE_DLLS ${Boost_DLLS} ${SDL2_DLL} ${FBX_DLL})

	if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
		set (DLLS ${DEBUG_DLLS})
	elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
		set (DLLS ${RELEASE_DLLS})
	else ()
		message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is an unknown build type for me.")
	endif ()

else ()
	include_directories(
		${JPEG_INCLUDE_DIR}
		${PNG_INCLUDE_DIRS})

	# if (APPLE)
	# 	# Need to copy fbxsdk
	# 	list(GET FBX_LIBRARY 0 FBX_DLL)
	# 	file (COPY ${FBX_DLL} DESTINATION ${CMAKE_BINARY_DIR}/examples)
	# 	file (COPY ${FBX_DLL} DESTINATION ${CMAKE_BINARY_DIR}/test)
	# endif ()
endif ()

if (WIN32)
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${FREETYPE_LIBRARIES}
		${FBX_LIBRARY} 
		${CMAKE_THREAD_LIBS_INIT})
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

# function(copy_dlls targetName dlls)
# 	add_custom_command(TARGET ${targetName} POST_BUILD
# 	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dlls} $<TARGET_FILE_DIR:${targetName}>
# 	COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${targetName}>/Resources
# 	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics/Shaders $<TARGET_FILE_DIR:${targetName}>/Shaders
# 	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Resources/Inconsolata-Regular.ttf $<TARGET_FILE_DIR:${targetName}>/Resources
# 	COMMENT "Post-build processing for ${targetName}" VERBATIM)
# endfunction()

list(GET FBX_LIBRARY 0 FBX_DLL)

function(target_link_with_gintonic targetName)

	target_link_libraries(${targetName} gintonic)

	# add_custom_command(TARGET ${targetName} POST_BUILD
	# 	COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${targetName}>/Resources
	# 	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Resources/Inconsolata-Regular.ttf $<TARGET_FILE_DIR:${targetName}>/Resources
	# 	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics/Shaders $<TARGET_FILE_DIR:${targetName}>/Shaders
	# 	COMMENT "Performing post-build copying stuff around for ${targetName}" VERBATIM)

	# add_custom_command(TARGET ${targetName} POST_BUILD
	# 	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	# 	${shader_source_files} "$<TARGET_FILE_DIR:${targetName}>/Shaders")

	# if (APPLE)
	# 	list(GET FBX_LIBRARY 0 FBX_DLL)
	# 	add_custom_command(TARGET ${targetName} POST_BUILD
	# 		COMMAND ${CMAKE_COMMAND} -E copy_if_different
	# 		${FBX_DLL} $<TARGET_FILE_DIR:${targetName}>)
	# endif ()

	# if (WIN32)

	# 	add_custom_command(TARGET ${targetName} POST_BUILD
	# 		COMMAND ${CMAKE_COMMAND} -E copy_if_different
	# 		${DLLS} $<TARGET_FILE_DIR:${targetName}>)

	# endif ()

	# add_custom_command(TARGET ${targetName} POST_BUILD
	# 	)

endfunction(target_link_with_gintonic)

# mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
# mark_as_advanced(CMAKE_INSTALL_PREFIX)

add_subdirectory (Tools)
add_subdirectory (Examples)
add_subdirectory (UnitTest)
