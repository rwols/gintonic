#ifndef gintonic_opengl_framebuffer_hpp
#define gintonic_opengl_framebuffer_hpp

#include "utilities.hpp"

namespace gintonic {
namespace opengl {

class framebuffer
{
private:
	GLuint m_handle;
public:
	framebuffer();

	inline ~framebuffer()
	{
		glDeleteFramebuffers(1, &m_handle);
	}

	framebuffer(framebuffer&) = delete;
	framebuffer& operator = (framebuffer&) = delete;
	
	inline framebuffer(framebuffer&& other) 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	framebuffer& operator = (framebuffer&& other);

	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
};

} // namespace opengl
} // namespace gintonic

#endif