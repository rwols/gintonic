#ifndef gintonic_opengl_vertex_array_object_array_hpp
#define gintonic_opengl_vertex_array_object_array_hpp

#include "utilities.hpp"

namespace gintonic {
namespace opengl {

template <GLuint Size> class vertex_array_object_array
{
public:

	inline vertex_array_object_array()
	{
		glGenVertexArrays(Size, m_handles);
	}

	inline ~vertex_array_object_array() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(Size, m_handles);
	}

	vertex_array_object_array(const vertex_array_object_array&) = delete;

	vertex_array_object_array(vertex_array_object_array&& other);

	vertex_array_object_array& operator = (const vertex_array_object_array&) = delete;

	vertex_array_object_array& operator = (vertex_array_object_array&& other);

	inline GLuint operator[](const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW { return m_handles[index]; }

private:
	GLuint m_handles[Size];
};

template <class GLuint Size>
vertex_array_object_array<Size>::vertex_array_object_array(vertex_array_object_array<Size>&& other)
{
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
}

template <class GLuint Size>
vertex_array_object_array<Size>& vertex_array_object_array<Size>::operator = (vertex_array_object_array<Size>&& other)
{
	if (this == &other) return *this;
	std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
	std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	return *this;
}

} // namespace opengl
} // namespace gintonic