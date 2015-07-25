cmake_minimum_required(VERSION 2.8)

set(project_name gintonic)
set(project_version v0.1)
set(WITH_PROFILING false) # Enable this for math functions profiling
set(WITH_MEMORY_PROFILING false) # Enable this for memory profiling
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_LIBS ON)

project(gintonic)
file(GLOB src_files "*.cpp")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp)

# # Find boost libraries
# find_package(Boost
# 	COMPONENTS
# 		program_options
# 		filesystem
# 		serialization
# 		system
# 	REQUIRED)

# find_package(OpenGL REQUIRED)
# set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
# find_package(Threads REQUIRED)
# find_package(JPEG REQUIRED)
# find_package(PNG REQUIRED)
# find_package(Freetype REQUIRED)

# INCLUDE(FindPkgConfig)
# pkg_search_module(SDL2 REQUIRED sdl2)

# set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")
# find_package(FBX REQUIRED)
# find_package(SDL2 REQUIRED)

# include_directories(${SDL2_INCLUDE_DIR})
# include_directories(${Boost_INCLUDE_DIR})
# include_directories(${JPEG_INCLUDE_DIR})
# include_directories(${PNG_INCLUDE_DIRS})
# include_directories(${FREETYPE_INCLUDE_DIRS})
# include_directories(${OPENGL_INCLUDE_DIRS})
# include_directories(${FBX_INCLUDE_DIR})

add_library(gintonic ${src_files})
target_link_libraries(
	gintonic 
	${Boost_LIBRARIES} 
	${OPENGL_LIBRARIES} 
	${SDL2_LIBRARY} 
	${JPEG_LIBRARIES} 
	${PNG_LIBRARIES} 
	${FREETYPE_LIBRARIES} 
	${FBX_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT}
	-ldl)

# add_definitions(-std=c++11)
# add_definitions(-Wall)
# add_definitions(-msse3)

# mark_as_advanced(FBX_INCLUDE_DIR)
# mark_as_advanced(FBX_LIBRARY)
# mark_as_advanced(FBX_LIBRARY_DEBUG)
# mark_as_advanced(SDL2MAIN_LIBRARY)
# mark_as_advanced(SDL2_INCLUDE_DIR)
# mark_as_advanced(SDL2_LIBRARY)
# mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
# mark_as_advanced(CMAKE_INSTALL_PREFIX)