#include "vertex_array_object.hpp"

namespace gintonic {
namespace opengl {

vertex_array_object::vertex_array_object() : m_handle(0)
{
	glGenVertexArrays(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
}

vertex_array_object& vertex_array_object::operator = (vertex_array_object&& other)
	BOOST_NOEXCEPT_OR_NOTHROW
{
	glDeleteVertexArrays(1, &m_handle);
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

} // namespace opengl
} // namespace gintonic