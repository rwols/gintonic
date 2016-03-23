/**
 * @file Framebuffer.hpp
 * @brief Defines an OpenGL Framebuffer Object.
 * @author Raoul Wols
 */

#pragma once

#include "utilities.hpp"

namespace gintonic {
namespace OpenGL {

/**
 * @brief Encapsulates an 
 * [OpenGL Framebuffer Object](https://www.opengl.org/wiki/Framebuffer_Object)
 */
class Framebuffer
{
private:
	GLuint mHandle;
public:

	class Exception : public std::exception
	{
	public:

		inline Exception(const GLenum status) noexcept : statusCode(status) {}
		virtual ~Exception() noexcept = default;
		inline virtual const char* what() const noexcept
		{
			return statusCodeToErrorString(statusCode);
		}

		const GLenum statusCode;

	private:

		static const char* statusCodeToErrorString(const GLenum status) noexcept;

	};

	/// Default constructor.
	Framebuffer();

	/// Destructor.
	inline ~Framebuffer()
	{
		glDeleteFramebuffers(1, &mHandle);
	}

	/// You cannot copy Framebuffers.
	Framebuffer(Framebuffer&) = delete;

	/// You cannot copy-assign Framebuffers.
	Framebuffer& operator = (Framebuffer&) = delete;
	
	/// Move constructor.
	inline Framebuffer(Framebuffer&& other) 
	: mHandle(other.mHandle)
	{
		other.mHandle = 0;
	}

	/// Move assignment operator.
	Framebuffer& operator = (Framebuffer&& other);

	/// Get the underlying OpenGL with a static_cast.
	inline operator GLuint() const noexcept 
	{
		return mHandle;
	}

	void bind(const GLenum token) const noexcept;
	void checkStatus() const;
	void checkStatus(bool& r) const noexcept;
};

} // namespace OpenGL
} // namespace gintonic
