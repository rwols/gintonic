/**
 * @file Framebuffer.hpp
 * @brief Defines an OpenGL Framebuffer Object.
 * @author Raoul Wols
 */

#pragma once

#include <gintonic/Graphics/OpenGL/utilities.hpp>

#include <gintonic/nsbegin.hpp>

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

	/**
	 * @brief Used when checking the status of a Framebuffer.
	 */
	class Exception : public std::exception
	{
	public:

		/**
		 * @brief Constructor.
		 * @param [in] status The status code that OpenGL returns.
		 */
		inline Exception(const GLenum status) noexcept : statusCode(status) {}
		
		/// Defaulted destructor.
		virtual ~Exception() noexcept = default;

		/// Returns a string representation of the status code.
		inline virtual const char* what() const noexcept
		{
			return statusCodeToErrorString(statusCode);
		}

		/// The status code.
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

	/**
	 * @brief Bind this Framebuffer.
	 * @param token Either `GL_DRAW_FRAMEBUFFER` or `GL_READ_FRAMEBUFFER`.
	 */
	void bind(const GLenum token) const noexcept;

	/**
	 * @brief Check the status of this Framebuffer.
	 * @exception gintonic::OpenGL::Framebuffer::Exception
	 */
	void checkStatus() const;

	/**
	 * @brief Check the status of this Framebuffer.
	 * @param [out] r True if the Framebuffer is complete,
	 * false if it is not complete.
	 */
	void checkStatus(bool& r) const noexcept;
};

} // namespace OpenGL

#include <gintonic/nsend.hpp>
