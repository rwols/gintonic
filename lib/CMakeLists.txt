#*******************************************************************************
# gintonic/lib
#
# The purpose of this cmake file is to create the "gintonic" library target. It
# attempts to find the packages "OpenGL", "Boost" and "FBX". Every modern OS has
# OpenGL, so that will probably pose no problems. However Boost and FBX require
# manual installation of the user. If you are on Linux, the Boost libraries are
# probably in your package manager. For instance, on Ubuntu you can install
# Boost with the command `apt get install libboost-dev`. For FBX, you will need
# to go to the Autodesk website and install FBX manually from there.
# Other dependencies include SDL, freetype, glad and pybind11, but all those are
# included as git submodules, so nothing has to be done and they will
# "just work".
#
# Important variables that this cmake file defines are:
#
# - gintonic_LIB_DIR -- This directory (containing the implementation files)
# - gintonic_SSE_VERSION -- The SSE target (as a string) to compile against
# - gintonic_WITH_PROFILING -- Profile various math functions
# - gintonic_WITH_MEMORY_PROFILING -- Profile memory allocations
# - gintonic_ENABLE_DEBUG_TRACE -- Enable debug tracing via the Renderer
# - gintonic_HIDE_CONSOLE -- Hide the console (only applicable to Windows)
# - gintonic_REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE -- When the console is hidden
#     on Windows, wether to redirect standard output streams to files.
# - gintonic_USE_LIBCXX -- Wether to use Clang's libc++ library. Only applicable
#     when the compiler is Clang.
#
#*******************************************************************************

set(CMAKE_MODULE_PATH 
	"${CMAKE_CURRENT_LIST_DIR}/cmake/modules;${CMAKE_MODULE_PATH}")
if (NOT BUILD_SHARED_LIBS)
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS system filesystem serialization REQUIRED)
find_package(FBX REQUIRED)

set(gintonic_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL 
	"The directory containing implementation files.")

set(gintonic_source_files
	Graphics/OpenGL/BufferObject.cpp
	Graphics/OpenGL/ShaderProgram.cpp
	Graphics/OpenGL/VertexArrayObject.cpp
	Graphics/OpenGL/Vertices.cpp
	Graphics/OpenGL/utilities.cpp
	Graphics/OpenGL/TextureObject.cpp
	Graphics/OpenGL/SourceCode.cpp
	Graphics/OpenGL/Framebuffer.cpp
	Graphics/PointShadowBuffer.cpp
	Graphics/skybox.cpp
	Graphics/AnimationClip.cpp
	Graphics/Skeleton.cpp
	Graphics/AmbientLight.cpp
	Graphics/Renderer.cpp
	Graphics/GUI/Base.cpp
	Graphics/GUI/ConsoleView.cpp
	Graphics/GUI/Panel.cpp
	Graphics/GUI/StringView.cpp
	Graphics/GUI/StringPointerView.cpp
	Graphics/Texture2D.cpp
	Graphics/Light.cpp
	Graphics/PointLight.cpp
	Graphics/SpotLight.cpp
	Graphics/Material.cpp
	Graphics/Mesh.cpp
	Graphics/GeometryBuffer.cpp
	Graphics/SpotShadowBuffer.cpp
	Graphics/DirectionalLight.cpp
	Graphics/Font.cpp
	Graphics/ShaderPrograms.cpp
	Graphics/DirectionalShadowBuffer.cpp
	EntityVisitor.cpp
	luabindings.cpp
	Foundation/ReadWriteLock.cpp
	Foundation/exception.cpp
	Foundation/Profiler.cpp
	Foundation/WriteLock.cpp
	Foundation/simd.cpp
	Foundation/Console.cpp
	Foundation/filesystem.cpp
	Foundation/Octree.cpp
	Math/vec3f.cpp
	Math/mat3f.cpp
	Math/mat2f.cpp
	Math/vec2f.cpp
	Math/quatf.cpp
	Math/box2f.cpp
	Math/mat4f.cpp
	Math/SQT.cpp
	Math/vec4f.cpp
	Math/box3f.cpp
	Camera.cpp
	FbxImporter.cpp
	gintonic.cpp
	Entity.cpp
	)

set(lua_src_dir lua-5.3.1/src)
file(GLOB lua_source_files "${lua_src_dir}/*.c")
list(REMOVE_ITEM lua_source_files 
	"${CMAKE_CURRENT_SOURCE_DIR}/${lua_src_dir}/luac.c")
set_source_files_properties(${lua_source_files} PROPERTIES LANGUAGE CXX)

# file(GLOB_RECURSE gintonic_source_files "*.cpp")
option(gintonic_SSE_VERSION "The SSE version." 30)
option(gintonic_WITH_PROFILING "Profile various math functions." OFF)
option(gintonic_WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	option(gintonic_ENABLE_DEBUG_TRACE 
		"Enable debug tracing via the renderer." ON)
else ()
	option(gintonic_ENABLE_DEBUG_TRACE 
		"Enable debug tracing via the renderer." OFF)
endif ()
option(gintonic_HIDE_CONSOLE "Hide the console on Windows." ON)
option(gintonic_REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE 
	"Redirect the standard output to the renderer's error stream." 
	${gintonic_ENABLE_DEBUG_TRACE}
option(gintonic_USE_LIBCXX "Use libc++ instead of stdlib." OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(gintonic_ENABLE_DEBUG_TRACE ON CACHE BOOL 
		"Enable debug tracing via the renderer.")	
endif ()
configure_file(cmake/config.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)
add_library(gintonic ${gintonic_source_files} ${lua_source_files})
if (WIN32)
	target_compile_definitions(gintonic PUBLIC _SCL_SECURE_NO_WARNINGS)
	target_compile_definitions(gintonic PUBLIC NOMINMAX)
	target_compile_definitions(gintonic PUBLIC _USE_MATH_DEFINES)
	target_compile_definitions(gintonic PUBLIC BOOST_ALL_NO_LIB)
	target_compile_options(gintonic PUBLIC /arch:SSE)
	target_compile_options(gintonic PRIVATE /MP)
else ()
	target_compile_definitions(gintonic PUBLIC LUA_USE_POSIX)
	target_compile_options(gintonic PUBLIC -msse3)
	target_compile_options(gintonic PRIVATE -Wall)
	if (gintonic_USE_LIBCXX)
		target_compile_options(gintonic PUBLIC -stdlib=libc++)
	endif ()
endif ()

set_target_properties(gintonic PROPERTIES CXX_STANDARD 14)


if (WIN32 AND gintonic_HIDE_CONSOLE)
	set_target_properties(gintonic PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

target_include_directories(gintonic PUBLIC
	../include
	${CMAKE_CURRENT_BINARY_DIR}/glad/include # Generated by glad
	# ${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	# ${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # For the config file
	../include/${lua_src_dir} # For Lua
	)

# Configure glad
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile" FORCE)
add_subdirectory(glad)

# Configure SDL2
if (BUILD_SHARED_LIBS)
	# Sigh... SDL should just respect BUILD_SHARED_LIBS
	set(SDL_SHARED_ENABLED_BY_DEFAULT ON CACHE BOOL 
		"Build a shared version of the library" FORCE)
	set(SDL_STATIC OFF CACHE BOOL "Build a static version of the library" FORCE)
	set(SDL2_LIBRARY SDL2)
else()
	set(SDL_SHARED_ENABLED_BY_DEFAULT OFF CACHE BOOL 
		"Build a shared version of the library" FORCE)
	set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
	set(SDL2_LIBRARY SDL2-static)
endif()
set(SDL_ATOMIC OFF CACHE BOOL "We don't need atomics" FORCE)
set(SDL_FILESYSTEM OFF CACHE BOOL "We don't need a filesystem" FORCE)
set(SDL_THREADS OFF CACHE BOOL "We don't need threads" FORCE)
set(SDL_TIMERS OFF CACHE BOOL "We don't need timers" FORCE)
set(SDL_FILE OFF CACHE BOOL "Don't need file support" FORCE)
set(SDL_LOADSO OFF CACHE BOOL "We don't need to dynamically load libs" FORCE)
add_subdirectory(SDL-mirror)
target_include_directories(gintonic PRIVATE ${SDL2_SOURCE_DIR}/include)

# Configure freetype
add_subdirectory(freetype2)
target_include_directories(gintonic PRIVATE freetype2/include)

# Configure pybind11
add_subdirectory(pybind11)

target_link_libraries(gintonic PUBLIC
	${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${SDL2_LIBRARY}
	freetype
	${FBX_LIBRARY}
	${CMAKE_DL_LIBS}
	glad
	SDL2main
	${SDL2_LIBRARY})

install(TARGETS gintonic DESTINATION lib)
