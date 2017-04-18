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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
if (NOT BUILD_SHARED_LIBS)
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost COMPONENTS system filesystem serialization REQUIRED)
find_package(FBX REQUIRED)

set(gintonic_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL 
	"The directory containing implementation files.")

set(gintonic_source_files

	# Foundation
	Foundation/Clock.cpp
	Foundation/ReadWriteLock.cpp
	Foundation/exception.cpp
	Foundation/Profiler.cpp
	Foundation/WriteLock.cpp
	Foundation/simd.cpp
	Foundation/filesystem.cpp
	Foundation/Octree.cpp

	# Graphics/OpenGL
	Graphics/OpenGL/BufferObject.cpp
	Graphics/OpenGL/ShaderProgram.cpp
	Graphics/OpenGL/VertexArrayObject.cpp
	Graphics/OpenGL/Vertices.cpp
	Graphics/OpenGL/utilities.cpp
	Graphics/OpenGL/TextureObject.cpp
	Graphics/OpenGL/SourceCode.cpp
	Graphics/OpenGL/Framebuffer.cpp

	# Graphics
	Graphics/PointShadowBuffer.cpp
	Graphics/skybox.cpp
	Graphics/AnimationClip.cpp
	Graphics/Skeleton.cpp
	Graphics/AmbientLight.cpp
	Graphics/Renderer.cpp
	Graphics/GUI/Base.cpp
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

	# Math
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

	# ???
	Application.cpp
	ApplicationStateMachine.cpp
	Asset.cpp
	Behaviour.cpp
	BoxCollider.cpp
	Camera.cpp
	Collider.cpp
	Component.cpp
	Component.cpp
	Entity.cpp
	EntityBase.cpp
	EntityVisitor.cpp
	FbxImporter.cpp
	gintonic.cpp
	MeshRenderer.cpp
	Prefab.cpp
	Renderer.cpp
	RenderStrategy.cpp
	RunLoop.cpp
	Scene.cpp
	SDLRenderContext.cpp
	SDLRunLoop.cpp
	SDLWindow.cpp
	Transform.cpp
	Window.cpp

	# imgui
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	)

set(gintonic_SSE_VERSION 30 CACHE STRING "The SSE version.")
option(gintonic_WITH_PROFILING "Profile various math functions." OFF)
option(gintonic_WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	option(gintonic_ENABLE_DEBUG_TRACE 
		"Enable debug tracing via the renderer." ON)
else ()
	option(gintonic_ENABLE_DEBUG_TRACE 
		"Enable debug tracing via the renderer." OFF)
endif ()
option(gintonic_HIDE_CONSOLE "Hide the console on Windows." ON)
option(gintonic_REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE 
	"Redirect the standard output to the renderer's error stream." 
	${gintonic_ENABLE_DEBUG_TRACE})
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
	if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL Debug)
		target_compile_options(gintonic PUBLIC /MDd)
	else()
		target_compile_options(gintonic PUBLIC /MD)
	endif()
else ()
	target_compile_definitions(gintonic PUBLIC LUA_USE_POSIX)
	target_compile_options(gintonic PUBLIC -msse3)
	target_compile_options(gintonic PRIVATE -Wall)
	target_compile_options(gintonic PRIVATE -fvisibility=default)
	target_compile_options(gintonic PRIVATE -fvisibility-inlines-hidden)
	if (CMAKE_STRIP AND CMAKE_BUILD_TYPE STREQUAL Release)
		if (APPLE)
			add_custom_command(TARGET gintonic POST_BUILD COMMAND ${CMAKE_STRIP} -x $<TARGET_FILE:gintonic>)
		else ()
			add_custom_command(TARGET gintonic POST_BUILD COMMAND ${CMAKE_STRIP} -u -r $<TARGET_FILE:gintonic>)
		endif()
	endif ()
	if (gintonic_USE_LIBCXX)
		target_compile_options(gintonic PUBLIC -stdlib=libc++)
	endif ()
endif ()

set_target_properties(gintonic PROPERTIES CXX_STANDARD 14)

if (WIN32 AND gintonic_HIDE_CONSOLE)
	set_target_properties(gintonic PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

# Configure glad
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	seT(GLAD_GENERATOR "c-debug" CACHE STRING "")
endif ()
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
if(WIN32)
	set(LIBC ON CACHE BOOL "")
endif()

add_subdirectory(SDL-mirror)

# Configure freetype
add_subdirectory(freetype2)

# Configure pybind11
add_subdirectory(pybind11)

# Configure cxxopts
add_subdirectory(cxxopts)

# Configure cereal
add_subdirectory(cereal)

target_include_directories(gintonic PUBLIC SYSTEM
	${CMAKE_CURRENT_BINARY_DIR}/glad/include # Generated by glad
	freetype2/include
	${Boost_INCLUDE_DIR}
	SDL-mirror/include
	imgui # For including imgui.h
	)

target_include_directories(gintonic PUBLIC
	${gintonic_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # For the config file
	)

target_link_libraries(gintonic PUBLIC
	FBX
	${Boost_LIBRARIES}
	${SDL2_LIBRARY}
	freetype
	glad
	cxxopts
	cereal
	)

install(TARGETS gintonic DESTINATION lib)

add_executable(maintest main.cpp)
target_link_libraries(maintest PRIVATE gintonic)
set_target_properties(maintest PROPERTIES CXX_STANDARD 14)

