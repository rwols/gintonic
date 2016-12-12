/**
 * @file VertexArrayObjectArray.hpp
 * @brief Defines an array of OpenGL Vertex Array Objects.
 * @author Raoul Wols
 */

#pragma once

#include "utilities.hpp"

namespace gintonic {
namespace OpenGL {

/**
 * @brief Encapsulates a fixed size array of 
 * [OpenGL Vertex Array Object](https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object)s.
 * 
 * @tparam Size The number of vertex object arrays.
 */
template <GLuint Size> class VertexArrayObjectArray
{
public:

	/// Default constructor.
	inline VertexArrayObjectArray()
	{
		glGenVertexArrays(Size, m_handles);
	}

	/// Destructor.
	inline ~VertexArrayObjectArray() noexcept
	{
		glDeleteVertexArrays(Size, m_handles);
	}

	/// You cannot copy vertex array object arrays.
	VertexArrayObjectArray(const VertexArrayObjectArray&) = delete;

	/// Move constructor.
	VertexArrayObjectArray(VertexArrayObjectArray&& other);

	/// You cannot copy vertex array object arrays.
	VertexArrayObjectArray& operator = (
		const VertexArrayObjectArray&) = delete;

	/// Move assignment operator.
	VertexArrayObjectArray& operator = (VertexArrayObjectArray&& other);

	/**
	 * @brief Get the underlying OpenGL handle at the specified index.
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

template <class GLuint Size>
VertexArrayObjectArray<Size>::VertexArrayObjectArray(
	VertexArrayObjectArray<Size>&& other)
{
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
}

template <class GLuint Size>
VertexArrayObjectArray<Size>& VertexArrayObjectArray<Size>::operator = (
	VertexArrayObjectArray<Size>&& other)
{
	if (this == &other) return *this;
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	return *this;
}

} // namespace OpenGL
} // namespace gintonic