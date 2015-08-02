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

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_definitions(-std=c++11)
add_definitions(-Wall)
add_definitions(-msse3)

foreach (example ${examples})
	add_executable(${example} ${example}.cpp)
	target_link_libraries(${example} gintonic)
endforeach ()