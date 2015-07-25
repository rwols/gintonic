cmake_minimum_required (VERSION 2.8)
# project (all)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_definitions(-std=c++11)
add_definitions(-Wall)
add_definitions(-msse3)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
include_directories("include")
# Find boost libraries
find_package(Boost
	COMPONENTS
		unit_test_framework
		program_options
		filesystem
		serialization
		system
	REQUIRED)
find_package(OpenGL REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")
# find_package(FBX REQUIRED)
set(FBX_ROOT "/usr/local")
set(FBX_INCLUDE_DIRS "${FBX_ROOT}/include")
set(FBX_LIBRARY "${FBX_ROOT}/lib/gcc4/x64/release/libfbxsdk.so")
set(FBX_LIBRARY_DEBUG "${FBX_ROOT}/lib/gcc4/x64/debug/libfbxsdk.so")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${FBX_INCLUDE_DIR})
# target_link_libraries(
# 	${targetname} 
# 	gintonic
# 	${Boost_LIBRARIES} 
# 	${OPENGL_LIBRARIES} 
# 	${SDL2_LIBRARY} 
# 	${JPEG_LIBRARIES} 
# 	${PNG_LIBRARIES} 
# 	${FREETYPE_LIBRARIES} 
# 	${FBX_LIBRARY} 
# 	${CMAKE_THREAD_LIBS_INIT}
# 	-ldl)
mark_as_advanced(FBX_INCLUDE_DIR)
mark_as_advanced(FBX_LIBRARY)
mark_as_advanced(FBX_LIBRARY_DEBUG)
mark_as_advanced(SDL2MAIN_LIBRARY)
mark_as_advanced(SDL2_INCLUDE_DIR)
mark_as_advanced(SDL2_LIBRARY)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
mark_as_advanced(CMAKE_INSTALL_PREFIX)
macro(target_link_gintonic targetname)
	target_link_libraries(
		${targetname} 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${JPEG_LIBRARIES} 
		${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES} 
		${FBX_LIBRARY} 
		${CMAKE_THREAD_LIBS_INIT}
		-ldl
		gintonic)
endmacro(target_link_gintonic)
add_subdirectory (lib)
add_subdirectory (examples)
add_subdirectory (test)