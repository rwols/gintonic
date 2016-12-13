include(CMakeParseArguments)

# Always do shared, even if BUILD_SHARED_LIBS is OFF,
# because these examples are not going to end up in
# the wild anyway.
# FIXME: Move this library to the lib folder? It
# seems more fitting.
add_library(Application SHARED Common/Application.cpp)
set_target_properties(Application PROPERTIES CXX_STANDARD 14)
target_link_libraries(Application PUBLIC gintonic)

function(add_example example_name)
	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs SOURCES ASSETS)
	cmake_parse_arguments(add_example
		"${options}"
		"${oneValueArgs}"
		"${multiValueArgs}"
		${ARGN})
	
	if (NOT add_example_SOURCES)
		message(FATAL_ERROR "Example ${example_name} requires SOURCES argument!")
	elseif (NOT add_example_ASSETS)
		message(WARNING "Example ${example_name} has no ASSETS argument!")
	endif (NOT add_example_SOURCES)

	add_executable(${example_name} ${add_example_SOURCES})
	set_target_properties(${example_name} PROPERTIES CXX_STANDARD 14)
	target_link_libraries(${example_name} PUBLIC Application)
	gintonic_target_depends_on_assets(
		TARGET ${example_name} 
		SOURCE_ASSET_DIRECTORY ${GINTONIC_ASSET_DIR} 
		ASSETS ${add_example_ASSETS})
endfunction()

set(systemFont Inconsolata-Regular.ttf)
set(catTexture DaVinci.jpg)
set(ruralBrickWallTextures RuralBrickWall.jpg RuralBrickWall_SPEC.png RuralBrickWall_NRM.png)
set(bricksTextures bricks.jpg bricks_SPEC.png bricks_NRM.png)

add_example(BasicShapes 
	SOURCES BasicShapes.cpp 
	ASSETS ${systemFont} ${catTexture})

add_example(Console 
	SOURCES Console.cpp 
	ASSETS ${systemFont})

add_example(DirectionalLight 
	SOURCES DirectionalLight.cpp 
	ASSETS ${systemFont} ${ruralBrickWallTextures})

add_example(FbxViewer
	SOURCES FbxViewer.cpp
	ASSETS ${systemFont})

add_example(HelloWorldText 
	SOURCES HelloWorldText.cpp 
	ASSETS ${systemFont})

add_example(Octree
	SOURCES Octree.cpp
	ASSETS ${systemFont})

add_example(OrthographicProjection
	SOURCES OrthographicProjection.cpp
	ASSETS ${systemFont} ${catTexture})

add_example(PointLights
	SOURCES PointLights.cpp
	ASSETS ${systemFont} ${catTexture} ${bricksTextures})

add_example(Shadows
	SOURCES Shadows.cpp
	ASSETS ${systemFont} ${bricksTextures})

add_example(SpotLights
	SOURCES SpotLights.cpp
	ASSETS ${systemFont} ${bricksTextures})

add_example(TexturedCube
	SOURCES TexturedCube.cpp
	ASSETS ${systemFont} ${ruralBrickWallTextures})
