/**
 * @file framebuffer.hpp
 * @brief Defines an OpenGL Framebuffer Object.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_framebuffer_hpp
#define gintonic_opengl_framebuffer_hpp

#include "utilities.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief Encapsulates an 
 * [OpenGL Framebuffer Object](https://www.opengl.org/wiki/Framebuffer_Object)
 */
class framebuffer
{
private:
	GLuint m_handle;
public:

	/// Default constructor.
	framebuffer();

	/// Destructor.
	inline ~framebuffer()
	{
		glDeleteFramebuffers(1, &m_handle);
	}

	/// You cannot copy framebuffers.
	framebuffer(framebuffer&) = delete;

	/// You cannot copy-assign framebuffers.
	framebuffer& operator = (framebuffer&) = delete;
	
	/// Move constructor.
	inline framebuffer(framebuffer&& other) 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	/// Move assignment operator.
	framebuffer& operator = (framebuffer&& other);

	/// Get the underlying OpenGL with a static_cast.
	inline operator GLuint() const noexcept 
	{
		return m_handle;
	}

	void bind(const GLenum token) const noexcept;
	void check_status() const;
	void check_status(bool& r) const noexcept;
};

} // namespace opengl
} // namespace gintonic

#endif