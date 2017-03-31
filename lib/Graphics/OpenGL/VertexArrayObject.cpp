#include <gintonic/Graphics/OpenGL/VertexArrayObject.hpp>

using namespace gintonic;
using namespace gintonic::OpenGL;

VertexArrayObject::VertexArrayObject() : mHandle(0)
{
    glGenVertexArrays(1, &mHandle);
    if (!mHandle) throw std::bad_alloc();
}

VertexArrayObject& VertexArrayObject::
operator=(VertexArrayObject&& other) noexcept
{
    glDeleteVertexArrays(1, &mHandle);
    mHandle = other.mHandle;
    other.mHandle = 0;
    return *this;
}
