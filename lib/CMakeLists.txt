cmake_minimum_required(VERSION 2.8)

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
	skybox.cpp
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

add_library(gintonic ${src_files})

# foreach (DLL ${DEBUG_DLLS})
# 	get_filename_component(DLL_NAME ${DLL} NAME)
# 	add_custom_command(TARGET gintonic 
# 		POST_BUILD
# 		COMMAND ${CMAKE_COMMAND}
# 		ARGS -E create_symlink ${DLL} "Debug\${DLL_NAME}")
# endforeach ()
# foreach (DLL ${RELEASE_DLLS})
# 	get_filename_component(DLL_NAME ${DLL} NAME)
# 	add_custom_command(TARGET gintonic
# 		POST_BUILD
# 		COMMAND ${CMAKE_COMMAND}
# 		ARGS -E create_symlink ${DLL} "Release\${DLL_NAME}")
# endforeach()

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

file(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
install(FILES ${header_files} DESTINATION include/gintonic)
install(TARGETS gintonic DESTINATION lib)