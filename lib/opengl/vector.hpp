#ifndef gintonic_opengl_vector_hpp
#define gintonic_opengl_vector_hpp

#include "buffer_object.hpp"
#include "buffer_object_array.hpp"

namespace gintonic {
namespace opengl {

template <GLenum Target, class T> class vector
{
public:

	vector(const GLenum usagehint)
	{
		bind();
		glBufferData(Target, sizeof(T) * m_reserved, nullptr, usagehint);
	}

	vector(const GLsizei reserve, const GLenum usagehint)
	: m_reserved(reserve)
	{
		bind();
		glBufferData(Target, sizeof(T) * m_reserved, nullptr, usagehint);
	}

	template <class Alloc>
	vector(const std::vector<T,Alloc>& v, const GLenum usagehint)
	: m_count(static_cast<GLsizei>(v.size()))
	, m_reserved(static_cast<GLsizei>(v.size()))
	{
		gtBufferData(Target, m_vbo, v, usagehint);
	}

	inline GLsizei size() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_count;
	}

	inline GLsizei reserved() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_reserved;
	}

	inline void bind() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindBuffer(Target, m_vbo);
	}

	template <class Alloc>
	void set(const std::vector<T,Alloc>& v, const GLenum usagehint)
	{
		m_count = static_cast<GLsizei>(v.size());
		if (m_count < m_reserved) gtBufferSubData(Target, 0, m_count, v);
		else
		{
			while (m_count >= m_reserved) m_reserved *= 2;
			glBufferData(Target, sizeof(T) * m_reserved, nullptr, usagehint);
			gtBufferSubData(Target, 0, m_count, v);
		}
	}

	inline void clear() BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_count = 0;
	}

private:
	buffer_object m_vbo;
	GLsizei m_count = 0;
	GLsizei m_reserved = 1;
};

template <GLenum Target, class T, GLuint Size> class vector_array
{
public:

	vector_array(const GLenum usagehint)
	{
		for (GLuint i = 0; i < Size; ++i)
		{
			bind(i);
			m_count[i] = 0;
			m_reserved[i] = 1;
			glBufferData(Target, sizeof(T) * m_reserved[i], nullptr, usagehint);	
		}
		
	}

	vector_array(const GLsizei reserve, const GLenum usagehint)
	{
		for (GLuint i = 0; i < Size; ++i)
		{
			bind(i);
			m_count[i] = 0;
			m_reserved[i] = reserve;
			glBufferData(Target, sizeof(T) * m_reserved[i], nullptr, usagehint);
		}

	}

	// template <class Alloc>
	// vector_array(const std::vector<T,Alloc>& v, const GLenum usagehint)
	// : m_count(static_cast<GLsizei>(v.size()))
	// , m_reserved(static_cast<GLsizei>(v.size()))
	// {
	// 	gtBufferData(Target, m_vbo, v, usagehint);
	// }

	inline GLsizei size(const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_count[index];
	}

	inline GLsizei reserved(const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_reserved[index];
	}

	inline void bind(const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindBuffer(Target, m_vbo[index]);
	}

	template <class Alloc>
	void set(
		const GLuint index, 
		const std::vector<T,Alloc>& v,
		const GLenum usagehint)
	{
		m_count[index] = static_cast<GLsizei>(v.size());
		if (m_count[index] < m_reserved[index])
		{
			gtBufferSubData(Target, 0, m_count[index], v);
		}
		else
		{
			while (m_count[index] >= m_reserved[index]) 
				m_reserved[index] *= 2;
			
			glBufferData(Target, sizeof(T) * m_reserved[index], 
				nullptr, usagehint);

			gtBufferSubData(Target, 0, m_count[index], v);
		}
	}

	inline void clear(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_count[index] = 0;
	}

	inline void clear_all() BOOST_NOEXCEPT_OR_NOTHROW
	{
		for (GLuint i = 0; i < Size; ++i) m_count[i] = 0;
	}

private:
	buffer_object_array<Size> m_vbo;
	GLsizei m_count[Size];
	GLsizei m_reserved[Size];
};

}} // namespace gintonic::opengl

#endif