#include "GenericShaderProgram.hpp"
#include "Shader.hpp"
#include "../../Math/vec2f.hpp"
#include "../../Math/vec3f.hpp"
#include "../../Math/vec4f.hpp"
#include "../../Math/mat2f.hpp"
#include "../../Math/mat3f.hpp"
#include "../../Math/mat4f.hpp"

namespace gintonic {
namespace OpenGL {

GenericShaderProgram::CompilationError::CompilationError(
	const boost::filesystem::path& vertexshader,
	const boost::filesystem::path& fragmentshader,
	const std::basic_string<GLchar>& infolog)
: ExceptionBase(vertexshader, fragmentshader, "Compilation error:\n\n")
{
	mWhatMessage.append("\tVertex Shader: ").append(getVertexShaderFilename().string()).append("\n");
	mWhatMessage.append("\tFragment Shader: ").append(getFragmentShaderFilename().string()).append("\n\n");
	mWhatMessage.append("\tError message:\n\n").append(infolog);
}
		
GenericShaderProgram::CompilationError::CompilationError(
	const boost::filesystem::path& vertexshader,
	const boost::filesystem::path& geometryshader,
	const boost::filesystem::path& fragmentshader,
	const std::basic_string<GLchar>& infolog)
: ExceptionBase(vertexshader, geometryshader, fragmentshader, "Compilation error:\n\n")
{
	mWhatMessage.append("\tVertex Shader: ").append(getVertexShaderFilename().string()).append("\n");
	mWhatMessage.append("\tGeometry Shader: ").append(getGeometryShaderFilename().string()).append("\n");
	mWhatMessage.append("\tFragment Shader: ").append(getFragmentShaderFilename().string()).append("\n\n");
	mWhatMessage.append("\tError message:\n\n").append(infolog);
}

GenericShaderProgram::GenericShaderProgram(
	boost::filesystem::path path_vertex,
	boost::filesystem::path path_fragment)
: mHandle(glCreateProgram())
, mVertexShader(path_vertex)
, mGeometryShader("")
, mFragmentShader(path_fragment)
{
	if (!mHandle) throw std::bad_alloc();

	GLint r;

	Shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
	Shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);

	glAttachShader(*this, vertexshader);
	glAttachShader(*this, fragmentshader);

	glLinkProgram(*this);
	glGetProgramiv(*this, GL_LINK_STATUS, &r);
	
	glDetachShader(*this, vertexshader);
	glDetachShader(*this, fragmentshader);

	if (r == GL_FALSE)
	{
		std::basic_string<GLchar> infolog;
		glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &r);
		infolog.resize(r);
		glGetProgramInfoLog(*this, r, nullptr, &infolog[0]);
		glDeleteProgram(mHandle);
		mHandle = 0;
		throw CompilationError(path_vertex, path_fragment, infolog);
	}
}

GenericShaderProgram::~GenericShaderProgram() noexcept
{
	glDeleteProgram(mHandle);
	mHandle = 0;
}

} // namespace OpenGL
} // namespace gintonic