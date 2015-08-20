#ifndef gintonic_opengl_shader_program_hpp
#define gintonic_opengl_shader_program_hpp

#include <memory> // for std::enable_shared_from_this
#include "utilities.hpp"
#include "../filesystem.hpp"

namespace gintonic {

union vec2f; // Forward declaration.
union vec3f; // Forward declaration.
union vec4f; // Forward declaration.
union mat2f; // Forward declaration.
union mat3f; // Forward declaration.
union mat4f; // Forward declaration.

namespace opengl {

class shader_program : public std::enable_shared_from_this<shader_program>
{
public:

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

	shader_program(const shader_program&) = delete;
	
	shader_program(shader_program&&);

	shader_program& operator = (const shader_program&) = delete;

	shader_program& operator = (shader_program&&);

	virtual ~shader_program() BOOST_NOEXCEPT_OR_NOTHROW;

	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_handle; }
	
	inline void operator()() const BOOST_NOEXCEPT_OR_NOTHROW { activate(); }

	void activate() const BOOST_NOEXCEPT_OR_NOTHROW;
	
	static void deactivate() BOOST_NOEXCEPT_OR_NOTHROW;

protected:

	shader_program(
		boost::filesystem::path vertex_shader_program_file,
		boost::filesystem::path fragment_shader_program_file);

	shader_program(
		boost::filesystem::path vertex_shader_program_file, 
		boost::filesystem::path geometry_shader_program_file, 
		boost::filesystem::path fragment_shader_program_file);

private:

	GLuint m_handle;

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
	static void set_uniform(const GLint location, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const mat3f& m) BOOST_NOEXCEPT_OR_NOTHROW;
	static void set_uniform(const GLint location, const mat4f& m) BOOST_NOEXCEPT_OR_NOTHROW;
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
};

} // namespace opengl
} // namespace gintonic

#endif