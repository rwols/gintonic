#include "Graphics/OpenGL/BufferObject.hpp"

namespace gintonic {
namespace OpenGL {

BufferObject::BufferObject() : mHandle(0)
{
	glGenBuffers(1, &mHandle);
	if (!mHandle) throw std::bad_alloc();
}

BufferObject::BufferObject(const GLenum target, const GLsizei size, 
	const GLvoid* data, const GLenum usage) : mHandle(0)
{
	glGenBuffers(1, &mHandle);
	if (!mHandle) throw std::bad_alloc();
	glBindBuffer(target, mHandle);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
}

BufferObject::BufferObject(const BufferObject& other) : mHandle(0)
{
	glGenBuffers(1, &mHandle);
	if (!mHandle) throw std::bad_alloc();
	GLint size;
	GLint usage;
	glBindBuffer(GL_COPY_READ_BUFFER, other.mHandle);
	glBindBuffer(GL_COPY_WRITE_BUFFER, mHandle);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
	glBufferData(GL_COPY_WRITE_BUFFER, static_cast<GLsizei>(size), 
		nullptr, static_cast<GLenum>(usage));
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 
		0, 0, size);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
}

BufferObject& BufferObject::operator=(const BufferObject& other)
{
	GLint size;
	GLint usage;
	glBindBuffer(GL_COPY_READ_BUFFER, other.mHandle);
	glBindBuffer(GL_COPY_WRITE_BUFFER, mHandle);
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
BufferObject& BufferObject::operator=(BufferObject&& other) noexcept
{
	glDeleteBuffers(1, &mHandle);
	mHandle = other.mHandle;
	other.mHandle = 0;
	return *this;
}

} // namespace OpenGL
} // namespace gintonic