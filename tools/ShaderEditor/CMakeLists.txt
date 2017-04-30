include(AddGintonicTool)
if (NOT WIN32)
add_gintonic_tool(ShaderEditor SOURCES main.cpp 
    ASSETS fonts/Inconsolata-Regular.ttf)
endif()
