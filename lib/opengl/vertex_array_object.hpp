/**
 * @file vertex_array_object.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_vertex_array_object_hpp
#define gintonic_opengl_vertex_array_object_hpp

#include "utilities.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief Encapsulates an 
 * [OpenGL Vertex Array Object](https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object).
 */
class vertex_array_object
{
private:
	GLuint m_handle;
public:

	/// Get the underlying OpenGL handle with a static_cast.
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}

	/// Default constructor.
	vertex_array_object();

	/// Destructor.
	inline ~vertex_array_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(1, &m_handle);
	}

	/// You cannot copy vertex array objects.
	vertex_array_object(const vertex_array_object&) = delete;

	/// Move constructor.
	inline vertex_array_object(vertex_array_object&& other)
		BOOST_NOEXCEPT_OR_NOTHROW
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	/// You cannot copy vertex array objects.
	vertex_array_object& operator = (const vertex_array_object&) = delete;
	
	/// Move assignment operator.
	vertex_array_object& operator = (vertex_array_object&& other)
		BOOST_NOEXCEPT_OR_NOTHROW;

	// template <class VertexType>
	// void setup(const GLuint array_buffer) const 
	// 	BOOST_NOEXCEPT_OR_NOTHROW
	// {
	// 	glBindVertexArray(m_handle);
	// 	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	// 	VertexType::enable_attributes();
	// }

	// template <class VertexType>
	// void setup(const GLuint array_buffer, 
	// 	const GLuint element_array_buffer) const BOOST_NOEXCEPT_OR_NOTHROW
	// {
	// 	glBindVertexArray(m_handle);
	// 	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
	// 	VertexType::enable_attributes();
	// }

	// template <class ContiguousArrayContainer>
	// void setup(const GLuint array_buffer, 
	// 	const ContiguousArrayContainer& vertices, const GLenum usage) const
	// {
	// 	glBindVertexArray(m_handle);
	// 	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	// 	ContiguousArrayContainer::value_type::enable_attributes();
	// 	gtBufferData(GL_ARRAY_BUFFER, vertices, usage);
	// }

	// template <class ContiguousVerticesContainer,
	// 	class ContiguousIndicesContainer>
	// void setup(const GLuint array_buffer,
	// 	const GLuint element_array_buffer,
	// 	const ContiguousVerticesContainer& vertices,
	// 	const ContiguousIndicesContainer& indices, 
	// 	const GLenum usage) const
	// {
	// 	glBindVertexArray(m_handle);
	// 	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
	// 	ContiguousVerticesContainer::value_type::enable_attributes();
	// 	gtBufferData(GL_ARRAY_BUFFER, vertices, usage);
	// 	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, usage);
	// }

	// template <class ContiguousVerticesContainer,
	// 	class ContiguousIndicesContainer>
	// void setup(const GLuint array_buffer,
	// 	const GLuint element_array_buffer,
	// 	const ContiguousVerticesContainer& vertices,
	// 	const ContiguousIndicesContainer& indices, 
	// 	const GLenum vertices_usage,
	// 	const GLenum indices_usage) const
	// {
	// 	glBindVertexArray(m_handle);
	// 	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
	// 	ContiguousVerticesContainer::value_type::enable_attributes();
	// 	gtBufferData(GL_ARRAY_BUFFER, vertices, vertices_usage);
	// 	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, indices_usage);
	// }
};


} // namespace opengl
} // namespace gintonic

#endif