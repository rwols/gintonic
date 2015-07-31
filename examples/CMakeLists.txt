cmake_minimum_required(VERSION 2.8)

set(examples directional_light
	fps_camera
	geometry_buffer_showcase
	point_lights
	rotating_square
	square
	text
	textured_cube
	textured_cube_with_mod_color)

foreach (example ${examples})
	add_executable(${example} ${example}.cpp)
	target_link_libraries(${example} gintonic)
endforeach ()