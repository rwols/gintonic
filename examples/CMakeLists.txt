#*******************************************************************************
# gintonic/examples
#
# The purpose of this cmake file is to define various example executables. All
# examples will link to the "Application" library target, which in turns links
# to the "gintonic" library target. The "Application" library simplifies writing
# examples by defining a base class from which to inherit from.
# Most examples also depend on assets from the asset directory. That is taken
# care of by the function "gintonic_target_depends_on_assets". See the cmake
# file in gintonic/Resources for the definition of that function.
#
#*******************************************************************************

include(CMakeParseArguments)

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
		message(FATAL_ERROR 
			"Example ${example_name} requires SOURCES argument!")
	elseif (NOT add_example_ASSETS)
		message(WARNING "Example ${example_name} has no ASSETS argument!")
	endif (NOT add_example_SOURCES)

	add_executable(${example_name} ${add_example_SOURCES})
	set_target_properties(${example_name} PROPERTIES CXX_STANDARD 14)
	target_link_libraries(${example_name} PUBLIC gintonic)
	gintonic_target_depends_on_assets(
		TARGET ${example_name} 
		SOURCE_ASSET_DIRECTORY ${gintonic_ASSET_DIR} 
		ASSETS ${add_example_ASSETS})
endfunction()

set(systemFont fonts/Inconsolata-Regular.ttf)
set(catTexture images/DaVinci.jpg)

set(ruralBrickWallTextures 
	images/RuralBrickWall.jpg 
	images/RuralBrickWall_SPEC.png 
	images/RuralBrickWall_NRM.png)

set(bricksTextures 
	images/bricks.jpg 
	images/bricks_SPEC.png 
	images/bricks_NRM.png)

add_example(BasicShapes 
	SOURCES BasicShapes.cpp 
	ASSETS ${systemFont} ${catTexture})

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
