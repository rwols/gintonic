cmake_minimum_required(VERSION 2.8)

# include_directories(${CMAKE_SOURCE_DIR}/Source)

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
			SOURCE_ASSET_DIRECTORY ${GINTONIC_ASSET_DIR} 
			ASSETS ${add_gintonic_tool_ASSETS})
	endif (add_gintonic_tool_ASSETS)
endfunction(add_gintonic_tool)

add_gintonic_tool(ConsoleTest SOURCES ConsoleTest.cpp)
add_gintonic_tool(DriverInfo SOURCES DriverInfo.cpp)
add_gintonic_tool(FbxConverter SOURCES FbxConverter.cpp ASSETS Inconsolata-Regular.ttf)
add_gintonic_tool(LuaTest SOURCES LuaTest.cpp)
