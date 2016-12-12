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
		glGenBuffers(Size, mHandles);
	}

	/// Destructor.
	inline ~BufferObjectArray() noexcept
	{
		glDeleteBuffers(Size, mHandles);
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
		return mHandles[index]; 
	}

	/**
	 * @brief Retrieve the data contained in this BufferObject.
	 * @tparam Type The type of data to fetch.
	 * @tparam Allocator The allocator for the std::vector.
	 * @param [in] index The index of the buffer.
	 * @param [out] usage The usage type.
	 * @return The data contained in this BufferObject.
	 */
	template
	<
		class Type,
		class Allocator = std::allocator<Type>
	>
	std::vector<Type, Allocator> retrieveDataAs(const std::size_t index, GLint& usage) const
	{
		GLint lSize;
		glBindBuffer(GL_COPY_READ_BUFFER, mHandles[index]);
		glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &lSize);
		glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
		std::vector<Type, Allocator> lResult(lSize);
		glGetBufferSubData(GL_COPY_READ_BUFFER, 0, lSize, (GLvoid*)lResult.data());
		return lResult;
	}

private:

	GLuint mHandles[Size];
};

template <GLuint Size>
BufferObjectArray<Size>::BufferObjectArray(BufferObjectArray<Size>&& other)
{
	std::memcpy(mHandles, other.mHandles, sizeof(GLuint) * Size);
	std::memset(other.mHandles, 0, sizeof(GLuint) * Size);
}

template <GLuint Size>
BufferObjectArray<Size>& BufferObjectArray<Size>::operator = (BufferObjectArray<Size>&& other)
{
	if (this == &other) return *this;
	std::memcpy(mHandles, other.mHandles, sizeof(GLuint) * Size);
	std::memset(other.mHandles, 0, sizeof(GLuint) * Size);
	return *this;
}

} // namespace OpenGL
} // namespace gintonic
