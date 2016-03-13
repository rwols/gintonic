#include "generic_shader_program.hpp"
#include "shader.hpp"

namespace gintonic {
namespace opengl {

generic_shader_program::compilation_error::compilation_error(
	const boost::filesystem::path& vertexshader,
	const boost::filesystem::path& fragmentshader,
	const std::basic_string<GLchar>& infolog)
: exception_base(vertexshader, fragmentshader, "Compilation error:\n\n")
{
	m_what_msg.append("\tVertex Shader: ").append(vertex_shader().string()).append("\n");
	m_what_msg.append("\tFragment Shader: ").append(fragment_shader().string()).append("\n\n");
	m_what_msg.append("\tError message:\n\n").append(infolog);
}
		
generic_shader_program::compilation_error::compilation_error(
	const boost::filesystem::path& vertexshader,
	const boost::filesystem::path& geometryshader,
	const boost::filesystem::path& fragmentshader,
	const std::basic_string<GLchar>& infolog)
: exception_base(vertexshader, geometryshader, fragmentshader, "Compilation error:\n\n")
{
	m_what_msg.append("\tVertex Shader: ").append(vertex_shader().string()).append("\n");
	m_what_msg.append("\tGeometry Shader: ").append(geometry_shader().string()).append("\n");
	m_what_msg.append("\tFragment Shader: ").append(fragment_shader().string()).append("\n\n");
	m_what_msg.append("\tError message:\n\n").append(infolog);
}

generic_shader_program::generic_shader_program(
	boost::filesystem::path path_vertex,
	boost::filesystem::path path_fragment)
: m_handle(glCreateProgram())
, m_vertexshader(path_vertex)
, m_geometryshader("")
, m_fragmentshader(path_fragment)
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