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
	RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
	*)

foreach(directory "${directories}")
	if(IS_DIRECTORY "${directory}" AND EXISTS "${directory}/CMakeLists.txt")
		add_subdirectory("${directory}")
	endif()
endforeach()

# TODO: Remove all this and do it "llvm-style"
function(add_gintonic_tool tool_name)
	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs SOURCES ASSETS)
	cmake_parse_arguments(add_gintonic_tool
		"${options}"
		"${oneValueArgs}"
		"${multiValueArgs}"
		${ARGN})
	
	if (NOT add_gintonic_tool_SOURCES)
		message(FATAL_ERROR "Tool ${tool_name} requires SOURCES argument!")
	endif (NOT add_gintonic_tool_SOURCES)

	add_executable(${tool_name} ${add_gintonic_tool_SOURCES})
	set_target_properties(${tool_name} PROPERTIES CXX_STANDARD 14)
	target_link_libraries(${tool_name} PUBLIC gintonic)
	if (add_gintonic_tool_ASSETS)
		gintonic_target_depends_on_assets(
			TARGET ${tool_name} 
			SOURCE_ASSET_DIRECTORY ${gintonic_ASSET_DIR} 
			ASSETS ${add_gintonic_tool_ASSETS})
	endif (add_gintonic_tool_ASSETS)
endfunction(add_gintonic_tool)

add_gintonic_tool(DriverInfo SOURCES DriverInfo.cpp)
add_gintonic_tool(FbxConverter SOURCES FbxConverter.cpp 
	ASSETS fonts/Inconsolata-Regular.ttf)
