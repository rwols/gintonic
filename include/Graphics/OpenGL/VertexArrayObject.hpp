/**
 * @file VertexArrayObject.hpp
 * @brief Defines the OpenGL Vertex Array Object.
 * @author Raoul Wols
 */

#pragma once

#include "utilities.hpp"

namespace gintonic {
namespace OpenGL {

/**
 * @brief Encapsulates an
 * [OpenGL Vertex Array
 * Object](https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object).
 */
class GINTONIC_EXPORT VertexArrayObject
{
  private:
    GLuint mHandle;

  public:
    /// Get the underlying OpenGL handle with a static_cast.
    inline operator GLuint() const noexcept { return mHandle; }

    /// Default constructor.
    VertexArrayObject();

    /// Destructor.
    inline ~VertexArrayObject() noexcept { glDeleteVertexArrays(1, &mHandle); }

    /// You cannot copy vertex array objects.
    VertexArrayObject(const VertexArrayObject&) = delete;

    /// Move constructor.
    inline VertexArrayObject(VertexArrayObject&& other) noexcept
        : mHandle(other.mHandle)
    {
        other.mHandle = 0;
    }

    /// You cannot copy vertex array objects.
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;

    /// Move assignment operator.
    VertexArrayObject& operator=(VertexArrayObject&& other) noexcept;
};

} // namespace OpenGL
} // namespace gintonic
