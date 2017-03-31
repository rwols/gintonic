/**
 * @file Shader.hpp
 * @brief Defines a templated OpenGL Shader Object.
 * @author Raoul Wols
 */

#pragma once

#include <gintonic/Foundation/exception.hpp>
#include <gintonic/Graphics/OpenGL/SourceCode.hpp>
#include <gintonic/Graphics/OpenGL/utilities.hpp>

#include <gintonic/nsbegin.hpp>

namespace OpenGL
{

/**
 * @brief Encapsulates an
 * [OpenGL Shader Object](https://www.opengl.org/wiki/Shader_Compilation).
 * @tparam Type The shader type. Can be one of:
 * * GL_VERTEX_SHADER
 * * GL_TESS_CONTROL_SHADER
 * * GL_TESS_EVALUATION_SHADER
 * * GL_GEOMETRY_SHADER
 * * GL_FRAGMENT_SHADER
 * * GL_COMPUTE_SHADER
 */
template <GLint Type> class Shader
{
  public:
    /**
     * @brief Construct a Shader from the given GLSL file.
     * @param p The filepath to a GLSL file.
     */
    Shader(const boost::filesystem::path& p);

    /**
     * @brief Construct a Shader from the given SourceCode object.
     * @param source The SourceCode object.
     */
    Shader(const SourceCode& source);

    /// Destructor.
    inline ~Shader() noexcept { glDeleteShader(mHandle); }

    /// Get the underlying OpenGL handle via a static_cast.
    inline operator GLuint() const noexcept { return mHandle; }

  private:
    GLuint mHandle;
};

template <GLint Type>
Shader<Type>::Shader(const boost::filesystem::path& p)
    : mHandle(glCreateShader(Type))
{
    {
        const SourceCode source(p);
        glShaderSource(*this, source.count(), source.string(), nullptr);
    }
    GLint r;
    glCompileShader(*this);
    glGetShaderiv(*this, GL_COMPILE_STATUS, &r);
    if (r == GL_FALSE)
    {
        std::string error_msg;
        glGetShaderiv(*this, GL_INFO_LOG_LENGTH, &r);
        error_msg.resize(r);
        glGetShaderInfoLog(*this, r, nullptr, &error_msg[0]);
        glDeleteShader(*this);
        throw exception(std::move(error_msg));
    }
}

template <GLint Type>
Shader<Type>::Shader(const SourceCode& source) : mHandle(glCreateShader(Type))
{
    GLint r;
    glShaderSource(*this, source.count(), source.string(), nullptr);
    glCompileShader(*this);
    glGetShaderiv(*this, GL_COMPILE_STATUS, &r);
    if (r == GL_FALSE)
    {
        std::string error_msg;
        glGetShaderiv(*this, GL_INFO_LOG_LENGTH, &r);
        error_msg.resize(r);
        glGetShaderInfoLog(*this, r, nullptr, &error_msg[0]);
        glDeleteShader(*this);
        throw exception(std::move(error_msg));
    }
}

/**
 * @brief Convenience typedef for a vertex Shader.
 */
typedef Shader<GL_VERTEX_SHADER> VertexShader;

/**
 * @brief Convenience typedef for a fragment Shader.
 */
typedef Shader<GL_FRAGMENT_SHADER> FragmentShader;

/**
 * @brief Convenience typedef for a geometry Shader.
 */
typedef Shader<GL_GEOMETRY_SHADER> GeometryShader;

} // namespace OpenGL

#include <gintonic/nsend.hpp>
