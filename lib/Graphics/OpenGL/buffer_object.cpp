#include "buffer_object.hpp"

namespace gintonic {
namespace opengl {

buffer_object::buffer_object() : m_handle(0)
{
	glGenBuffers(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
}

buffer_object::buffer_object(const GLenum target, const GLsizei size, 
	const GLvoid* data, const GLenum usage) : m_handle(0)
{
	glGenBuffers(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
	glBindBuffer(target, m_handle);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
}

buffer_object::buffer_object(const buffer_object& other) : m_handle(0)
{
	glGenBuffers(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
	GLint size;
	GLint usage;
	glBindBuffer(GL_COPY_READ_BUFFER, other.m_handle);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_handle);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
	glBufferData(GL_COPY_WRITE_BUFFER, static_cast<GLsizei>(size), 
		nullptr, static_cast<GLenum>(usage));
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 
		0, 0, size);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
}

buffer_object& buffer_object::operator=(const buffer_object& other)
{
	GLint size;
	GLint usage;
	glBindBuffer(GL_COPY_READ_BUFFER, other.m_handle);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_handle);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
	glBufferData(GL_COPY_WRITE_BUFFER, static_cast<GLsizei>(size), 
		nullptr, static_cast<GLenum>(usage));
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 
		0, 0, size);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	return *this;
}
buffer_object& buffer_object::operator=(buffer_object&& other) noexcept
{
	glDeleteBuffers(1, &m_handle);
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

} // namespace opengl
} // namespace gintonic