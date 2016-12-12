cmake_minimum_required(VERSION 2.8)

add_definitions(-DBOOST_TEST_DYN_LINK)
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
file(GLOB source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach (source_file ${source_files})
	get_filename_component(exe ${source_file} NAME_WE)
	add_executable(${exe} ${exe}.cpp)
	set_target_properties(${exe} PROPERTIES CXX_STANDARD 14)
	target_include_directories(${exe} PUBLIC ${Boost_INCLUDE_DIR})
	target_link_libraries(${exe} PUBLIC gintonic ${Boost_LIBRARIES})
	add_test(NAME ${exe} COMMAND ${exe})
	list(APPEND executables ${exe})
endforeach ()

# Need at least one executable in this directory.
list(GET executables 0 exe)

# get_filename_component(post_build_tests ${CMAKE_CURRENT_LIST_DIR} NAME_WE)

add_custom_target(post_build_tests ALL
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_SYSTEM_DLL} $<TARGET_FILE_DIR:${exe}>
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_FILESYSTEM_DLL} $<TARGET_FILE_DIR:${exe}> 
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_SERIALIZATION_DLL} $<TARGET_FILE_DIR:${exe}> 
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_PROGRAM_OPTIONS_DLL} $<TARGET_FILE_DIR:${exe}>
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BOOST_UNIT_TEST_FRAMEWORK_DLL} $<TARGET_FILE_DIR:${exe}> 
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FBX_DLL} $<TARGET_FILE_DIR:${exe}>
	# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_DLL} $<TARGET_FILE_DIR:${exe}>
	COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Resources/Inconsolata-Regular.ttf $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/lib/Graphics/Shaders $<TARGET_FILE_DIR:${exe}>/Shaders	
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Examples/bricks.jpg $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Examples/bricks_SPEC.png $<TARGET_FILE_DIR:${exe}>/Resources
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Examples/bricks_NRM.png $<TARGET_FILE_DIR:${exe}>/Resources
	DEPENDS ${executables}
	COMMENT "Performing post-build processing for ${targetname}")