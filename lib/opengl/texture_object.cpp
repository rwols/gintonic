#include "texture_object.hpp"

namespace gintonic {
namespace opengl {

texture_object::texture_object() : m_handle(0)
{
	glGenTextures(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
}

texture_object& texture_object::operator=(texture_object&& other) BOOST_NOEXCEPT_OR_NOTHROW
{
	glDeleteTextures(1, &m_handle);
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

} // namespace opengl
} // namespace gintonic
