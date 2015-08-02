cmake_minimum_required(VERSION 2.8)

set(examples directional_light
	fbx_importer
	fps_camera
	geometry_buffer_showcase
	point_lights
	quadtree_console
	rotating_square
	square
	text
	textured_cube
	textured_cube_with_mod_color)

find_package(Boost REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..;${CMAKE_MODULE_PATH}")
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
	${CMAKE_SOURCE_DIR}/lib
	${Boost_INCLUDE_DIR}
	${FBX_INCLUDE_DIR})

foreach (example ${examples})
	add_executable(${example} ${example}.cpp)
	target_link_libraries(${example} gintonic_static ${FBX_LIBRARY_DEBUG})
endforeach ()