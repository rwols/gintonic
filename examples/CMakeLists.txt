cmake_minimum_required(VERSION 2.8)

set(executables directional_light
	fbx_importer
	fps_camera
	geometry_buffer_showcase
	point_lights
	quadtree_console
	rotating_square
	square
	text
	textured_cube
	textured_cube_with_mod_color)

find_package(Boost REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..;${CMAKE_MODULE_PATH}")
if (APPLE OR WIN32)
	find_package(FBX REQUIRED)
else ()
	set(FBX_ROOT "/usr/local")
	set(FBX_INCLUDE_DIR "${FBX_ROOT}/include")
	set(FBX_LIBRARY "${FBX_ROOT}/lib/gcc4/x64/release/libfbxsdk.so")
	set(FBX_LIBRARY_DEBUG "${FBX_ROOT}/lib/gcc4/x64/debug/libfbxsdk.so")
endif ()

# get_filename_component(FBX_SYMLINK ${FBX_LIBRARY} NAME)

include_directories(
	${CMAKE_SOURCE_DIR}/lib
	${Boost_INCLUDE_DIR}
	${FBX_INCLUDE_DIR})

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	# if ( MSVC )
 #   	    set_target_properties( gintonic PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
 #    	set_target_properties( gintonic PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR} )
 #   		set_target_properties( gintonic PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR} )
	#     # etc for the other available configuration types (MinSizeRel, RelWithDebInfo)
	# endif ( MSVC )
	target_link_libraries(${exe} gintonic ${FBX_LIBRARY})
	# add_custom_command(TARGET ${exe} POST_BUILD
	# 	COMMAND ln -sf ${FBX_LIBRARY} $<TARGET_FILE_DIR:${exe}>)
endforeach ()