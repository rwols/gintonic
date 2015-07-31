cmake_minimum_required(VERSION 2.8)

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

configure_file(config.hpp.in config.hpp)

add_library(gintonic STATIC ${src_files})
if (WIN32)
target_link_libraries(
	gintonic 
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
endif ()