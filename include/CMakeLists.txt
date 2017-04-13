#*******************************************************************************
# gintonic/include
#
# The purpose of this cmake file is to define the variables gintonic_INCLUDE_DIR
# and gintonic_SHADER_DIR, and to define the "gintonic-doc" target so that we
# can build the documentation with "doxygen". It attemps to find the executable
# "doxygen".
#
# Important variables that this cmake file defines are:
#
# - gintonic_INCLUDE_DIR -- This directory (containing the header files)
# - gintonic_SHADER_DIR -- The directory containing the shader files.
# - gintonic_BUILD_DOCUMENTATION -- Wether to build documentation. If the
#     doxygen executable was found, defaults to ON, else it defaults to OFF.
#
#*******************************************************************************

set(gintonic_INCLUDE_DIR 
	${CMAKE_CURRENT_SOURCE_DIR} 
	CACHE INTERNAL "The include directory containing all headers.")

set(gintonic_SHADER_DIR 
	"${gintonic_INCLUDE_DIR}/Graphics/Shaders" 
	CACHE INTERNAL "The shader directory.")

set(gintonic_header_files
	${CMAKE_CURRENT_SOURCE_DIR}/FbxImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ForwardDeclarations.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gintonic.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/SpotShadowBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/AmbientLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/DirectionalShadowBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/khrplatform.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/Framebuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/ShaderProgram.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/BufferObjectArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/Shader.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/BufferObject.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/TextureObject.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/SourceCode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/VertexArrayObject.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/Vector.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/VertexArrayObjectArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/utilities.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/OpenGL/Vertices.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/PointShadowBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/ShadowBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/GeometryBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/DirectionalLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/skybox.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Renderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Material.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/PointLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Skeleton.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/SpotLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Font.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/GUI/Base.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/GUI/StringView.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/GUI/StringPointerView.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/GUI/Panel.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Texture2D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Mesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/AnimationClip.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Light.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Graphics/ShaderPrograms.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/tuple.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/simd.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/WriteLock.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/WithAlignedNewAndDelete.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/Object.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/filesystem.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/Profiler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/WeakPointerCache.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/Reflection.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/scancodes.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/flyweights.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/ReadWriteLock.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/portable_archive_exception.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/Timer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/portable_archive.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/exception.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/utilities.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/allocator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/polymorphic_portable_archive.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/Octree.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/portable_oarchive.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Foundation/portable_iarchive.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/mat4fstack.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/mat4f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/mat2f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/box2f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/vec3f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/box3f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/vec4f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/MatrixPipeline.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/quatf.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/SQTstack.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/vec2f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/SQT.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/mat3f.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Math/Interpolator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/EntityVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Entity.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Camera.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/GraphicsContext.hpp
)

find_package(Doxygen)
option(gintonic_BUILD_DOCUMENTATION 
	"Create and install the HTML based API documentation (requires Doxygen)" 
	${DOXYGEN_FOUND})

if (gintonic_BUILD_DOCUMENTATION)
	configure_file(cmake/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
		COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${gintonic_header_files}
		COMMENT "Generating documentation." VERBATIM)
endif()

install(DIRECTORY . DESTINATION include/gintonic)
