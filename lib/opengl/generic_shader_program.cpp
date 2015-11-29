#include "generic_shader_program.hpp"
#include "shader.hpp"

namespace gintonic {
namespace opengl {

generic_shader_program::compilation_error::compilation_error(
	const boost::filesystem::path& vertexshader,
	const boost::filesystem::path& fragmentshader,
	const std::basic_string<GLchar>& infolog)
: m_vertexshader(vertexshader)
, m_geometryshader("")
, m_fragmentshader(fragmentshader)
, m_what_msg("Compilation error:\n\n")
{
	m_what_msg.append("\tVertex Shader: ").append(m_vertexshader.string()).append("\n");
	m_what_msg.append("\tFragment Shader: ").append(m_fragmentshader.string()).append("\n\n");
	m_what_msg.append("\tError message:\n\n").append(infolog);
}
		
generic_shader_program::compilation_error::compilation_error(
	const boost::filesystem::path& vertexshader,
	const boost::filesystem::path& geometryshader,
	const boost::filesystem::path& fragmentshader,
	const std::basic_string<GLchar>& infolog)
: m_vertexshader(vertexshader)
, m_geometryshader(geometryshader)
, m_fragmentshader(fragmentshader)
, m_what_msg("Compilation error:\n\n")
{
	msg.append("\tVertex Shader: ").append(m_vertexshader.string()).append("\n");
	msg.append("\tGeometry Shader: ").append(m_geometryshader.string()).append("\n");
	msg.append("\tFragment Shader: ").append(m_fragmentshader.string()).append("\n\n");
	msg.append("\tError message:\n\n").append(infolog);
}

generic_shader_program::generic_shader_program(
	boost::filesystem::path path_vertex,
	boost::filesystem::path path_fragment)
: m_handle(glCreateProgram())
{
	if (!m_handle) throw std::bad_alloc();

	GLint r;

	shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
	shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);

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
		glDeleteProgram(m_handle);
		m_handle = 0;
		throw compilation_error(path_vertex, path_fragment, infolog);
	}
}

generic_shader_program::~generic_shader_program() noexcept
{
	glDeleteProgram(m_handle);
	m_handle = 0;
}

} // namespace opengl
} // namespace gintonic