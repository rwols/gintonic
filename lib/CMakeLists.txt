cmake_minimum_required(VERSION 2.8)

set(lua_src_dir "lua-5.3.1/src")

set(src_files
	${lua_src_dir}/lapi.c
	${lua_src_dir}/lauxlib.c
	${lua_src_dir}/lbaselib.c
	${lua_src_dir}/lbitlib.c
	${lua_src_dir}/lcode.c
	${lua_src_dir}/lcorolib.c
	${lua_src_dir}/lctype.c
	${lua_src_dir}/ldblib.c
	${lua_src_dir}/ldebug.c
	${lua_src_dir}/ldo.c
	${lua_src_dir}/ldump.c
	${lua_src_dir}/lfunc.c
	${lua_src_dir}/lgc.c
	${lua_src_dir}/linit.c
	${lua_src_dir}/liolib.c
	${lua_src_dir}/llex.c
	${lua_src_dir}/lmathlib.c
	${lua_src_dir}/lmem.c
	${lua_src_dir}/loadlib.c
	${lua_src_dir}/lobject.c
	${lua_src_dir}/lopcodes.c
	${lua_src_dir}/loslib.c
	${lua_src_dir}/lparser.c
	${lua_src_dir}/lstate.c
	${lua_src_dir}/lstring.c
	${lua_src_dir}/lstrlib.c
	${lua_src_dir}/ltable.c
	${lua_src_dir}/ltablib.c
	${lua_src_dir}/ltm.c
	${lua_src_dir}/lua.c
	${lua_src_dir}/lundump.c
	${lua_src_dir}/lutf8lib.c
	${lua_src_dir}/lvm.c
	${lua_src_dir}/lzio.c
	opengl/buffer_object.cpp
	opengl/framebuffer.cpp
	opengl/generic_shader_program.cpp
	opengl/glad.cpp
	opengl/shader_program.cpp
	opengl/source_code.cpp
	opengl/texture_object.cpp
	opengl/textures.cpp
	opengl/vertex_array_object.cpp
	opengl/utilities.cpp
	SQT.cpp
	basic_shapes.cpp
	box2f.cpp
	box3f.cpp
	camera.cpp
	component.cpp
	entity.cpp
	exception.cpp
	filesystem.cpp
	fonts.cpp
	gintonic.cpp
	lights.cpp
	locks.cpp
	luabindings.cpp
	mat2f.cpp
	mat3f.cpp
	mat4f.cpp
	materials.cpp
	mesh.cpp
	octree.cpp
	profiler.cpp
	proj_info.cpp
	quatf.cpp
	renderer.cpp
	shaders.cpp
	skybox.cpp
	static_mesh.cpp
	timers.cpp
	vec2f.cpp
	vec3f.cpp
	vec4f.cpp
	vertices.cpp)

find_package(Doxygen)

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
option(WITH_PROFILING "Profile various math functions. Does nothing at the moment." OFF)
option(WITH_PROFILING "Profile various math functions." OFF)
option(WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." ON)
option(HIDE_CONSOLE "Hide the console on Windows." OFF)
option(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE "Redirect the standard output streams to files." OFF)

if (BUILD_DOCUMENTATION)
	if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()
	configure_file(Doxyfile.cmake ${CMAKE_BINARY_DIR}/Doxyfile)
	add_custom_target(doc ALL
		COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating documentation."
		VERBATIM)
endif()


set(SSE_VERSION 30)
# set(FBX_NAMESPACE fbxsdk_2016_0)

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