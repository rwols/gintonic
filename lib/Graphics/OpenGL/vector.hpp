/**
 * @file vector.hpp
 * @brief Defines the OpenGL vector class wrapper and the array vector class
 * wrapper.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_vector_hpp
#define gintonic_opengl_vector_hpp

#include "buffer_object.hpp"
#include "buffer_object_array.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief A vector class for buffering data to GPU memory.
 * 
 * @tparam Target Specifies the target of the buffer. For instance,
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * @tparam T Specifies the type of element to store.
 */
template <GLenum Target, class T> class vector
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param usagehint Specifies the usage hint.
	 */
	vector(const GLenum usagehint)
	{
		bind();
		glBufferData(Target, sizeof(T) * m_reserved, nullptr, usagehint);
	}

	/**
	 * @brief Constructor.
	 * 
	 * @param reserve Reserve this many elements.
	 * @param usagehint Specifies the usage hint.
	 */
	vector(const GLsizei reserve, const GLenum usagehint)
	: m_reserved(reserve)
	{
		bind();
		glBufferData(Target, sizeof(T) * m_reserved, nullptr, usagehint);
	}

	/**
	 * @brief Constructor.
	 * 
	 * @tparam Alloc The allocator type of the std::vector.
	 * @param v The std::vector to initialize this vector with.
	 * @param usagehint Specifies the usage hint.
	 */
	template <class Alloc>
	vector(const std::vector<T,Alloc>& v, const GLenum usagehint)
	: m_count(static_cast<GLsizei>(v.size()))
	, m_reserved(static_cast<GLsizei>(v.size()))
	{
		gtBufferData(Target, m_vbo, v, usagehint);
	}

	/**
	 * @brief Get the size of the vector.
	 * @return The size of the vector.
	 */
	inline GLsizei size() const noexcept
	{
		return m_count;
	}

	/**
	 * @brief Get the number of reserved elements of the vector.
	 * @return The number of reserved elements of the vector.
	 */
	inline GLsizei reserved() const noexcept
	{
		return m_reserved;
	}

	/**
	 * @brief Bind the vector.
	 * @details the Target template parameter will be used for the target.
	 */
	inline void bind() const noexcept
	{
		glBindBuffer(Target, m_vbo);
	}

	/**
	 * @brief Set the contents of this vector from an std::vector.
	 * 
	 * @details This method tries to use glBufferSubData as much as possible,
	 * and only uses glBufferData if there is not enough space. The
	 * reallocation strategy is to increase the size of the vector by 2.
	 * 
	 * @param v The std::vector to set the vector with.
	 * @param usagehint Specifies the usage hint.
	 * @tparam Alloc The allocator type of the std::vector.
	 */
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

	/**
	 * @brief Clear this vector of all its elements.
	 */
	inline void clear() noexcept
	{
		m_count = 0;
	}

private:
	buffer_object m_vbo;
	GLsizei m_count = 0;
	GLsizei m_reserved = 1;
};

/**
 * @brief An array of vector classes for buffering data to GPU memory.
 * 
 * @tparam Target Specifies the target of the buffer. For instance,
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * @tparam T Specifies the type of element to store.
 * @tparam Size Specifies the number of vectors to use.
 */
template <GLenum Target, class T, GLuint Size> class vector_array
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param usagehint Specifies the usage hint.
	 */
	vector_array(const GLenum usagehint)
	{
		for (GLuint i = 0; i < Size; ++i)
		{
			bind(i);
			m_count[i] = 0;
			m_reserved[i] = 1;
			glBufferData(Target, sizeof(T) * m_reserved[i], nullptr, 
				usagehint);	
		}
	}

	/**
	 * @brief Constructor.
	 * 
	 * @param reserve Reserve this many elements.
	 * @param usagehint Specifies the usage hint.
	 */
	vector_array(const GLsizei reserve, const GLenum usagehint)
	{
		for (GLuint i = 0; i < Size; ++i)
		{
			bind(i);
			m_count[i] = 0;
			m_reserved[i] = reserve;
			glBufferData(Target, sizeof(T) * m_reserved[i], nullptr, 
				usagehint);
		}
	}

	/**
	 * @brief Get the size of one of the vectors in the array.
	 * 
	 * @param index The index into the array.
	 * @return The size of the vector at the index location.
	 */
	inline GLsizei size(const GLuint index) const noexcept
	{
		return m_count[index];
	}

	/**
	 * @brief Get the reserved size of one of the vectors in the array.
	 * 
	 * @param index The index into the array.
	 * @return The reserved size of the vector at the index location.
	 */
	inline GLsizei reserved(const GLuint index) const 
		noexcept
	{
		return m_reserved[index];
	}

	/**
	 * @brief Bind the vector at the given index in the array.
	 * @details the Target template parameter will be used for the target.
	 * @param index The index into the array of vectors.
	 */
	inline void bind(const GLuint index) const noexcept
	{
		glBindBuffer(Target, m_vbo[index]);
	}

	/**
	 * @brief Set the contents of the vector at a given index from an 
	 * std::vector.
	 * 
	 * @details This method tries to use glBufferSubData as much as possible,
	 * and only uses glBufferData if there is not enough space. The
	 * reallocation strategy is to increase the size of the vector by 2.
	 * 
	 * @param index The index into the array of vectors.
	 * @param v The std::vector to set the vector with.
	 * @param usagehint Specifies the usage hint.
	 * @tparam Alloc The allocator type of the std::vector.
	 */
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

	/**
	 * @brief Clear the vector at the index position of the vector array.
	 * 
	 * @param index The index into the array of vectors.
	 */
	inline void clear(const GLuint index) noexcept
	{
		m_count[index] = 0;
	}

	/**
	 * @brief Clear all vectors in the array of vectors.
	 */
	inline void clear_all() noexcept
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