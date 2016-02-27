#include "shader_program.hpp"
#include "shader.hpp"
#include "../vec2f.hpp"
#include "../vec3f.hpp"
#include "../vec4f.hpp"
#include "../mat2f.hpp"
#include "../mat3f.hpp"
#include "../mat4f.hpp"

namespace gintonic {
namespace opengl {

shader_program::shader_program(
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
		throw exception(std::move(infolog));
	}
}

shader_program::shader_program(
	boost::filesystem::path path_vertex, 
	boost::filesystem::path path_geometry, 
	boost::filesystem::path path_fragment)
	: m_handle(glCreateProgram())
{
	if (!m_handle) throw std::bad_alloc();

	GLint r;

	shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
	shader<GL_GEOMETRY_SHADER> geometryshader(path_geometry);
	shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);

	glAttachShader(*this, vertexshader);
	glAttachShader(*this, geometryshader);
	glAttachShader(*this, fragmentshader);

	glLinkProgram(*this);
	glGetProgramiv(*this, GL_LINK_STATUS, &r);

	glDetachShader(*this, vertexshader);
	glDetachShader(*this, geometryshader);
	glDetachShader(*this, fragmentshader);

	if (r == GL_FALSE)
	{
		std::basic_string<GLchar> infolog;
		glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &r);
		infolog.resize(r);
		glGetProgramInfoLog(*this, r, nullptr, &infolog[0]);
		glDeleteProgram(m_handle);
		m_handle = 0;
		throw exception(std::move(infolog));
	}
}

GLint shader_program::num_active_attributes() const noexcept
{
	GLint result;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTES, &result);
	return result;
}

GLint shader_program::num_active_uniforms() const noexcept
{
	GLint result;
	glGetProgramiv(*this, GL_ACTIVE_UNIFORMS, &result);
	return result;
}

shader_program::uniform shader_program::get_uniform(const GLint index) const
{
	uniform result;
	GLint bufsize;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
	result.name.resize(bufsize);
	glGetActiveUniform(*this, index, bufsize, nullptr, &result.size, &result.type, &result.name[0]);
	result.location = glGetUniformLocation(*this, result.name.c_str());
	return result;
}

shader_program::attribute shader_program::get_attribute(const GLint location) const
{
	attribute result;
	GLint bufsize;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
	result.name.resize(bufsize);
	glGetActiveAttrib(*this, location, bufsize, nullptr, &result.size, &result.type, &result.name[0]);
	result.location = glGetAttribLocation(*this, result.name.c_str());
	return result;
}

shader_program::shader_program(shader_program&& other) : m_handle(other.m_handle)
{
	other.m_handle = 0;
}

shader_program& shader_program::operator = (shader_program&& other)
{
	this->~shader_program();
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

shader_program::~shader_program() noexcept { glDeleteProgram(*this); }

void shader_program::activate() const noexcept { glUseProgram(m_handle); }
void shader_program::deactivate() noexcept { glUseProgram(0); }

GLint shader_program::get_uniform_location(const GLchar* name) const
{
	const auto r = glGetUniformLocation(*this, name);
	if (r == -1)
	{
		throw exception("Uniform \"" + std::string(name) + "\" not present.");
	}
	else
	{
		return r;
	}
}

bool shader_program::get_uniform_location(const GLchar* name, GLint& location) const noexcept
{
	location = glGetUniformLocation(*this, name);
	return location == -1 ? false : true;
}

GLint shader_program::get_attrib_location(const GLchar* name) const
{
	const auto r = glGetAttribLocation(*this, name);
	if (r == -1)
	{
		throw std::runtime_error("shader_program::get_attrib_location: attrib not found.");
	}
	else
	{
		return r;
	}
}

bool shader_program::get_attrib_location(const GLchar* name, GLint& location) const noexcept
{
	location = glGetAttribLocation(*this, name);
	return location == -1 ? false : true;
}

void shader_program::set_uniform(const char* uniformName, const GLfloat value) const
{
	glUniform1f(get_uniform_location(uniformName), value);
}
void shader_program::set_uniform(const char* uniformName, const vec2f& v) const
{
	glUniform2f(get_uniform_location(uniformName), v.x, v.y);
}
void shader_program::set_uniform(const char* uniformName, const vec3f& v) const
{
	glUniform3f(get_uniform_location(uniformName), v.x, v.y, v.z);
}
void shader_program::set_uniform(const char* uniformName, const vec4f& v) const
{
	glUniform4f(get_uniform_location(uniformName), v.x, v.y, v.z, v.w);
}
void shader_program::set_uniform(const char* uniformName, const mat3f& m) const
{
	// float temp[9];
	// const float* ptr = m.value_ptr();
	// temp[0] = ptr[0];
	// temp[1] = ptr[1];
	// temp[2] = ptr[2];

	// temp[3] = ptr[4];
	// temp[4] = ptr[5];
	// temp[5] = ptr[6];
	
	// temp[6] = ptr[8];
	// temp[7] = ptr[9];
	// temp[8] = ptr[10];
	
	glUniformMatrix3fv(get_uniform_location(uniformName), 1, GL_FALSE, m.value_ptr());
}
void shader_program::set_uniform(const char* uniformName, const mat4f& m) const
{
	glUniformMatrix4fv(get_uniform_location(uniformName), 1, GL_FALSE, m.value_ptr());
}
void shader_program::set_uniform(const char* uniformName, const GLint i) const
{
	glUniform1i(get_uniform_location(uniformName), i);
}
void shader_program::set_uniform(const GLint location, const GLfloat value) noexcept
{
	glUniform1f(location, value);
}
void shader_program::set_uniform(const GLint location, const vec2f& v) noexcept
{
	glUniform2f(location, v.x, v.y);
}
void shader_program::set_uniform(const GLint location, const vec3f& v) noexcept
{
	glUniform3f(location, v.x, v.y, v.z);
}
void shader_program::set_uniform(const GLint location, const vec4f& v) noexcept
{
	glUniform4f(location, v.x, v.y, v.z, v.w);
}
void shader_program::set_uniform(const GLint location, const mat3f& m) noexcept
{
	// float temp[9];
	// const float* ptr = m.value_ptr();
	// temp[0] = ptr[0];
	// temp[1] = ptr[1];
	// temp[2] = ptr[2];

	// temp[3] = ptr[4];
	// temp[4] = ptr[5];
	// temp[5] = ptr[6];
	
	// temp[6] = ptr[8];
	// temp[7] = ptr[9];
	// temp[8] = ptr[10];

	glUniformMatrix3fv(location, 1, GL_FALSE, m.value_ptr());
}
void shader_program::set_uniform(const GLint location, const mat4f& m) noexcept
{
	glUniformMatrix4fv(location, 1, GL_FALSE, m.value_ptr());
}
void shader_program::set_uniform(const GLint location, const GLint i) noexcept
{
	glUniform1i(location, i);
}
void shader_program::set_uniform(const GLint location, const std::vector<GLfloat>& values) noexcept
{
	glUniform1fv(location, values.size(), values.data());
}
void shader_program::set_uniform(const GLint location, const std::vector<GLint>& values) noexcept
{
	glUniform1iv(location, values.size(), values.data());
}


} // namespace opengl
} // namespace gintonic