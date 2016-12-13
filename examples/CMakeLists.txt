cmake_minimum_required(VERSION 2.8)

file(GLOB source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(Application Common/Application.cpp)
set_target_properties(Application PROPERTIES CXX_STANDARD 14)
target_link_libraries(Application gintonic)

foreach (source_file ${source_files})
	get_filename_component(exe ${source_file} NAME_WE)
	add_executable(${exe} ${exe}.cpp)
	set_target_properties(${exe} PROPERTIES CXX_STANDARD 14)
	target_link_libraries(${exe} Application)
	list(APPEND executables ${exe})
endforeach ()

list(GET executables 0 exe)

get_filename_component(targetname ${CMAKE_CURRENT_LIST_DIR} NAME_WE)

add_custom_target(${targetname} ALL
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_SYSTEM_DLL} $<TARGET_FILE_DIR:${exe}>
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_FILESYSTEM_DLL} $<TARGET_FILE_DIR:${exe}> 
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_SERIALIZATION_DLL} $<TARGET_FILE_DIR:${exe}> 
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_PROGRAM_OPTIONS_DLL} $<TARGET_FILE_DIR:${exe}>
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_UNIT_TEST_FRAMEWORK_DLL} $<TARGET_FILE_DIR:${exe}> 
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FBX_DLL} $<TARGET_FILE_DIR:${exe}>
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_DLL} $<TARGET_FILE_DIR:${exe}>
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${GINTONIC_SHADER_DIR} $<TARGET_FILE_DIR:${exe}>/Shaders	
	COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GINTONIC_ASSET_DIR}/Inconsolata-Regular.ttf $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different DaVinci.jpg        $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different RuralBrickWall.jpg $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different RuralBrickWall_SPEC.png $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different RuralBrickWall_NRM.png $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different bricks.jpg         $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different bricks_SPEC.png    $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different bricks_NRM.png     $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different DaVinci.jpg        $<TARGET_FILE_DIR:${exe}>/Resources
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	DEPENDS ${executables}
	COMMENT "Performing post-build processing for ${targetname}" VERBATIM)