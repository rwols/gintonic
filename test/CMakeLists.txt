cmake_minimum_required(VERSION 2.8)

set(unit_tests math
	materials)

foreach (unit_test ${unit_tests})
	add_executable(${unit_test} ${unit_test}.cpp)
	target_link_libraries(${unit_test} gintonic)
endforeach ()