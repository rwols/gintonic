/**
 * @file BufferObjectArray.hpp
 * @brief Defines an array of OpenGL Buffer Objects.
 * @author Raoul Wols
 */

#pragma once

#include "utilities.hpp"
#include <cstring> // for std::memcpy and std::memmove

namespace gintonic {
namespace OpenGL {

/**
 * @brief An array of 
 * [OpenGL buffer object](https://www.opengl.org/wiki/Buffer_Object)s.
 * 
 * @tparam Size The number of buffer objects in the array.
 */
template <GLuint Size> class BufferObjectArray
{
public:

	/// Default constructor.
	inline BufferObjectArray()
	{
		glGenBuffers(Size, m_handles);
	}

	/// Destructor.
	inline ~BufferObjectArray() noexcept
	{
		glDeleteBuffers(Size, m_handles);
	}

	/// You cannot copy buffer object arrays.
	BufferObjectArray(const BufferObjectArray&) = delete;

	/// You cannot copy buffer object arrays.
	BufferObjectArray& operator = (const BufferObjectArray&) = delete;

	/// Move constructor.
	BufferObjectArray(BufferObjectArray&& other);

	/// Move assignment operator.
	BufferObjectArray& operator = (BufferObjectArray&& other);

	/**
	 * @brief Access the underlying OpenGL handle at the specified index.
	 * 
	 * @param index The index. Must be less than the Size template parameter.
	 * @return The underlying OpenGL handle at the specified index.
	 */
	inline GLuint operator[](const GLuint index) const 
		noexcept
	{
		return m_handles[index]; 
	}

private:

	GLuint m_handles[Size];
};

template <GLuint Size>
BufferObjectArray<Size>::BufferObjectArray(BufferObjectArray<Size>&& other)
{
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
}

template <GLuint Size>
BufferObjectArray<Size>& BufferObjectArray<Size>::operator = (BufferObjectArray<Size>&& other)
{
	if (this == &other) return *this;
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	return *this;
}

} // namespace OpenGL
} // namespace gintonic
