cmake_minimum_required(VERSION 2.8)

set(executables math
	materials
	vec2)

add_definitions(-DBOOST_TEST_DYN_LINK) 

include_directories(${CMAKE_SOURCE_DIR}/lib)

foreach (exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	if (MSVC)
	    set_target_properties(${exe} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	    set_target_properties(${exe} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
	    set_target_properties(${exe} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
	    # etc for the other available configuration types (MinSizeRel, RelWithDebInfo)
	endif (MSVC)
	if (WIN32)
		target_link_libraries(
			${exe}
			gintonic 
			${Boost_LIBRARIES} 
			${OPENGL_LIBRARIES} 
			${SDL2_LIBRARY} 
			${FREETYPE_LIBRARIES} 
			${FBX_LIBRARY} 
			${CMAKE_THREAD_LIBS_INIT})
		# foreach (DLL ${DEBUG_DLLS})
		# 	get_filename_component(DLL_NAME ${DLL} NAME)
		# 	add_custom_command(TARGET ${exe} 
		# 		POST_BUILD
		# 		COMMAND ${CMAKE_COMMAND}
		# 		ARGS -E create_symlink ${DLL} "Debug\${DLL_NAME}")
		# endforeach ()
		# foreach (DLL ${RELEASE_DLLS})
		# 	get_filename_component(DLL_NAME ${DLL} NAME)
		# 	add_custom_command(TARGET ${exe}
		# 		POST_BUILD
		# 		COMMAND ${CMAKE_COMMAND}
		# 		ARGS -E create_symlink ${DLL} "Release\${DLL_NAME}")
		# endforeach()
	elseif (APPLE)
		target_link_libraries(
			${exe}
			gintonic 
			${Boost_LIBRARIES} 
			${OPENGL_LIBRARIES} 
			${SDL2_LIBRARY} 
			${JPEG_LIBRARIES} 
			${PNG_LIBRARIES} 
			${FREETYPE_LIBRARIES} 
			${FBX_LIBRARY_DEBUG} 
			${CMAKE_THREAD_LIBS_INIT})
	else ()
		target_link_libraries(
			${exe}
			gintonic 
			${Boost_LIBRARIES} 
			${OPENGL_LIBRARIES} 
			${SDL2_LIBRARY} 
			${JPEG_LIBRARIES} 
			${PNG_LIBRARIES} 
			${FREETYPE_LIBRARIES} 
			${FBX_LIBRARY_DEBUG} 
			${CMAKE_THREAD_LIBS_INIT}
			-ldl)
	endif ()
endforeach ()