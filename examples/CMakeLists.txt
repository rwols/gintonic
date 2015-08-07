cmake_minimum_required(VERSION 2.8)

set(executables directional_light
	fbx_importer
	fps_camera
	geometry_buffer_showcase
	point_lights
	quadtree_console
	rotating_square
	spot_lights
	square
	text
	textured_cube
	textured_cube_with_mod_color)

include_directories(${CMAKE_SOURCE_DIR}/lib)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_libraries(${exe} gintonic)
endforeach ()