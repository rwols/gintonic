#include "TextureObject.hpp"

namespace gintonic {
namespace OpenGL {

TextureObject::TextureObject() : mHandle(0)
{
	glGenTextures(1, &mHandle);
	if (!mHandle) throw std::bad_alloc();
}

TextureObject& TextureObject::operator=(TextureObject&& other) noexcept
{
	glDeleteTextures(1, &mHandle);
	mHandle = other.mHandle;
	other.mHandle = 0;
	return *this;
}

} // namespace OpenGL
} // namespace gintonic
