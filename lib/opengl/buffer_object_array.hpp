#ifndef gintonic_opengl_buffer_object_array_hpp
#define gintonic_opengl_buffer_object_array_hpp

#include "utilities.hpp"
#include <cstring> // for std::memcpy and std::memmove

namespace gintonic {
namespace opengl {

template <GLuint Size> class buffer_object_array
{
public:

	inline buffer_object_array()
	{
		glGenBuffers(Size, m_handles);
	}

	inline ~buffer_object_array() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(Size, m_handles);
	}

	buffer_object_array(const buffer_object_array&) = delete;

	buffer_object_array& operator = (const buffer_object_array&) = delete;

	buffer_object_array(buffer_object_array&& other);

	buffer_object_array& operator = (buffer_object_array&& other);

	inline GLuint operator[](const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW { return m_handles[index]; }
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