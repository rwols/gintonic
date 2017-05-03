#include "Graphics/OpenGL/utilities.hpp"
#include "Math/mat2f.hpp"
#include "Math/mat3f.hpp"
#include "Math/mat4f.hpp"
#include "Math/vec2f.hpp"
#include "Math/vec3f.hpp"
#include "Math/vec4f.hpp"

namespace gintonic
{
namespace OpenGL
{

void setUniform(const GLint location, const GLint value) noexcept
{
    glUniform1i(location, value);
}

void setUniform(const GLint location, const GLfloat value) noexcept
{
    glUniform1f(location, value);
}

void setUniform(const GLint location, const vec2f& value) noexcept
{
    glUniform2f(location, value.x, value.y);
}

void setUniform(const GLint location, const vec3f& value) noexcept
{
    glUniform3f(location, value.x, value.y, value.z);
}

void setUniform(const GLint location, const vec4f& value) noexcept
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void setUniform(const GLint location, const mat2f& value) noexcept
{
    // TODO
}

void setUniform(const GLint location, const mat3f& value) noexcept
{
    glUniformMatrix3fv(location, 1, GL_FALSE, value.value_ptr());
}

void setUniform(const GLint location, const mat4f& value) noexcept
{
    glUniformMatrix4fv(location, 1, GL_FALSE, value.value_ptr());
}

void setUniform(const GLint location,
                const std::vector<GLfloat>& values) noexcept
{
    glUniform1fv(location, static_cast<GLsizei>(values.size()), values.data());
}

void setUniform(const GLint location, const std::vector<GLint>& values) noexcept
{
    glUniform1iv(location, static_cast<GLsizei>(values.size()), values.data());
}

void setUniform(const GLint location,
                const std::vector<mat4f, allocator<mat4f>>& values) noexcept
{
    glUniformMatrix4fv(location, static_cast<GLsizei>(values.size()), GL_FALSE,
                       (const GLfloat*)values[0].value_ptr());
}

void setUniform(const GLint location, const std::vector<mat3f>& values) noexcept
{
    glUniformMatrix3fv(location, static_cast<GLsizei>(values.size()), GL_FALSE,
                       (const GLfloat*)values[0].value_ptr());
}

} // namespace opengl
} // namespace gintonic
