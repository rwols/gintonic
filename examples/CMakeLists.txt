cmake_minimum_required(VERSION 2.8)

project(square)
add_executable(square square.cpp)
target_link_gintonic(square)

project(rotating_square)
add_executable(rotating_square rotating_square.cpp)
target_link_gintonic(rotating_square)

project(text)
add_executable(text text.cpp)
target_link_gintonic(text)

project(fps_camera)
add_executable(fps_camera fps_camera.cpp)
target_link_gintonic(fps_camera)

project(textured_cube)
add_executable(textured_cube textured_cube.cpp)
target_link_gintonic(textured_cube)

project(textured_cube_with_mod_color)
add_executable(textured_cube_with_mod_color textured_cube_with_mod_color.cpp)
target_link_gintonic(textured_cube_with_mod_color)

project(serialize_materials)
add_executable(serialize_materials serialize_materials.cpp)
target_link_gintonic(serialize_materials)

project(geometry_buffer_showcase)
add_executable(geometry_buffer_showcase geometry_buffer_showcase.cpp)
target_link_gintonic(geometry_buffer_showcase)

project(directional_light)
add_executable(directional_light directional_light.cpp)
target_link_gintonic(directional_light)

project(point_lights)
add_executable(point_lights point_lights.cpp)
target_link_gintonic(point_lights)