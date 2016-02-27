#include "framebuffer.hpp"

namespace gintonic {
namespace opengl {

framebuffer::framebuffer()
{
	glGenFramebuffers(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
}

framebuffer& framebuffer::operator = (framebuffer&& other)
{
	this->~framebuffer();
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

void framebuffer::check_status() const noexcept
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Framebuffer status was not complete.");
	}
}

void framebuffer::check_status(bool& r) const noexcept
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		r = false;
	}
	else r = true;
}

} // namespace opengl
} // namespace gintonic
