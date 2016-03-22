cmake_minimum_required(VERSION 2.8)

set(executables 
	SQT
	box2f
	mat2f
	mat3f
	mat4f
	materials
	quatf
	vec2f
	vec3f
	vec4f)

add_definitions(-DBOOST_TEST_DYN_LINK) 

include_directories(${CMAKE_SOURCE_DIR}/lib)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_with_gintonic(${exe})
endforeach ()