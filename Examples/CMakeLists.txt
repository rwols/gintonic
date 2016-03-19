cmake_minimum_required(VERSION 2.8)

set(executables BasicShapes
	DirectionalLight
	FbxViewer
	HelloWorldText
	StreamBuffers
	PointLights
	SpotLights
	TexturedCube)

include_directories(${CMAKE_SOURCE_DIR}/Source)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp Application.cpp)
	target_link_libraries(${exe} gintonic)
endforeach ()