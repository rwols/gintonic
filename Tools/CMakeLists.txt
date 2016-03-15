cmake_minimum_required(VERSION 2.8)

set(executables
	driver_info
	fbx_converter
	luatest)

include_directories(${CMAKE_SOURCE_DIR}/lib)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_libraries(${exe} gintonic)
endforeach ()