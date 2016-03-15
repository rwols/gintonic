cmake_minimum_required(VERSION 2.8)

set(executables basic_shapes
	directional_light
	fbx_viewer
	FbxViewer
	point_lights
	rotating_square
	skybox
	spot_lights
	text
	TexturedCube)

include_directories(${CMAKE_SOURCE_DIR}/lib)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_libraries(${exe} gintonic)
endforeach ()