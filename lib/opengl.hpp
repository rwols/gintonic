/*!
\file opengl.hpp
\author Raoul Wols
\date 17/09/2014
\brief Classes and functions dealing with the OpenGL API.
*/

#ifndef gintonic_opengl_hpp
#define gintonic_opengl_hpp

#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cassert>

#include "math.hpp"
#include "exception.hpp"
#include "object.hpp"
#include "tuple.hpp"
#include "filesystem.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	#pragma clang diagnostic ignored "-Wdocumentation"
#endif

#ifdef BOOST_MSVC
	#include <windows.h>
#endif

#include "glad.hpp"

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

//////////////////////////////////////////////////////////////////////
// For debugging purposes. Put this after a call to the opengl api. //
//////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
	#ifndef DEBUG_GL_ERROR
	#define DEBUG_GL_ERROR                                            \
		std::cerr << __FILE__ << ':' << __LINE__ << ": ";             \
		switch(glGetError())                                          \
		{                                                             \
		case GL_NO_ERROR:                                             \
			std::cerr << "GL_NO_ERROR\n"; break;                      \
		case GL_INVALID_ENUM:                                         \
			std::cerr << "GL_INVALID_ENUM\n"; break;                  \
		case GL_INVALID_VALUE:                                        \
			std::cerr << "GL_INVALID_VALUE\n"; break;                 \
		case GL_INVALID_OPERATION:                                    \
			std::cerr << "GL_INVALID_OPERATION\n"; break;             \
		case GL_INVALID_FRAMEBUFFER_OPERATION:                        \
			std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION\n"; break; \
		case GL_OUT_OF_MEMORY:                                        \
			std::cerr << "GL_OUT_OF_MEMORY\n"; break;                 \
		}
	#endif

	#ifndef DEBUG_PRINT
		#define DEBUG_PRINT std::cerr << __FILE__ << ':' << __LINE__ << '\n';
	#endif

#else
	#ifndef DEBUG_GL_ERROR
		#define DEBUG_GL_ERROR // as nothing
	#endif

	#ifndef DEBUG_PRINT
		#define DEBUG_PRINT // as nothing
	#endif

#endif

#ifndef DEBUG_PRINT
	#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
	#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
	#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif


void glDebugPrintBuffers(const GLint expected_vao, const GLint expected_vbo, const GLint expected_ibo);

/*!
\brief Convenience function to upload an std::vector of data to an OpenGL buffer object.
\tparam T The value type that the std::vector holds. This parameter is deduced by the compiler.
\param [in] target The target.
\param [in] v The data to upload.
\param [in] usage The usage hint for OpenGL.

This function is equivalent to calling
\code
glBufferData(target, v.size() * sizeof(T), v.data(), usage);
\endcode
*/

template <class ContiguousArrayContainer>
inline void gtBufferData(const GLenum target, 
	const ContiguousArrayContainer& v, const GLenum usage)
{
	glBufferData(target, v.size() 
		* sizeof(typename ContiguousArrayContainer::value_type), 
		reinterpret_cast<const GLvoid*>(v.data()), usage);
}

// template <class T, class AllocT> 
// inline void gtBufferData(const GLenum target, const std::vector<T, AllocT>& v,
// 	const GLenum usage)
// {
// 	glBufferData(target, v.size() * sizeof(T), v.data(), usage);
// }

/*!
\brief Convenience function to upload an std::vector to a part of an OpenGL buffer object.
\tparam T The value type that the std::vector holds. This parameter is deduced by the compiler.
\param [in] target The target.
\param [in] element_offset The element offset into the buffer. Note that this is **not** the byte offset.
The convenience function will take care of that, given the size of T.
\param [in] num_elements The number of elements to upload from the std::vector.
\param [in] v The data to upload.

The OpenGL buffer object will be filled with num_elements from v starting at position element_offset.
Note that the data *will be read from the start of v*. If you want to upload all the contents of v,
then set num_elements to v.size().

This function is equivalent to calling
\code
glBufferSubData(target, sizeof(T) * element_offset, sizeof(T) * num_elements, v.data());
\endcode
*/
template <class T, class Alloc> inline void gtBufferSubData(const GLenum target,
	const std::size_t element_offset,
	const std::size_t num_elements,
	const std::vector<T, Alloc>& v)
{
	glBufferSubData(target, sizeof(T) * element_offset, sizeof(T) * num_elements, v.data());
}

namespace gintonic {

class material;
class light;

namespace opengl {

/*!
\brief Get the major and minor version of the current OpenGL context.

After calling this function, major will contain the major version number and minor will
contain the minor version number.

\param [out] major The major version.
\param [out] minor The minor version.

\sa driver(std::string& vendor, std::string& renderer, std::string& version, 
std::string& shading_language_version, std::vector<std::string>& extension)
*/
void driverVersion(int& major, int& minor);

/*!
\brief Obtain various strings of information from the current OpenGL context.

\param [out] vendor The name of the vendor of the driver.
\param [out] renderer The name of the renderer.
\param [out] version The version as a string.
\param [out] shading_language_version The version number of GLSL as a string.
\param [out] extensions The function names of all available extensions in this context.

\sa driverVersion(int& major, int& minor)
*/
void driver
(
	std::string& vendor,
	std::string& renderer,
	std::string& version,
	std::string& shading_language_version,
	std::vector<std::string>& extensions
);

/*!
\brief Simple structure to obtain information about the CPU.

After construction of an instance, you can query that instance for
various features of the CPU. You do not have to call cpu_features::query() first.
This is only necessary if cpu_features::clear() has been called before.
*/
struct cpu_features
{
	bool x64; //!< Wether this CPU is 64-bits.
	bool mmx; //!< Wether this CPU supports MMX instructions.
	bool sse; //!< Wether this CPU supports SSE instructions.
	bool sse2; //!< Wether this CPU supports SSE2 instructions.
	bool sse3; //!< Wether this CPU supports SSE3 instructions.
	bool ssse3; //!< Wether this CPU supports SSSE3 instructions.
	bool sse41; //!< Wether this CPU supports SSE4.1 instructions.
	bool sse42; //!< Wether this CPU supports SSE4.2 instructions.
	bool sse4a; //!< Wether this CPU supports SSE4a instructions.
	bool avx; //!< Wether this CPU supports AVX instructions.
	bool xop; //!< Wether this CPU supports XOP instructions.
	bool fma3; //!< Wether this CPU supports FMA3 instructions.
	bool fma4; //!< Wether this CPU supposrts FMA4 instructions.

	/*!
	\brief Clear all the boolean values in this struct.
	*/
	void clear();

	/*!
	\brief Query all the boolean values in this struct.
	*/
	void query();

	/*!
	\brief The default constructor fills all the boolean values of this struct.

	There is no need to call cpu_features::query().
	*/
	cpu_features();

	/*!
	\brief Pretty-print the CPU features.

	\param [in,out] os The output stream to print to.
	\param [in] feat A cpu_features structure.
	\return The same output stream.
	*/
	friend std::ostream& operator << (std::ostream& os, const cpu_features& feat);
};

class buffer_object
{
private:
	GLuint m_handle;
public:
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}
	inline buffer_object() : m_handle(0)
	{
		glGenBuffers(1, &m_handle);
		if (!m_handle) throw std::bad_alloc();
	}
	buffer_object(const GLenum target, const GLsizei size, 
		const GLvoid* data, const GLenum usage) : m_handle(0)
	{
		glGenBuffers(1, &m_handle);
		if (!m_handle) throw std::bad_alloc();
		glBindBuffer(target, m_handle);
		glBufferData(target, size, data, usage);
		glBindBuffer(target, 0);
	}
	template <class ContiguousArrayContainer> 
	buffer_object(const GLenum target, const ContiguousArrayContainer& data, 
		const GLenum usage) : m_handle(0)
	{
		glGenBuffers(1, &m_handle);
		if (!m_handle) throw std::bad_alloc();
		glBindBuffer(target, m_handle);
		gtBufferData(target, data, usage);
		glBindBuffer(target, 0);
	}
	buffer_object(const buffer_object& other) : m_handle(0)
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
	inline buffer_object(buffer_object&& other) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}
	buffer_object& operator=(const buffer_object& other)
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
	buffer_object& operator=(buffer_object&& other) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(1, &m_handle);
		m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}
	inline ~buffer_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(1, &m_handle);
	}
	void swap(buffer_object& other)
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

class texture_object
{
private:
	GLuint m_handle;
public:
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}
	inline texture_object() : m_handle(0)
	{
		glGenTextures(1, &m_handle);
		if (!m_handle) throw std::bad_alloc();
	}
	texture_object(const texture_object& other) = delete;
	inline texture_object(texture_object&& other) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}
	texture_object& operator=(const texture_object& other) = delete;
	texture_object& operator=(texture_object&& other)
	BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteTextures(1, &m_handle);
		m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}
	inline ~texture_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteTextures(1, &m_handle);
	}
};

template <GLuint Size> class buffer_object_array
{
public:

	/*!
	\brief Constructor generates Size buffer_object's.

	Equivalent to
	\code
	BOOST_CONSTEXPR std::size_t Size = ...;
	GLuint buffer_object_array[Size];
	glGenBuffers(Size, buffer_object_array);
	\endcode
	*/
	inline buffer_object_array()
	{
		glGenBuffers(Size, m_handles);
	}

	/*!
	\brief Destructor.

	Equivalent to
	\code
	glDeleteBuffers(Size, buffer_object_array);
	\endcode
	*/
	inline ~buffer_object_array() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(Size, m_handles);
	}

	buffer_object_array(const buffer_object_array&) = delete;

	/*!
	\brief Move-constructor.
	\param [in,out] other Another buffer_object_array.

	After this call, the other buffer_object is in a void state, so do not use
	it anymore.
	*/
	inline buffer_object_array(buffer_object_array&& other)
	{
		std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
		std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	}

	buffer_object_array& operator = (const buffer_object_array&) = delete;

	/*!
	\brief Move-assignment operator.
	\param [in,out] other Another buffer_object_array.

	After this call, the other buffer_object is in a void state, so do not use
	it anymore.
	*/
	inline buffer_object_array& operator = (buffer_object_array&& other)
	{
		if (this == &other) return *this;
		std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
		std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
		return *this;
	}

	/*!
	\brief Get the underlying GLuint handle based on an index.
	\param [in] index The index to the handle.
	\return The index'th handle.

	This method will not check for out-of-bounds.
	*/
	inline GLuint operator[](const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW { return m_handles[index]; }
private:
	GLuint m_handles[Size];
};

class vertex_array_object
{
private:
	GLuint m_handle;
public:
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}
	inline vertex_array_object() : m_handle(0)
	{
		glGenVertexArrays(1, &m_handle);
	}

	inline ~vertex_array_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(1, &m_handle);
	}

	vertex_array_object(const vertex_array_object&) = delete;

	inline vertex_array_object(vertex_array_object&& other)
		BOOST_NOEXCEPT_OR_NOTHROW
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	vertex_array_object& operator = (const vertex_array_object&) = delete;
	
	inline vertex_array_object& operator = (vertex_array_object&& other)
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(1, &m_handle);
		m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}

	template <class VertexType>
	void setup(const GLuint array_buffer) const 
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
		VertexType::enable_attributes();
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}

	template <class VertexType>
	void setup(const GLuint array_buffer, 
		const GLuint element_array_buffer) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
		VertexType::enable_attributes();
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}

	template <class ContiguousArrayContainer>
	void setup(const GLuint array_buffer, 
		const ContiguousArrayContainer& vertices, const GLenum usage) const
	{
		glBindVertexArray(m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
		ContiguousArrayContainer::value_type::enable_attributes();
		gtBufferData(GL_ARRAY_BUFFER, vertices, usage);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}

	template <class ContiguousVerticesContainer,
		class ContiguousIndicesContainer>
	void setup(const GLuint array_buffer,
		const GLuint element_array_buffer,
		const ContiguousVerticesContainer& vertices,
		const ContiguousIndicesContainer& indices, 
		const GLenum usage) const
	{
		glBindVertexArray(m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
		ContiguousVerticesContainer::value_type::enable_attributes();
		gtBufferData(GL_ARRAY_BUFFER, vertices, usage);
		gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, usage);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}

	template <class ContiguousVerticesContainer,
		class ContiguousIndicesContainer>
	void setup(const GLuint array_buffer,
		const GLuint element_array_buffer,
		const ContiguousVerticesContainer& vertices,
		const ContiguousIndicesContainer& indices, 
		const GLenum vertices_usage,
		const GLenum indices_usage) const
	{
		glBindVertexArray(m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
		ContiguousVerticesContainer::value_type::enable_attributes();
		gtBufferData(GL_ARRAY_BUFFER, vertices, vertices_usage);
		gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, indices_usage);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}
};

/*!
\brief A compile-time size array of vertex_array_object's.

If you need more than one vertex_array_object and you know
at compile time how many you need, then using this class
improves performance a little bit. This class is templated
on the number of buffers that you need.

\tparam Size The number of vertex_array_object's to create.

\sa vertex_array_object
\sa vertex_array_object
\sa vertex_array_object_array
*/
template <GLuint Size> class vertex_array_object_array
{
public:

	/*!
	\brief Constructor generates Size vertex_array_object's.

	Equivalent to
	\code
	BOOST_CONSTEXPR std::size_t Size = ...;
	GLuint vertex_array_object_array[Size];
	glGenBuffers(Size, vertex_array_object_array);
	\endcode
	*/
	inline vertex_array_object_array()
	{
		glGenVertexArrays(Size, m_handles);
	}

	/*!
	\brief Destructor.

	Equivalent to
	\code
	glDeleteBuffers(Size, vertex_array_object_array);
	\endcode
	*/
	~vertex_array_object_array() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(Size, m_handles);
	}

	vertex_array_object_array(const vertex_array_object_array&) = delete;

	/*!
	\brief Move-constructor.
	\param [in,out] other Another vertex_array_object_array.

	After this call, the other vertex_array_object_array is in a void state, so do not use
	it anymore.
	*/
	inline vertex_array_object_array(vertex_array_object_array&& other)
	{
		std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
		std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
	}

	vertex_array_object_array& operator = (const vertex_array_object_array&) = delete;

	/*!
	\brief Move-assignment operator.
	\param [in,out] other Another vertex_array_object_array.

	After this call, the other vertex_array_object_array is in a void state, so do not use
	it anymore.
	*/
	inline vertex_array_object_array& operator = (vertex_array_object_array&& other)
	{
		if (this == &other) return *this;
		std::memcpy(m_handles, other.m_handles, sizeof(GLuint) * Size);
		std::memset(other.m_handles, 0, sizeof(GLuint) * Size);
		return *this;
	}

	/*!
	\brief Get the underlying GLuint handle based on an index.
	\param [in] index The index to the handle.
	\return The index'th handle.

	This method will not check for out-of-bounds.
	*/
	inline GLuint operator[](const GLuint index) const BOOST_NOEXCEPT_OR_NOTHROW { return m_handles[index]; }


private:
	GLuint m_handles[Size];
};

class framebuffer
{
public:
	inline framebuffer()
	{
		glGenFramebuffers(1, &m_handle);
	}
	inline ~framebuffer()
	{
		glDeleteFramebuffers(1, &m_handle);
	}
	framebuffer(framebuffer&) = delete;
	framebuffer& operator = (framebuffer&) = delete;
	inline framebuffer(framebuffer&& other) : m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}
	framebuffer& operator = (framebuffer&& other)
	{
		this->~framebuffer();
		m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
private:
	GLuint m_handle;
};

template <GLenum Target, GLenum Access> class map_buffer
{
public:
	inline map_buffer()
	{
		glMapBuffer(Target, Access);
	}
	inline ~map_buffer() BOOST_NOEXCEPT_OR_NOTHROW
	{
		if (glUnmapBuffer(Target) == GL_FALSE)
			std::cerr << "warning: unsuccesfully unmapped buffer.";
	}
	inline void* data() const BOOST_NOEXCEPT_OR_NOTHROW { return m_data; }
	inline operator void*() const BOOST_NOEXCEPT_OR_NOTHROW { return m_data; }
private:
	void* m_data;
};

template
<
	GLenum Target,
	GLenum Usage,
	GLenum Mode,
	class T, 
	class Alloc = std::allocator<T>
> class lazy_vector
{
private:
	buffer_object m_vbo;
	std::vector<T, Alloc> m_cache;
	bool m_dirty;
	GLsizei m_count = 0;
	GLsizei m_buffer_size = 0;
public:
	typedef typename std::vector<T, Alloc>::value_type value_type;
	typedef typename std::vector<T, Alloc>::allocator_type allocator_type;
	typedef GLsizei size_type;
	typedef GLint difference_type;
	typedef typename std::vector<T, Alloc>::reference reference;
	typedef typename std::vector<T, Alloc>::const_reference const_reference;
	typedef typename std::vector<T, Alloc>::pointer pointer;
	typedef typename std::vector<T, Alloc>::const_pointer const_pointer;
	typedef typename std::vector<T, Alloc>::iterator iterator;
	typedef typename std::vector<T, Alloc>::const_iterator const_iterator;
	typedef typename std::vector<T, Alloc>::reverse_iterator reverse_iterator;
	typedef typename std::vector<T, Alloc>::const_reverse_iterator const_reverse_iterator;

	lazy_vector() = default;
	explicit lazy_vector(const size_type count, const T& value, const Alloc& alloc = Alloc())
	: m_cache(count, value, alloc)
	, m_dirty(true)
	{

	}
	explicit lazy_vector(const Alloc& alloc)
	: m_cache(alloc)
	, m_dirty(false)
	{

	}
	explicit lazy_vector(const size_type count)
	: m_cache(count)
	, m_dirty(true)
	{

	}
	template <class InputIt> lazy_vector(InputIt first, InputIt last, const Alloc& alloc = Alloc())
	: m_cache(first, last, alloc)
	, m_dirty(true)
	{

	}
	lazy_vector(const lazy_vector& other) = default;
	lazy_vector(const lazy_vector& other, const Alloc& alloc)
	: m_vbo(other.m_vbo)
	, m_cache(other.m_cache, alloc)
	, m_dirty(other.m_dirty)
	, m_count(other.m_count)
	, m_buffer_size(other.m_buffer_size)
	{

	}
	lazy_vector(lazy_vector&& other)
	: m_vbo(std::move(other.m_vbo))
	, m_cache(std::move(other.m_cache))
	, m_dirty(other.m_dirty)
	, m_count(other.m_count)
	, m_buffer_size(other.m_buffer_size)
	{

	}
	lazy_vector(lazy_vector&& other, const Alloc& alloc)
	: m_vbo(std::move(other.m_vbo))
	, m_cache(std::move(other.m_cache), alloc)
	, m_dirty(other.m_dirty)
	, m_count(other.m_count)
	, m_buffer_size(other.m_buffer_size)
	{

	}
	lazy_vector(std::initializer_list<T> init, const Alloc& alloc)
	: m_cache(init, alloc)
	, m_dirty(true)
	{

	}

	~lazy_vector() = default;

	lazy_vector& operator=(lazy_vector&) = default;
	lazy_vector& operator=(lazy_vector&& other)
	{
		m_vbo = std::move(other.m_vbo);
		m_cache = std::move(other.m_cache);
		m_dirty = other.m_dirty;
		return *this;
	}
	lazy_vector& operator=(std::initializer_list<T> ilist)
	{
		m_cache = ilist;
		m_dirty = true;
		return *this;
	}

	void assign(const size_type count, const T& value)
	{
		m_cache.assign(count, value);
		m_dirty = true;
	}

	template <class InputIt>
	void assign(InputIt first, InputIt last)
	{
		m_cache.assign(first, last);
		m_dirty = true;
	}

	void assign(std::initializer_list<T> ilist)
	{
		m_cache.assign(ilist);
		m_dirty = true;
	}

	inline allocator_type get_allocator() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.get_allocator();
	}

	inline reference at(const size_type pos)
	{
		m_dirty = true;
		return m_cache.at(pos);
	}

	inline const_reference at(const size_type pos) const
	{
		return m_cache.at(pos);
	}

	inline reference operator[](const size_type pos)
	{
		m_dirty = true;
		return m_cache[pos];
	}

	inline const_reference operator[](const size_type pos) const
	{
		m_dirty = true;
		return m_cache[pos];
	}

	inline reference front()
	{
		m_dirty = true;
		return m_cache.front();
	}

	inline const_reference front() const
	{
		return m_cache.front();
	}

	inline reference back()
	{
		m_dirty = true;
		return m_cache.back();
	}

	inline const_reference back() const
	{
		return m_cache.back();
	}

	inline T* data() BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_dirty = true;
		return m_cache.data();
	}

	inline const T* data() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.data();
	}

	inline iterator begin() BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_dirty = true;
		return m_cache.begin();
	}

	inline const_iterator begin() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.begin();
	}

	inline const_iterator cbegin() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.cbegin();
	}

	inline iterator end() BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_dirty = true;
		return m_cache.end();
	}

	inline const_iterator end() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.end();
	}

	inline const_iterator cend() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache,cend();
	}

	inline reverse_iterator rbegin() BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_dirty = true;
		return m_cache.rbegin();
	}

	inline const_reverse_iterator rbegin() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.rbegin();
	}

	inline const_reverse_iterator crbegin() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.crbegin();
	}

	inline reverse_iterator rend() BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_dirty = true;
		return m_cache.rend();
	}

	inline const_reverse_iterator rend() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.rend();
	}

	inline const_reverse_iterator crend() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.crend();
	}

	inline bool empty() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.empty();
	}

	inline size_type size() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.size();
	}

	inline size_type max_size() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return std::numeric_limits<GLsizei>::max();
	}

	inline void reserve(const size_type new_cap)
	{
		m_cache.reserve(new_cap);
	}

	inline size_type capacity() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_cache.capacity();
	}

	inline void shrink_to_fit()
	{
		m_cache.shrink_to_fit();
	}

	inline void clear()
	{
		m_dirty = true;
		m_cache.clear();
	}

	inline iterator insert(const_iterator pos, const T& value)
	{
		m_dirty = true;
		return m_cache.insert(pos, value);
	}

	inline iterator insert(const_iterator pos, T&& value)
	{
		m_dirty = true;
		return m_cache.insert(pos, std::move(value));
	}

	inline iterator insert(const_iterator pos, const size_type count, const T& value)
	{
		m_dirty = true;
		return m_cache.insert(pos, count, value);
	}

	template <class InputIt>
	inline iterator insert(const_iterator pos, InputIt first, InputIt last)
	{
		m_dirty = true;
		return m_cache.insert(pos, first, last);
	}

	inline iterator insert(const_iterator pos, std::initializer_list<T> ilist)
	{
		m_dirty = true;
		return m_cache.insert(pos, ilist);
	}

	template <class... Args> inline iterator emplace(const_iterator pos, Args&&... args)
	{
		m_dirty = true;
		return m_cache.emplace(pos, std::forward<Args>(args)...);
	}

	inline iterator erase(const_iterator pos)
	{
		m_dirty = true;
		return m_cache.erase(pos);
	}

	inline iterator erase(const_iterator first, const_iterator last)
	{
		m_dirty = true;
		return m_cache.erase(first, last);
	}

	inline void push_back(const T& value)
	{
		m_dirty = true;
		m_cache.push_back(value);
	}

	inline void push_back(T&& value)
	{
		m_dirty = true;
		m_cache.push_back(std::move(value));
	}

	template <class... Args> inline void emplace_back(Args&&... args)
	{
		m_dirty = true;
		m_cache.emplace_back(std::forward<Args>(args)...);
	}

	inline void pop_back()
	{
		m_dirty = true;
		m_cache.pop_back();
	}

	inline void resize(const size_type count)
	{
		m_dirty = true;
		m_cache.resize(count);
	}

	inline void resize(const size_type count, const value_type& value)
	{
		m_dirty = true;
		m_cache.resize(count, value);
	}

	template <GLenum OtherTarget, GLenum OtherMode, GLenum OtherUsage>
	void swap(lazy_vector<OtherTarget, OtherMode, OtherUsage, T, Alloc>& other)
	{
		m_vbo.swap(other.m_vbo);
		m_cache.swap(other.m_cache);
		std::swap(m_dirty, other.m_dirty);
		std::swap(m_count, other.m_count);
		std::swap(m_buffer_size, other.m_buffer_size);
	}

	void synchronize()
	{
		if (m_dirty)
		{
			glBindBuffer(Target, m_vbo);
			if (m_cache.size() <= m_buffer_size)
			{
				gtBufferSubData(Target, 0, m_cache.size(), m_cache);
			}
			else
			{
				gtBufferData(Target, m_cache, Usage);
				m_buffer_size = m_cache.size();
			}
			m_count = m_cache.size();
			m_dirty = false;
		}
	}

	void draw()
	{
		if (m_dirty)
		{
			glBindBuffer(Target, m_vbo);
			if (m_cache.size() <= m_buffer_size)
			{
				gtBufferSubData(Target, 0, m_cache.size(), m_cache);
			}
			else
			{
				gtBufferData(Target, m_cache, Usage);
				m_buffer_size = m_cache.size();
			}
			m_count = m_cache.size();
			m_dirty = false;
		}
		glDrawArrays(Mode, 0, m_count);
	}

	inline void draw() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDrawArrays(Mode, 0, m_count);
	}

	inline void draw_unsynchronized() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		draw();
	}

};

/*!
\brief Encapsulates a file containing GLSL source code.

All constructors of this class are private. You must use
the static methods source_code::from_file and source_code::from_memory
to obtain a source_code object. Furthermore, the copy constructor
and copy assignment operator are deleted. You can move source_code objects.
This class is mainly used for the construction of a shader. However,
one of the constructors of a shader simply takes the filenames to the
vertex shader, fragment shader and optionally to the geometry shader.
Thus, you do not have to use this class directly.
*/
class source_code
{
public:

	/*!
	\brief Constructs a source_code from a filename.
	\param [in] filename The path to the GLSL source code.
	\return A new source_code instance.
	\exception If the file doesn't exist or cannot be opened
	or is empty, throws std::runtime_error.
	*/
	static source_code from_file(const char* filename);

	/*!
	\brief Constructs a source_code from a filename.
	\param [in] filename The path to the GLSL source code.
	\return A new source_code instance.
	\exception If the file doesn't exist or cannot be opened
	or is empty, throws std::runtime_error.
	*/
	static source_code from_file(const std::string& filename);

	/*!
	\brief Constructs a source_code from a location in memory.
	\param [in] str The string containing the full source code.
	\return A new source_code instance.
	\exception If str is null, throws std::runtime_error.
	*/
	static source_code from_memory(const GLchar* str);

	/*!
	\brief Constructs a source_code from a file.
	\param [in] p The path to the file.
	\return A new source_code instance.
	\exception If p is not a regular file or if the contents is empty, throws std::runtime_error.
	*/
	source_code(const boost::filesystem::path& p);

	/*!
	\brief Copy constructor.
	\param [in] other Another source_code object.
	*/
	source_code(const source_code& other);

	/*!
	\brief Move constructor.

	After calling this constructor, the other source_code is
	in a void state, so do not use that object anymore.

	\param [in] other Another source_code object.
	*/
	source_code(source_code&& other);

	/*!
	\brief Copy assignment operator.
	\param [in] other Another source_code object.
	\return This.
	*/
	source_code& operator=(const source_code& other);

	/*!
	\brief Move assignment operator.

	After calling this constructor, the other source_code is
	in a void state, so do not use that object anymore.

	\param [in] other Another source_code object.
	\return This.
	*/
	source_code& operator=(source_code&& other);
	~source_code();

	/*!
	\brief Get the actual source code.

	What you get is an array of c-strings. Each c-string has a terminating null byte.
	To obtain the number of c-strings, use source_code::count().

	\return An array of c-strings, each c-string has a terminating null byte.
	\sa source_code::count()
	*/
	inline const GLchar** string() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_cast<const GLchar**>(m_string);
	}

	/*!
	\brief Get the number of lines in the source code.
	\return The number of lines in the source.
	\sa source_code::string()
	*/
	inline GLsizei count() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_count;
	}

private:
	GLchar** m_string;
	GLsizei m_count;
	// GLint* m_length;

	inline source_code(GLchar** string, GLsizei count)
	: m_string(string), m_count(count) {}
};

/*!
\brief Use this class to define new shader types.

This class is the skeleton of the classes

- vertex_shader
- fragment_shader
- geometry_shader

Since their functionality only differs by an enum.
It is recommended that you do not use this class directly,
but rather one of the aformentioned actual shaders.

\tparam Type The type, as an OpenGL enum, of shader.
*/
template <GLint Type> class basic_shader
{
public:

	struct error : virtual exception {};
	typedef boost::error_info<struct tag_basic_shader_errinfo, std::string> errinfo;
	typedef boost::error_info<struct tag_basic_shader_pathinfo, boost::filesystem::path> pathinfo;

	basic_shader(const boost::filesystem::path& p) : m_handle(glCreateShader(Type))
	{
		{
			const source_code source(p);
			glShaderSource(*this, source.count(), source.string(), nullptr);
		}
		GLint r;
		glCompileShader(*this);
		glGetShaderiv(*this, GL_COMPILE_STATUS, &r);
		if (r == GL_FALSE)
		{
			std::string error_msg;
			glGetShaderiv(*this, GL_INFO_LOG_LENGTH, &r);
			error_msg.resize(r);
			glGetShaderInfoLog(*this, r, nullptr, &error_msg[0]);
			glDeleteShader(*this);
			throw exception(std::move(error_msg));
		}
	}

	basic_shader(const source_code& source) : m_handle(glCreateShader(Type))
	{
		GLint r;
		glShaderSource(*this, source.count(), source.string(), nullptr);
		glCompileShader(*this);
		glGetShaderiv(*this, GL_COMPILE_STATUS, &r);
		if (r == GL_FALSE)
		{
			std::string error_msg;
			glGetShaderiv(*this, GL_INFO_LOG_LENGTH, &r);
			error_msg.resize(r);
			glGetShaderInfoLog(*this, r, nullptr, &error_msg[0]);
			glDeleteShader(*this);
			throw exception(std::move(error_msg));
		}
	}
	virtual ~basic_shader() BOOST_NOEXCEPT_OR_NOTHROW { glDeleteShader(m_handle); }

	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
private:
	GLuint m_handle;
};

typedef basic_shader<GL_VERTEX_SHADER>   vertex_shader;
typedef basic_shader<GL_FRAGMENT_SHADER> fragment_shader;
typedef basic_shader<GL_GEOMETRY_SHADER> geometry_shader;

class shader 
//: public object<shader, std::tuple<boost::filesystem::path, boost::filesystem::path, boost::filesystem::path>>
: public std::enable_shared_from_this<shader>
{
public:

	struct error : public exception {};
	struct link_error : public error {};
	struct uniform_not_found : public error {};
	struct attribute_not_found : public error {};
	struct not_present : public error {};

	// typedef boost::error_info<struct tag_link_errinfo, std::string> link_errinfo;
	// typedef boost::error_info<struct tag_uniform_errinfo, const char*> uniform_errinfo;
	// typedef boost::error_info<struct tag_attribute_errinfo, const char*> attribute_errinfo;
	// typedef boost::error_info<struct tag_shader_errinfo, key_type> shader_errinfo;

	struct uniform
	{
		std::basic_string<GLchar> name;
		GLuint location;
		GLint size;
		GLenum type;
		inline bool operator <  (const uniform& other) const BOOST_NOEXCEPT_OR_NOTHROW { return name < other.name; }
		inline bool operator >= (const uniform& other) const BOOST_NOEXCEPT_OR_NOTHROW { return !operator<(other); }
		inline bool operator >  (const uniform& other) const BOOST_NOEXCEPT_OR_NOTHROW { return name > other.name; }
		inline bool operator <= (const uniform& other) const BOOST_NOEXCEPT_OR_NOTHROW { return !operator>(other); }
	};

	typedef uniform attribute;

	// virtual void bind_matrices() const BOOST_NOEXCEPT_OR_NOTHROW = 0;
	// virtual void bind_material(std::shared_ptr<const material>) const BOOST_NOEXCEPT_OR_NOTHROW = 0;
	// virtual void bind_light(std::shared_ptr<const light>) const BOOST_NOEXCEPT_OR_NOTHROW = 0;

	virtual ~shader() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	shader(
		boost::filesystem::path vertex_shader_file,
		boost::filesystem::path fragment_shader_file);

	shader(
		boost::filesystem::path vertex_shader_file, 
		boost::filesystem::path geometry_shader_file, 
		boost::filesystem::path fragment_shader_file);

private:

	shader();

	GLuint m_handle;
	// static std::map<key_type, std::vector<boost::filesystem::path>> s_shader_map;
	// friend boost::flyweights::detail::optimized_key_value<key_type, shader, key_extractor>;
	
	// shader(const key_type& key);
	// shader(key_type&& key);
	// virtual void construct_from_key() final;
	
	// friend class boost::serialization::access;
	// template <class Archive> void serialize(Archive& ar, const unsigned int /*version*/)
	// {
	// 	ar & boost::serialization::base_object<object<shader, key_type>>(*this);
	// }

protected:
	GLint get_uniform_location(const GLchar* name) const;
	bool get_uniform_location(const GLchar* name, GLint& location) const BOOST_NOEXCEPT_OR_NOTHROW;
	GLint get_attrib_location(const GLchar* name) const;
	bool get_attrib_location(const GLchar* name, GLint& location) const BOOST_NOEXCEPT_OR_NOTHROW;
	GLint num_active_attributes() const BOOST_NOEXCEPT_OR_NOTHROW;
	GLint num_active_uniforms() const BOOST_NOEXCEPT_OR_NOTHROW;
	uniform get_uniform(const GLint index) const;
	attribute get_attribute(const GLint index) const;

	void set_uniform(const char* uniformName, const GLfloat value) const;
	void set_uniform(const char* uniformName, const vec2f& v) const;
	void set_uniform(const char* uniformName, const vec3f& v) const;
	void set_uniform(const char* uniformName, const vec4f& v) const;
	void set_uniform(const char* uniformName, const mat3f& m) const;
	void set_uniform(const char* uniformName, const mat4f& m) const;
	void set_uniform(const char* uniformName, const GLint i) const;

	inline void set_uniform1(const char* uniformName, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1f(get_uniform_location(uniformName), values[0]);
	}
	inline void set_uniform2(const char* uniformName, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform2f(get_uniform_location(uniformName), values[0], values[1]);
	}
	inline void set_uniform3(const char* uniformName, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform3f(get_uniform_location(uniformName), values[0], values[1], values[2]);
	}
	inline void set_uniform4(const char* uniformName, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform4f(get_uniform_location(uniformName), values[0], values[1], values[2], values[3]);
	}
	inline void set_uniform1(const char* uniformName, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1i(get_uniform_location(uniformName), values[0]);
	}
	inline void set_uniform2(const char* uniformName, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform2i(get_uniform_location(uniformName), values[0], values[1]);
	}
	inline void set_uniform3(const char* uniformName, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform3i(get_uniform_location(uniformName), values[0], values[1], values[2]);
	}
	inline void set_uniform4(const char* uniformName, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform4i(get_uniform_location(uniformName), values[0], values[1], values[2], values[3]);
	}
	inline void set_uniform1(const char* uniformName, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1ui(get_uniform_location(uniformName), values[0]);
	}
	inline void set_uniform2(const char* uniformName, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform2ui(get_uniform_location(uniformName), values[0], values[1]);
	}
	inline void set_uniform3(const char* uniformName, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform3ui(get_uniform_location(uniformName), values[0], values[1], values[2]);
	}
	inline void set_uniform4(const char* uniformName, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform4ui(get_uniform_location(uniformName), values[0], values[1], values[2], values[3]);
	}
	inline void set_uniform1(const char* uniformName, const GLsizei count, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1fv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform2(const char* uniformName, const GLsizei count, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform2fv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform3(const char* uniformName, const GLsizei count, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform3fv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform4(const char* uniformName, const GLsizei count, const GLfloat* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform4fv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform1(const char* uniformName, const GLsizei count, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1iv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform2(const char* uniformName, const GLsizei count, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform2iv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform3(const char* uniformName, const GLsizei count, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform3iv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform4(const char* uniformName, const GLsizei count, const GLint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform4iv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform1(const char* uniformName, const GLsizei count, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1uiv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform2(const char* uniformName, const GLsizei count, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform2uiv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform3(const char* uniformName, const GLsizei count, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform3uiv(get_uniform_location(uniformName), count, values);
	}
	inline void set_uniform4(const char* uniformName, const GLsizei count, const GLuint* values) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform4uiv(get_uniform_location(uniformName), count, values);
	}

	static void set_uniform(const GLint location, const GLfloat value) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const vec2<GLfloat>& v) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const vec3<GLfloat>& v) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const vec4<GLfloat>& v) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const mat3<GLfloat>& m) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const mat4<GLfloat>& m) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const GLint i) BOOST_NOEXCEPT_OR_NOTHROW;
	
	static void set_uniform(const GLint location, const std::vector<GLfloat>& values) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const std::vector<GLint>& values) BOOST_NOEXCEPT_OR_NOTHROW;

	template <std::size_t Size> 
	static void set_uniform(const GLint location, const std::array<GLfloat, Size>& values) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1fv(location, Size, values.data());
	}

	template <std::size_t Size>
	static void set_uniform(const GLint location, const std::array<GLint, Size>& values) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glUniform1iv(location, Size, values.data());
	}

public:
	
	shader(const shader&) = delete;
	shader(shader&&);
	shader& operator = (const shader&) = delete;
	shader& operator = (shader&&);
	// virtual ~shader() BOOST_NOEXCEPT_OR_NOTHROW;

	void activate() const BOOST_NOEXCEPT_OR_NOTHROW;
	static void deactivate() BOOST_NOEXCEPT_OR_NOTHROW;

	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
	inline void operator()() const BOOST_NOEXCEPT_OR_NOTHROW { activate(); }
};

} // end of namespace opengl
} // end of namespace gintonic

// namespace std {

// template<>
// void swap(gintonic::opengl::buffer_object& lhs, gintonic::opengl::buffer_object& rhs) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	lhs.swap(rhs);
// }

// template
// <
// 	GLenum TargetLHS,
// 	GLenum TargetRHS,
// 	GLenum ModeLHS,
// 	GLenum ModeRHS,
// 	GLenum UsageLHS,
// 	GLenum UsageRHS,
// 	class T,
// 	class Alloc
// >
// void swap(gintonic::opengl::lazy_vector<TargetLHS, ModeLHS, UsageLHS, T, Alloc>& lhs,
// 	gintonic::opengl::lazy_vector<TargetRHS, ModeRHS, UsageRHS, T, Alloc>& rhs) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	lhs.swap(rhs);
// }

// } // namespace std

#endif