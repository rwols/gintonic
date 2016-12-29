#*******************************************************************************
# gintonic/tools
#
# The purpose of this cmake file is to define various tool executables. All the 
# executables that are defined here are linked with the "gintonic" library 
# target. Some tools require assets from the asset  directory. That is taken 
# care of by the function "gintonic_target_depends_on_assets". See the cmake 
# file in gintonic/Resources for the definition of that function.
#
#*******************************************************************************

file(GLOB directories
	LIST_DIRECTORIES ON
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	*)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

foreach (directory ${directories})
	if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${directory}" AND 
		EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt")
		message(STATUS "Configuring tool ${directory}")
		add_subdirectory(${directory})
	endif()
endforeach()
