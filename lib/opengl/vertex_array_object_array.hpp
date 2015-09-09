/**
 * @file vertex_array_object_array.hpp
 * @brief Defines an array of OpenGL Vertex Array Objects.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_vertex_array_object_array_hpp
#define gintonic_opengl_vertex_array_object_array_hpp

#include "utilities.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief Encapsulates a fixed size array of 
 * [OpenGL Vertex Array Object](https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object)s.
 * 
 * @tparam Size The number of vertex object arrays.
 */
template <GLuint Size> class vertex_array_object_array
{
public:

	/// Default constructor.
	inline vertex_array_object_array()
	{
		glGenVertexArrays(Size, m_handles);
	}

	/// Destructor.
	inline ~vertex_array_object_array() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(Size, m_handles);
	}

	/// You cannot copy vertex array object arrays.
	vertex_array_object_array(const vertex_array_object_array&) = delete;

	/// Move constructor.
	vertex_array_object_array(vertex_array_object_array&& other);

	/// You cannot copy vertex array object arrays.
	vertex_array_object_array& operator = (
		const vertex_array_object_array&) = delete;

	/// Move assignment operator.
	vertex_array_object_array& operator = (vertex_array_object_array&& other);

	/**
	 * @brief Get the underlying OpenGL handle at the specified index.
	 * 
	 * @param index The index. Must be less than the Size template parameter.
	 * @return The underlying OpenGL handle at the specified index.
	 */
	inline GLuint operator[](const GLuint index) const 
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handles[index];
	}

private:
	GLuint m_handles[Size];
};

template <class GLuint Size>
vertex_array_object_array<Size>::vertex_array_object_array(
	vertex_array_object_array<Size>&& other)
{
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
}

template <class GLuint Size>
vertex_array_object_array<Size>& vertex_array_object_array<Size>::operator = (
	vertex_array_object_array<Size>&& other)
{
	if (this == &other) return *this;
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	return *this;
}

} // namespace opengl
} // namespace gintonic