cmake_minimum_required(VERSION 2.8)

set(executables math
	materials
	vec2)

add_definitions(-DBOOST_TEST_DYN_LINK) 

include_directories(${CMAKE_SOURCE_DIR}/lib)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_libraries(${exe} gintonic)
endforeach ()