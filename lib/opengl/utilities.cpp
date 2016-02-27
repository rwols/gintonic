#include "utilities.hpp"
#include "../vec2f.hpp"
#include "../vec3f.hpp"
#include "../vec4f.hpp"
#include "../mat2f.hpp"
#include "../mat3f.hpp"
#include "../mat4f.hpp"

namespace gintonic {
namespace opengl {

void set_uniform(const GLint location, const GLint value) noexcept
{
	glUniform1i(location, value);
}

void set_uniform(const GLint location, const GLfloat value) noexcept
{
	glUniform1f(location, value);
}

void set_uniform(const GLint location, const vec2f& value) noexcept
{
	glUniform2f(location, value.x, value.y);
}

void set_uniform(const GLint location, const vec3f& value) noexcept
{
	glUniform3f(location, value.x, value.y, value.z);
}

void set_uniform(const GLint location, const vec4f& value) noexcept
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void set_uniform(const GLint location, const mat2f& value) noexcept
{
	// TODO
}

void set_uniform(const GLint location, const mat3f& value) noexcept
{
	glUniformMatrix3fv(location, 1, GL_FALSE, value.value_ptr());
}

void set_uniform(const GLint location, const mat4f& value) noexcept
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value.value_ptr());
}

void set_uniform(const GLint location, 
	const std::vector<GLfloat>& values) noexcept
{
	glUniform1fv(location, values.size(), values.data());
}

void set_uniform(const GLint location, 
	const std::vector<GLint>& values) noexcept
{
	glUniform1iv(location, values.size(), values.data());
}

} // namespace opengl
} // namespace gintonic