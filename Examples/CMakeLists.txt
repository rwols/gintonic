cmake_minimum_required(VERSION 2.8)

set(executables BasicShapes
	directional_light
	fbx_viewer
	FbxViewer
	StreamBuffers
	point_lights
	rotating_square
	skybox
	spot_lights
	text
	TexturedCube)

include_directories(${CMAKE_SOURCE_DIR}/Source)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_libraries(${exe} gintonic)
endforeach ()