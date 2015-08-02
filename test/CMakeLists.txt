cmake_minimum_required(VERSION 2.8)

set(unit_tests math
	materials)

add_definitions(-std=c++11)
add_definitions(-Wall)
add_definitions(-msse3)
add_definitions(-DBOOST_TEST_DYN_LINK) 

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIRS})

foreach (unit_test ${unit_tests})
	add_executable(${unit_test} ${unit_test}.cpp)
	target_link_libraries(${unit_test} gintonic ${Boost_LIBRARIES})
endforeach ()