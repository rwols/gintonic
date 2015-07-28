cmake_minimum_required(VERSION 2.8)

project(app)
set(src_files main.cpp)
add_executable(app ${src_files})
target_link_gintonic(app)

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