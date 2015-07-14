/*!
\file opengl.hpp
\author Raoul Wols
\date 17/09/2014
\brief Classes and functions dealing with the OpenGL API.
*/

#ifndef opengl_hpp
#define opengl_hpp

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

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	#pragma clang diagnostic ignored "-Wdocumentation"
#endif

#ifdef BOOST_MSVC
	#define NOMINMAX
	#include <windows.h>
#endif

#include "glad.hpp"
#include <SDL.h>

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
template <class T> inline void gtBufferData(const GLenum target,
	const std::vector<T>& v,
	const GLenum usage)
{
	glBufferData(target, v.size() * sizeof(T), v.data(), usage);
}

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
template <class T> inline void gtBufferSubData(const GLenum target,
	const std::size_t element_offset,
	const std::size_t num_elements,
	const std::vector<T>& v)
{
	glBufferSubData(target, sizeof(T) * element_offset, sizeof(T) * num_elements, v.data());
}

namespace gintonic {
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

class base
{
public:
	base(base&&) = delete;
	base(const base&) = delete;
	base& operator = (const base&) = delete;
	base& operator = (base&&) = delete;
	virtual ~base() {}
	operator GLuint() const { return mHandle; }
protected:
	base(const GLuint handle) : mHandle(handle) {}
	GLuint mHandle;
private:

};

/*!
\brief Handle to a generic OpenGL buffer object.

On construction, this class will generate an OpenGL buffer object. On destruction, it will
destroy the OpenGL buffer object that it holds. You can simply cast this class to a GLuint
to obtain the underlying buffer. This means that you can use it in OpenGL function calls. For
instance,
\code
opengl::buffer_object buf;
glBindBuffer(GL_ARRAY_BUFFER, buf);
\endcode

You **must not** call glDeleteBuffers on this object, or your program will crash.

The size
of this class is the size of a GLuint, and all methods are inlined (mostly just one line of code),
so there is no reason not to use this class.

As with all ``handle-like'' classes in this framework, a buffer_object is not copy-constructible
and not copy-assignable. However, a buffer_object is move-constructible and move-assignable.

\sa buffer_object_array
\sa vertex_array_object
\sa vertex_array_object_array
*/
class buffer_object
{
public:

	/*!
	\brief Constructs a buffer_object.

	Equivalent to
	\code
	GLuint buffer_object;
	glGenBuffers(1, &buffer_object);
	\endcode
	*/
	inline buffer_object()
	{
		glGenBuffers(1, &m_handle);
	}

	/*!
	\brief Destructor.

	Equivalent to
	\code
	glDeleteBuffers(1, &buffer_object);
	\endcode
	*/
	inline ~buffer_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteBuffers(1, &m_handle);
	}

	buffer_object(const buffer_object&) = delete;

	/*!
	\brief Move-constructor.
	\param [in,out] other The other buffer_object.

	After this call, the other buffer_object is in a void state, so do not use
	it anymore.
	*/
	inline buffer_object(buffer_object&& other) : m_handle(other.m_handle) { other.m_handle = 0; }
	
	buffer_object& operator = (const buffer_object&) = delete;

	/*!
	\brief Move-assignment operator.
	\param [in,out] other The other buffer_object.

	After this call, the other buffer_object is in a void state, so do not use
	it anymore.
	*/
	inline buffer_object& operator = (buffer_object&& other)
	{
		m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}

	/*!
	\brief A buffer_object is castable to a GLuint.
	*/
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
private:
	GLuint m_handle;
};

/*!
\brief A compile-time size array of buffer_object's.

If you need more than one buffer_object and you know
at compile time how many you need, then using this class
improves performance a little bit. This class is templated
on the number of buffers that you need.

\tparam Size The number of buffer_object's to create.

\sa buffer_object
\sa vertex_array_object
\sa vertex_array_object_array
*/
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

/*!
\brief Encapsulates a handle to an OpenGL vertex array object.

This class is *almost* the same as a buffer_object. The difference
is in the constructor and destructor. Where a buffer_object uses
glGenBuffers and glDeleteBuffers, a vertex_array_object uses
glGenVertexArrays and glDeleteVertexArrays.

\sa buffer_object
\sa buffer_object_array
\sa vertex_array_object_array
*/
class vertex_array_object
{
public:

	/*!
	\brief Constructs a vertex_array_object.

	Equivalent to
	\code
	GLuint vertex_array_object;
	glGenVertexArrays(1, &vertex_array_object);
	\endcode
	*/
	inline vertex_array_object()
	{
		glGenVertexArrays(1, &m_handle);
	}

	/*!
	\brief Destructor.

	Equivalent to
	\code
	glDeleteVertexArrays(1, &vertex_array_object);
	\endcode
	*/
	inline ~vertex_array_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteVertexArrays(1, &m_handle);
	}

	vertex_array_object(const vertex_array_object&) = delete;

	/*!
	\brief Move-constructor.
	\param [in,out] other The other vertex_array_object.

	After this call, the other vertex_array_object is in a void state, so do not use
	it anymore.
	*/
	inline vertex_array_object(vertex_array_object&& other)
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}

	vertex_array_object& operator = (const vertex_array_object&) = delete;

	/*!
	\brief Move-assignment operator.
	\param [in,out] other The other vertex_array_object.

	After this call, the other vertex_array_object is in a void state, so do not use
	it anymore.
	*/
	inline vertex_array_object& operator = (vertex_array_object&& other)
	{
		m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}

	/*!
	\brief A vertex_array_object is castable to a GLuint.
	*/
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
private:
	GLuint m_handle;
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
			std::cerr << error_msg << '\n';
			BOOST_THROW_EXCEPTION(error() << errinfo(error_msg) << pathinfo(p));
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
			std::cerr << error_msg << '\n';
			BOOST_THROW_EXCEPTION( error() << errinfo(error_msg) );
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

class shader : public object<shader, std::tuple<boost::filesystem::path, boost::filesystem::path, boost::filesystem::path>>
{
public:

	//static void init(const boost::filesystem::path& path_to_list_of_shaders);
	//inline static void release() BOOST_NOEXCEPT_OR_NOTHROW { /* does nothing */ };

	struct error : public exception {};
	struct link_error : public error {};
	struct uniform_not_found : public error {};
	struct attribute_not_found : public error {};
	struct not_present : public error {};

	typedef boost::error_info<struct tag_link_errinfo, std::string> link_errinfo;
	typedef boost::error_info<struct tag_uniform_errinfo, const char*> uniform_errinfo;
	typedef boost::error_info<struct tag_attribute_errinfo, const char*> attribute_errinfo;
	typedef boost::error_info<struct tag_shader_errinfo, key_type> shader_errinfo;

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

private:

	GLuint m_handle;
	static std::map<key_type, std::vector<boost::filesystem::path>> s_shader_map;
	friend boost::flyweights::detail::optimized_key_value<key_type, shader, key_extractor>;
	shader(const key_type& key);
	shader(key_type&& key);
	void construct();

public:

	shader() = delete;
	shader(const shader&) = delete;
	shader(shader&&);
	shader& operator = (const shader&) = delete;
	shader& operator = (shader&&);
	virtual ~shader() BOOST_NOEXCEPT_OR_NOTHROW;

	void activate() const BOOST_NOEXCEPT_OR_NOTHROW;
	static void deactivate() BOOST_NOEXCEPT_OR_NOTHROW;

	GLint get_uniform_location(const GLchar* uniform_name_in_source_code) const;
	GLint get_attrib_location(const GLchar* attribute_name_in_source_code) const;
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

	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
	inline void operator()() const BOOST_NOEXCEPT_OR_NOTHROW { activate(); }
};

} // end of namespace opengl
} // end of namespace gintonic

#endif