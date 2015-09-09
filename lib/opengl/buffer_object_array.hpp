/**
 * @file buffer_object_array.hpp
 * @brief Defines an array of OpenGL Buffer Objects.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_buffer_object_array_hpp
#define gintonic_opengl_buffer_object_array_hpp

#include "utilities.hpp"
#include <cstring> // for std::memcpy and std::memmove

namespace gintonic {
namespace opengl {

/**
 * @brief An array of 
 * [OpenGL buffer object](https://www.opengl.org/wiki/Buffer_Object)s.
 * 
 * @tparam Size The number of buffer objects in the array.
 */
template <GLuint Size> class buffer_object_array
{
public:

	/// Default constructor.
	inline buffer_object_array()
	{
		glGenBuffers(Size, m_handles);
	}

	/// Destructor.
	inline ~buffer_object_array() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(Size, m_handles);
	}

	/// You cannot copy buffer object arrays.
	buffer_object_array(const buffer_object_array&) = delete;

	/// You cannot copy buffer object arrays.
	buffer_object_array& operator = (const buffer_object_array&) = delete;

	/// Move constructor.
	buffer_object_array(buffer_object_array&& other);

	/// Move assignment operator.
	buffer_object_array& operator = (buffer_object_array&& other);

	/**
	 * @brief Access the underlying OpenGL handle at the specified index.
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

template <GLuint Size>
buffer_object_array<Size>::buffer_object_array(buffer_object_array<Size>&& other)
{
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
}

template <GLuint Size>
buffer_object_array<Size>& buffer_object_array<Size>::operator = (buffer_object_array<Size>&& other)
{
	if (this == &other) return *this;
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	return *this;
}

} // namespace opengl
} // namespace gintonic

#endif