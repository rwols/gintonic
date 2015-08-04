cmake_minimum_required(VERSION 2.8)

set(executables math
	materials
	vec2)

add_definitions(-std=c++11)
add_definitions(-Wall)
add_definitions(-msse3)
add_definitions(-DBOOST_TEST_DYN_LINK) 

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..;${CMAKE_MODULE_PATH}")
if (APPLE OR WIN32)
	find_package(FBX REQUIRED)
else ()
	set(FBX_ROOT "/usr/local")
	set(FBX_INCLUDE_DIR "${FBX_ROOT}/include")
	set(FBX_LIBRARY "${FBX_ROOT}/lib/gcc4/x64/release/libfbxsdk.so")
	set(FBX_LIBRARY_DEBUG "${FBX_ROOT}/lib/gcc4/x64/debug/libfbxsdk.so")
endif ()

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIRS} ${FBX_INCLUDE_DIR})

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_link_libraries(${exe} gintonic ${Boost_LIBRARIES} ${FBX_LIBRARY})
	add_custom_command(TARGET ${exe} POST_BUILD
		COMMAND ln -sf ${FBX_LIBRARY} $<TARGET_FILE_DIR:${exe}>)
endforeach ()