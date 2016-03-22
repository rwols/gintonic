cmake_minimum_required(VERSION 2.8)

set(executables BasicShapes
	DirectionalLight
	FbxViewer
	HelloWorldText
	PointLights
	SpotLights
	TexturedCube)

include_directories(${CMAKE_SOURCE_DIR}/Source)

function(add_resource targetName resourceName)
	add_custom_command(TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/${resourceName}" "$<TARGET_FILE_DIR:${targetName}>/Resources")
endfunction(add_resource)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp Application.cpp)
	target_link_with_gintonic(${exe})
endforeach ()

add_resource(PointLights DaVinci.jpg)
add_resource(PointLights RuralBrickWall.jpg)
add_resource(PointLights bricks.jpg)
add_resource(PointLights bricks_SPEC.png)
add_resource(PointLights bricks_NRM.png)