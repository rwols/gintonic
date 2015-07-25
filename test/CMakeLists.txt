cmake_minimum_required(VERSION 2.8)

project(test)
file(GLOB src_files "*.cpp")

add_executable(test ${src_files})
target_link_gintonic(test)