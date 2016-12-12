#include "Graphics/OpenGL/ShaderProgram.hpp"
#include "Graphics/OpenGL/Shader.hpp"
#include "Math/vec2f.hpp"
#include "Math/vec3f.hpp"
#include "Math/vec4f.hpp"
#include "Math/mat2f.hpp"
#include "Math/mat3f.hpp"
#include "Math/mat4f.hpp"

namespace gintonic {
namespace OpenGL {

ShaderProgram::ShaderProgram(
	boost::filesystem::path path_vertex, 
	boost::filesystem::path path_fragment)
	: mHandle(glCreateProgram())
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
		throw exception(std::move(infolog));
	}
}

ShaderProgram::ShaderProgram(
	boost::filesystem::path path_vertex, 
	boost::filesystem::path path_geometry, 
	boost::filesystem::path path_fragment)
	: mHandle(glCreateProgram())
{
	if (!mHandle) throw std::bad_alloc();

	GLint r;

	Shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
	Shader<GL_GEOMETRY_SHADER> geometryshader(path_geometry);
	Shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);

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
		glDeleteProgram(mHandle);
		mHandle = 0;
		throw exception(std::move(infolog));
	}
}

GLint ShaderProgram::numActiveAttributes() const noexcept
{
	GLint result;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTES, &result);
	return result;
}

GLint ShaderProgram::numActiveUniforms() const noexcept
{
	GLint result;
	glGetProgramiv(*this, GL_ACTIVE_UNIFORMS, &result);
	return result;
}

ShaderProgram::Uniform ShaderProgram::getUniform(const GLint index) const
{
	Uniform result;
	GLint bufsize;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
	result.name.resize(bufsize);
	glGetActiveUniform(*this, index, bufsize, nullptr, &result.size, &result.type, &result.name[0]);
	result.location = glGetUniformLocation(*this, result.name.c_str());
	return result;
}

ShaderProgram::Attribute ShaderProgram::getAttribute(const GLint location) const
{
	Attribute result;
	GLint bufsize;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
	result.name.resize(bufsize);
	glGetActiveAttrib(*this, location, bufsize, nullptr, &result.size, &result.type, &result.name[0]);
	result.location = glGetAttribLocation(*this, result.name.c_str());
	return result;
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) : mHandle(other.mHandle)
{
	other.mHandle = 0;
}

ShaderProgram& ShaderProgram::operator = (ShaderProgram&& other)
{
	this->~ShaderProgram();
	mHandle = other.mHandle;
	other.mHandle = 0;
	return *this;
}

ShaderProgram::~ShaderProgram() noexcept { glDeleteProgram(*this); }

void ShaderProgram::activate() const noexcept { glUseProgram(mHandle); }
void ShaderProgram::deactivate() noexcept { glUseProgram(0); }

GLint ShaderProgram::getUniformLocation(const GLchar* name) const
{
	const auto r = glGetUniformLocation(*this, name);
	if (r == -1)
	{
		throw exception("Uniform \"" + std::basic_string<GLchar>(name) + "\" not present.");
	}
	else
	{
		return r;
	}
}

bool ShaderProgram::getUniformLocation(const GLchar* name, GLint& location) const noexcept
{
	location = glGetUniformLocation(*this, name);
	return location == -1 ? false : true;
}

GLuint ShaderProgram::getUniformBlockIndex(const GLchar* name) const
{
	const auto r = glGetUniformBlockIndex(*this, name);
	if (r == GL_INVALID_INDEX)
	{
		throw exception("Uniform block \"" + std::basic_string<GLchar>(name) + "\" not present.");
	}
	else
	{
		return r;
	}
}

bool ShaderProgram::getUniformBlockIndex(const GLchar* name, GLuint& index) const noexcept
{
	index = glGetUniformBlockIndex(*this, name);
	return index == GL_INVALID_INDEX ? false : true;
}

GLint ShaderProgram::getAttributeLocation(const GLchar* name) const
{
	const auto r = glGetAttribLocation(*this, name);
	if (r == -1)
	{
		throw std::runtime_error("ShaderProgram::getAttributeLocation: attrib not found.");
	}
	else
	{
		return r;
	}
}

bool ShaderProgram::getAttributeLocation(const GLchar* name, GLint& location) const noexcept
{
	location = glGetAttribLocation(*this, name);
	return location == -1 ? false : true;
}

void ShaderProgram::setUniform(const char* uniformName, const GLfloat value) const
{
	glUniform1f(getUniformLocation(uniformName), value);
}
void ShaderProgram::setUniform(const char* uniformName, const vec2f& v) const
{
	glUniform2f(getUniformLocation(uniformName), v.x, v.y);
}
void ShaderProgram::setUniform(const char* uniformName, const vec3f& v) const
{
	glUniform3f(getUniformLocation(uniformName), v.x, v.y, v.z);
}
void ShaderProgram::setUniform(const char* uniformName, const vec4f& v) const
{
	glUniform4f(getUniformLocation(uniformName), v.x, v.y, v.z, v.w);
}
void ShaderProgram::setUniform(const char* uniformName, const mat3f& m) const
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
	
	glUniformMatrix3fv(getUniformLocation(uniformName), 1, GL_FALSE, m.value_ptr());
}
void ShaderProgram::setUniform(const char* uniformName, const mat4f& m) const
{
	glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE, m.value_ptr());
}
void ShaderProgram::setUniform(const char* uniformName, const GLint i) const
{
	glUniform1i(getUniformLocation(uniformName), i);
}
void ShaderProgram::setUniform(const GLint location, const GLfloat value) noexcept
{
	glUniform1f(location, value);
}
void ShaderProgram::setUniform(const GLint location, const vec2f& v) noexcept
{
	glUniform2f(location, v.x, v.y);
}
void ShaderProgram::setUniform(const GLint location, const vec3f& v) noexcept
{
	glUniform3f(location, v.x, v.y, v.z);
}
void ShaderProgram::setUniform(const GLint location, const vec4f& v) noexcept
{
	glUniform4f(location, v.x, v.y, v.z, v.w);
}
void ShaderProgram::setUniform(const GLint location, const mat3f& m) noexcept
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
void ShaderProgram::setUniform(const GLint location, const mat4f& m) noexcept
{
	glUniformMatrix4fv(location, 1, GL_FALSE, m.value_ptr());
}
void ShaderProgram::setUniform(const GLint location, const GLint i) noexcept
{
	glUniform1i(location, i);
}
void ShaderProgram::setUniform(const GLint location, const std::vector<GLfloat>& values) noexcept
{
	glUniform1fv(location, values.size(), values.data());
}
void ShaderProgram::setUniform(const GLint location, const std::vector<GLint>& values) noexcept
{
	glUniform1iv(location, values.size(), values.data());
}


} // namespace OpenGL
} // namespace gintonic