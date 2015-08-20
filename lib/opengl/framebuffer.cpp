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

} // namespace opengl
} // namespace gintonic
