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
	"${CMAKE_CURRENT_SOURCE_DIR}"
	CACHE INTERNAL "The include directory containing all headers.")

set(gintonic_SHADER_DIR 
	"${gintonic_INCLUDE_DIR}/gintonic/Graphics/Shaders" 
	CACHE INTERNAL "The shader directory.")

set(include_prefix "${CMAKE_CURRENT_SOURCE_DIR}/gintonic")

set(gintonic_header_files
	"${CMAKE_CURRENT_SOURCE_DIR}/gintonic.hpp"
	"${include_prefix}/FbxImporter.hpp"
	"${include_prefix}/ForwardDeclarations.hpp"
	"${include_prefix}/Graphics/SpotShadowBuffer.hpp"
	"${include_prefix}/Graphics/AmbientLight.hpp"
	"${include_prefix}/Graphics/DirectionalShadowBuffer.hpp"
	"${include_prefix}/Graphics/OpenGL/khrplatform.hpp"
	"${include_prefix}/Graphics/OpenGL/Framebuffer.hpp"
	"${include_prefix}/Graphics/OpenGL/ShaderProgram.hpp"
	"${include_prefix}/Graphics/OpenGL/BufferObjectArray.hpp"
	"${include_prefix}/Graphics/OpenGL/Shader.hpp"
	"${include_prefix}/Graphics/OpenGL/BufferObject.hpp"
	"${include_prefix}/Graphics/OpenGL/TextureObject.hpp"
	"${include_prefix}/Graphics/OpenGL/SourceCode.hpp"
	"${include_prefix}/Graphics/OpenGL/VertexArrayObject.hpp"
	"${include_prefix}/Graphics/OpenGL/Vector.hpp"
	"${include_prefix}/Graphics/OpenGL/VertexArrayObjectArray.hpp"
	"${include_prefix}/Graphics/OpenGL/utilities.hpp"
	"${include_prefix}/Graphics/OpenGL/Vertices.hpp"
	"${include_prefix}/Graphics/PointShadowBuffer.hpp"
	"${include_prefix}/Graphics/ShadowBuffer.hpp"
	"${include_prefix}/Graphics/GeometryBuffer.hpp"
	"${include_prefix}/Graphics/DirectionalLight.hpp"
	"${include_prefix}/Graphics/skybox.hpp"
	"${include_prefix}/Graphics/Renderer.hpp"
	"${include_prefix}/Graphics/Material.hpp"
	"${include_prefix}/Graphics/PointLight.hpp"
	"${include_prefix}/Graphics/Skeleton.hpp"
	"${include_prefix}/Graphics/SpotLight.hpp"
	"${include_prefix}/Graphics/Font.hpp"
	"${include_prefix}/Graphics/GUI/Base.hpp"
	"${include_prefix}/Graphics/GUI/StringView.hpp"
	"${include_prefix}/Graphics/GUI/StringPointerView.hpp"
	"${include_prefix}/Graphics/GUI/Panel.hpp"
	"${include_prefix}/Graphics/Texture2D.hpp"
	"${include_prefix}/Graphics/Mesh.hpp"
	"${include_prefix}/Graphics/AnimationClip.hpp"
	"${include_prefix}/Graphics/Light.hpp"
	"${include_prefix}/Graphics/ShaderPrograms.hpp"
	"${include_prefix}/Foundation/tuple.hpp"
	"${include_prefix}/Foundation/simd.hpp"
	"${include_prefix}/Foundation/WriteLock.hpp"
	"${include_prefix}/Foundation/WithAlignedNewAndDelete.hpp"
	"${include_prefix}/Foundation/Object.hpp"
	"${include_prefix}/Foundation/filesystem.hpp"
	"${include_prefix}/Foundation/Profiler.hpp"
	"${include_prefix}/Foundation/WeakPointerCache.hpp"
	"${include_prefix}/Foundation/Reflection.hpp"
	"${include_prefix}/Foundation/scancodes.hpp"
	"${include_prefix}/Foundation/flyweights.hpp"
	"${include_prefix}/Foundation/ReadWriteLock.hpp"
	"${include_prefix}/Foundation/portable_archive_exception.hpp"
	"${include_prefix}/Foundation/Timer.hpp"
	"${include_prefix}/Foundation/portable_archive.hpp"
	"${include_prefix}/Foundation/exception.hpp"
	"${include_prefix}/Foundation/utilities.hpp"
	"${include_prefix}/Foundation/allocator.hpp"
	"${include_prefix}/Foundation/polymorphic_portable_archive.hpp"
	"${include_prefix}/Foundation/Octree.hpp"
	"${include_prefix}/Foundation/portable_oarchive.hpp"
	"${include_prefix}/Foundation/portable_iarchive.hpp"
	"${include_prefix}/Math/mat4fstack.hpp"
	"${include_prefix}/Math/mat4f.hpp"
	"${include_prefix}/Math/mat2f.hpp"
	"${include_prefix}/Math/box2f.hpp"
	"${include_prefix}/Math/vec3f.hpp"
	"${include_prefix}/Math/box3f.hpp"
	"${include_prefix}/Math/vec4f.hpp"
	"${include_prefix}/Math/MatrixPipeline.hpp"
	"${include_prefix}/Math/quatf.hpp"
	"${include_prefix}/Math/SQTstack.hpp"
	"${include_prefix}/Math/vec2f.hpp"
	"${include_prefix}/Math/SQT.hpp"
	"${include_prefix}/Math/mat3f.hpp"
	"${include_prefix}/Math/Interpolator.hpp"
	"${include_prefix}/EntityVisitor.hpp"
	"${include_prefix}/Entity.hpp"
	"${include_prefix}/Camera.hpp"
	"${include_prefix}/Application.hpp"
)

find_package(Doxygen)
option(gintonic_BUILD_DOCUMENTATION 
	"Build documentation (requires Doxygen)" 
	${DOXYGEN_FOUND})

if (gintonic_BUILD_DOCUMENTATION)
	configure_file(cmake/Doxyfile.cmake "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
		COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		DEPENDS ${gintonic_header_files}
		COMMENT "Generating documentation." VERBATIM)
endif()

install(DIRECTORY gintonic DESTINATION include)
install(FILES gintonic.hpp DESTINATION include)
