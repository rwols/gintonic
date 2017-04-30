include(AddGintonicTool)
set(source_files
	main.cpp)
if (NOT WIN32)
add_gintonic_tool(TextureEditor SOURCES ${source_files}
	ASSETS fonts/Inconsolata-Regular.ttf)
endif()
