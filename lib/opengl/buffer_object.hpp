/**
 * @file buffer_object.hpp
 * @brief Defines the OpenGL Buffer Object class.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_buffer_object_hpp
#define gintonic_opengl_buffer_object_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

namespace gintonic {
namespace opengl {

/**
 * @brief Buffer object that encapsulates an 
 * [OpenGL buffer object](https://www.opengl.org/wiki/Buffer_Object).
 */
class buffer_object
{
private:
	GLuint m_handle;
public:
	
	/// You can access the underlying OpenGL handle via a static_cast.
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}

	/// Default constructor.
	buffer_object();

	/**
	 * @brief Constructor.
	 * @details The constructor makes the buffer_object act as an OpenGL
	 * Vertex Buffer. It binds the buffer as defined by the target parameter
	 * and fills the buffer with the contents located at the data pointer.
	 * 
	 * @param target The target of the binding point. e.g. GL_ARRAY_BUFFER.
	 * @param size The number of bytes located at the data pointer.
	 * @param data The raw data pointer.
	 * @param usage A usage hint. e.g. GL_STATIC_DRAW or GL_DYNAMIC_DRAW.
	 */
	buffer_object(const GLenum target, const GLsizei size, 
		const GLvoid* data, const GLenum usage);

	/**
	 * @brief Constructor.
	 * @details The constructor makes the buffer_object act as an OpenGL
	 * Vertex Buffer. It binds the buffer as defined by the target parameter
	 * and fills the buffer with the contents of the contiguous array.
	 * 
	 * @param target The target of the binding point. e.g. GL_ARRAY_BUFFER.
	 * @param data An STL-compliant contiguous array, like std::vector or
	 *        std::array.
	 * @param usage A usage hint. e.g. GL_STATIC_DRAW or GL_DYNAMIC_DRAW.
	 */
	template <class ContiguousArrayContainer> 
	buffer_object(
		const GLenum target, 
		const ContiguousArrayContainer& data, 
		const GLenum usage);

	/**
	 * @brief Copy constructor. (Yes, you can copy OpenGL buffers!)
	 * @details The copying is all done inside the GPU. There are no memory
	 * transfers from GPU memory to CPU memory (and vice versa).
	 * 
	 * Copying buffers is slow, so try to avoid it.
	 * 
	 * @param other Another buffer_object.
	 */
	buffer_object(const buffer_object& other);

	/**
	 * @brief Move constructor.
	 * @details Moving a buffer_object is very simple. We copy over the OpenGL
	 * handle and set the OpenGL handle of the other buffer_object to zero.
	 * When the destructor of the other buffer_object is called, it will do
	 * nothing.
	 * 
	 * @param other Another buffer_object.
	 */
	inline buffer_object(buffer_object&& other) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	/**
	 * @brief Copy assignment operator (Yes, you can copy OpenGL buffers!)
	 * @details The copying is all done inside the GPU. There are no memory
	 * transfers from GPU memory to CPU memory (and vice versa).
	 * 
	 * Copying buffers is slow, so try to avoid it.
	 * 
	 * @param other Another buffer_object.
	 * 
	 * @return *this.
	 */
	buffer_object& operator=(const buffer_object& other);

	/**
	 * @brief Move constructor.
	 * @details Moving a buffer_object is very simple. We copy over the OpenGL
	 * handle and set the OpenGL handle of the other buffer_object to zero.
	 * When the destructor of the other buffer_object is called, it will do
	 * nothing.
	 * 
	 * @param other Another buffer_object.
	 * 
	 * @return *this.
	 */
	buffer_object& operator=(buffer_object&& other) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Destructor.
	 * @details The destructor makes a call to the OpenGL API to delete the
	 * contents of the buffer via the encapsulated handle.
	 */
	inline ~buffer_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(1, &m_handle);
	}

	/**
	 * @brief Swap this buffer_object with another buffer_object.
	 * @param other Another buffer_object.
	 */
	inline void swap(buffer_object& other)
	{
		std::swap(m_handle, other.m_handle);
	}
	
private:

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive> void save(Archive& ar, 
		const unsigned /*version*/) const
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

	template <class Archive> void load(Archive& ar, 
		const unsigned /*version*/)
	{
		GLint usage;
		std::vector<char> data;
		ar & data & usage;
		glBindBuffer(GL_COPY_WRITE_BUFFER, m_handle);
		gtBufferData(GL_COPY_WRITE_BUFFER, data, static_cast<GLenum>(usage));
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

//!@cond
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
//!@endcond

} // namespace opengl
} // namespace gintonic

#endif