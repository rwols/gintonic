cmake_minimum_required(VERSION 2.8)

set(executables
	DriverInfo
	FbxConverter
	LuaTest)

include_directories(${CMAKE_SOURCE_DIR}/Source)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_with_gintonic(${exe})
endforeach ()