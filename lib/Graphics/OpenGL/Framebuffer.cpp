#include "Framebuffer.hpp"

namespace gintonic {
namespace OpenGL {

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &mHandle);
	if (!mHandle) throw std::bad_alloc();
}

Framebuffer& Framebuffer::operator = (Framebuffer&& other)
{
	this->~Framebuffer();
	mHandle = other.mHandle;
	other.mHandle = 0;
	return *this;
}

void Framebuffer::bind(const GLenum token) const noexcept
{
	glBindFramebuffer(token, mHandle);
}

void Framebuffer::checkStatus() const
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Framebuffer status was not complete.");
	}
}

void Framebuffer::checkStatus(bool& r) const noexcept
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		r = false;
	}
	else r = true;
}

} // namespace OpenGL
} // namespace gintonic
