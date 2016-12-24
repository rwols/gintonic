#*******************************************************************************
# gintonic/Resources
#
# The purpose of this cmake file is to define the gintonic_ASSET_DIR variable
# and to define the function gintonic_target_depends_on_assets.
#
# Important variables that this cmake file defines are:
#
# - gintonic_ASSET_DIR -- The asset directory
#
#*******************************************************************************

set(gintonic_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR} 
	CACHE INTERNAL "The asset directory.")

function(gintonic_target_depends_on_assets)

	set(options "")
	set(oneValueArgs TARGET SOURCE_ASSET_DIRECTORY)
	set(multiValueArgs ASSETS)
	cmake_parse_arguments(gintonic_target_depends_on_assets
		"${options}"
		"${oneValueArgs}"
		"${multiValueArgs}"
		${ARGN})
	
	set(targetDir $<TARGET_FILE_DIR:${gintonic_target_depends_on_assets_TARGET}>)
	set(sourceAssetDir ${gintonic_target_depends_on_assets_SOURCE_ASSET_DIRECTORY})
	set(targetShaderDir ${targetDir}/Shaders)
	set(targetAssetDir ${targetDir}/assets)

	list(APPEND args 
		COMMAND ${CMAKE_COMMAND} -E copy_directory 
			${gintonic_SHADER_DIR} ${targetShaderDir})

	list(APPEND args 
		COMMAND ${CMAKE_COMMAND} -E make_directory 
			${targetAssetDir})

	foreach (asset ${gintonic_target_depends_on_assets_ASSETS})
		if (NOT EXISTS ${sourceAssetDir}/${asset})
			message(FATAL_ERROR	 "While processing asset dependencies of target\
 \"${gintonic_target_depends_on_assets_TARGET}\" it was determined that the \
asset \"${sourceAssetDir}/${asset}\" does not exist.")
		endif ()
		list(APPEND args 
			COMMAND ${CMAKE_COMMAND} -E copy_if_different 
				${sourceAssetDir}/${asset} ${targetAssetDir}/${asset})
	endforeach()

	add_custom_command(
		TARGET ${gintonic_target_depends_on_assets_TARGET} 
		POST_BUILD ${args})

endfunction()

install(FILES fonts/Inconsolata-Regular.ttf DESTINATION assets/fonts)