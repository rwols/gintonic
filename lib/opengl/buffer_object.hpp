#ifndef gintonic_opengl_buffer_object_hpp
#define gintonic_opengl_buffer_object_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

namespace gintonic {
namespace opengl {

class buffer_object
{
private:
	GLuint m_handle;
public:
	
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}

	buffer_object();

	buffer_object(const GLenum target, const GLsizei size, 
		const GLvoid* data, const GLenum usage);

	template <class ContiguousArrayContainer> 
	buffer_object(
		const GLenum target, 
		const ContiguousArrayContainer& data, 
		const GLenum usage);

	buffer_object(const buffer_object& other);

	inline buffer_object(buffer_object&& other) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	buffer_object& operator=(const buffer_object& other);

	buffer_object& operator=(buffer_object&& other) BOOST_NOEXCEPT_OR_NOTHROW;

	inline ~buffer_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(1, &m_handle);
	}

	inline void swap(buffer_object& other)
	{
		std::swap(m_handle, other.m_handle);
	}
	
private:

	friend boost::serialization::access;
	template <class Archive> void save(Archive& ar, const unsigned /*version*/) const
	{
		GLint size;
		GLint usage;
		glBindBuffer(GL_COPY_READ_BUFFER, m_handle);
		glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
		glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
		std::vector<char> data(size);
		glGetBufferSubData(GL_COPY_READ_BUFFER, 0, size, (GLvoid*)data.data());
		ar & data & usage;
	}

	template <class Archive> void load(Archive& ar, const unsigned /*version*/)
	{
		GLint usage;
		std::vector<char> data;
		ar & data & usage;
		glBindBuffer(GL_COPY_WRITE_BUFFER, m_handle);
		gtBufferData(GL_COPY_WRITE_BUFFER, data, static_cast<GLenum>(usage));
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

template <class C> 
buffer_object::buffer_object(
	const GLenum target, 
	const C& data, 
	const GLenum usage) 
: m_handle(0)
{
	glGenBuffers(1, &m_handle);
	if (!m_handle) throw std::bad_alloc();
	glBindBuffer(target, m_handle);
	gtBufferData(target, data, usage);
	glBindBuffer(target, 0);
}

} // namespace opengl
} // namespace gintonic

#endif