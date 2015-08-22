cmake_minimum_required(VERSION 2.8)

set(src_files
	opengl/buffer_object.cpp
	opengl/framebuffer.cpp
	opengl/glad.cpp
	opengl/shader_program.cpp
	opengl/source_code.cpp
	opengl/texture_object.cpp
	opengl/textures.cpp
	opengl/vertex_array_object.cpp
	SQT.cpp
	actors.cpp
	basic_shapes.cpp
	box2f.cpp
	box3f.cpp
	camera.cpp
	entity.cpp
	exception.cpp
	filesystem.cpp
	fonts.cpp
	gintonic.cpp
	lights.cpp
	locks.cpp
	mat2f.cpp
	mat3f.cpp
	mat4f.cpp
	materials.cpp
	mesh.cpp
	octree.cpp
	profiler.cpp
	quatf.cpp
	renderer.cpp
	shaders.cpp
	skybox.cpp
	timers.cpp
	vec2f.cpp
	vec3f.cpp
	vec4f.cpp
	vertices.cpp)

option(WITH_PROFILING "Profile various math functions. Does nothing at the moment." OFF)
option(WITH_PROFILING "Profile various math functions." OFF)
option(WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." ON)
option(HIDE_CONSOLE "Hide the console on Windows." OFF)
option(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE "Redirect the standard output streams to files." OFF)

set(SSE_VERSION 30)
set(FBX_NAMESPACE fbxsdk_2016_0)

configure_file(config.cmake ${CMAKE_BINARY_DIR}/config.hpp)

if (HIDE_CONSOLE)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(ENABLE_DEBUG_TRACE ON CACHE BOOL "Enable debug tracing via the renderer.")	
endif ()

add_subdirectory(opengl)

add_library(gintonic ${src_files})

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