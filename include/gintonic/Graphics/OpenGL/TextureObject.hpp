/**
 * @file TextureObject.hpp
 * @brief Defines an OpenGL Texture Object.
 * @author Raoul Wols
 */

#pragma once

#include <gintonic/Graphics/OpenGL/utilities.hpp>

#include <gintonic/nsbegin.hpp>

namespace OpenGL
{

/**
 * @brief Encapsulates an
 * [OpenGL Texture Object](https://www.opengl.org/wiki/Texture)
 */
class TextureObject
{
  private:
    GLuint mHandle;

  public:
    /// Get the underlying OpenGL handle with a static_cast.
    inline operator GLuint() const noexcept { return mHandle; }

    /// Constructor.
    TextureObject();

    /// You cannot copy construct a texture object.
    TextureObject(const TextureObject& other) = delete;

    /// You cannot copy assign a texture object.
    TextureObject& operator=(const TextureObject& other) = delete;

    /// You can, however, move construct a texture object.
    inline TextureObject(TextureObject&& other) noexcept
        : mHandle(other.mHandle)
    {
        other.mHandle = 0;
    }

    /// You can, however, move assign a texture object.
    TextureObject& operator=(TextureObject&& other) noexcept;

    /// Destructor destroys the OpenGL handle.
    inline ~TextureObject() noexcept { glDeleteTextures(1, &mHandle); }

    /// Bind a texture object to the specified texture unit.
    inline void bind(const GLenum texture_type, const GLint texture_unit) const
        noexcept
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(texture_type, mHandle);
    }
};

} // namespace OpenGL

#include <gintonic/nsend.hpp>
