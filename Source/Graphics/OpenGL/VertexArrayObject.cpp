#include "VertexArrayObject.hpp"

namespace gintonic {
namespace OpenGL {

VertexArrayObject::VertexArrayObject() : mHandle(0)
{
	glGenVertexArrays(1, &mHandle);
	if (!mHandle) throw std::bad_alloc();
}

VertexArrayObject& VertexArrayObject::operator = (VertexArrayObject&& other)
	noexcept
{
	glDeleteVertexArrays(1, &mHandle);
	mHandle = other.mHandle;
	other.mHandle = 0;
	return *this;
}

} // namespace OpenGL
} // namespace gintonic