cmake_minimum_required(VERSION 2.8)

project(math)
add_executable(math math.cpp)
target_link_no_lib(math)

project(materials)
add_executable(materials materials.cpp)
target_link_gintonic(materials)