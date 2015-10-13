/**
 * @file shader_program.hpp
 * @brief Defines the base class for shader classes.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_shader_program_hpp
#define gintonic_opengl_shader_program_hpp

#include <memory> // for std::enable_shared_from_this
#include <array>
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

/**
 * @brief Encapsulates an
 * [OpenGL Shader Program](https://www.opengl.org/wiki/Shader_Compilation).
 * 
 * @details A shader program is a link between at least a vertex shader and
 * a fragment shader. There can be more shader stages inbetween these two
 * shaders.
 * 
 * Most of the functionality of the shader_program is present in the
 * protected section. You are encouraged to derive from shader_program and
 * use all the protected functionality.
 */
class shader_program : public std::enable_shared_from_this<shader_program>
{
public:

	/**
	 * @brief Encapsulates a uniform variable of a shader.
	 */
	struct uniform
	{
		/// The name of the uniform variable.
		std::basic_string<GLchar> name;

		/// The location of the uniform variable in a shader.
		GLuint location;

		/// The size of the uniform variable (in bytes).
		GLint size;

		/// The type of the uniform variable.
		GLenum type;

		/// Compares the names lexicographically.
		inline bool operator <  (const uniform& other) const 
			noexcept
		{
			return name < other.name;
		}
		
		/// Compares the names lexicographically.
		inline bool operator >= (const uniform& other) const 
			noexcept
		{
			return !operator<(other); 
		}

		/// Compares the names lexicographically.
		inline bool operator >  (const uniform& other) const 
			noexcept
		{
			return name > other.name;
		}

		/// Compares the names lexicographically.
		inline bool operator <= (const uniform& other) const 
			noexcept
		{
			return !operator>(other);
		}
	};

	/// An attribute is not really a uniform, but C++-wise they're the same.
	typedef uniform attribute;

	/// You cannot copy shader_programs.
	shader_program(const shader_program&) = delete;
	
	/// Move constructor.
	shader_program(shader_program&&);

	/// You cannot copy-assign shader_programs.
	shader_program& operator = (const shader_program&) = delete;

	/// Move assignment operator.
	shader_program& operator = (shader_program&&);

	/// Virtual destructor, so inheritance is possible.
	virtual ~shader_program() noexcept;

	/// Get the underlying OpenGL handle with a static_cast.
	inline operator GLuint() const noexcept
	{
		return m_handle;
	}
	
	/**
	 * @brief You can call operator() on a shader_program to activate it.
	 * @details This is just syntax-sugar.
	 */
	inline void operator()() const noexcept { activate(); }

	/**
	 * @brief Activate a shader_program (or: use this shader_program).
	 */
	void activate() const noexcept;
	
	/**
	 * @brief Deactivate a shader_program (or: stop using this 
	 * shader_program).
	 * 
	 * @details Note that this is a static method. Indeed, in the
	 * implementation all that is done is calling the OpenGL API to use the
	 * 0 (zero) shader_program.
	 */
	static void deactivate() noexcept;

protected:

	/// Forwarding constructor.
	shader_program(
		boost::filesystem::path vertex_shader_program_file,
		boost::filesystem::path fragment_shader_program_file);

	/// Forwarding constructor.
	shader_program(
		boost::filesystem::path vertex_shader_program_file, 
		boost::filesystem::path geometry_shader_program_file, 
		boost::filesystem::path fragment_shader_program_file);

private:

	GLuint m_handle;

protected:

	/**
	 * @name Attribute Management
	 * 
	 * Manage attributes with these methods.
	 */

	///@{

	/**
	 * @brief Get the location of a attribute.
	 * @param name The name of the attribute.
	 * @return The location of the attribute.
	 * @exception Will throw an exception if the attribute is not present with
	 * the given name. If you don't want to catch exceptions, consider using
	 * the other flavour of this method that doesn't throw but only returns
	 * a boolean value indicating success or failure.
	 */
	GLint get_attrib_location(const GLchar* name) const;

	/**
	 * @brief Get the location of a attribute, non-throwing version.
	 * @param name The name of the attribute.
	 * @param location A mutable reference. The method will set this value
	 * to the location of the attribute, or to -1 on failure.
	 * @return True on success, false if no attribute with the given name
	 * is present in the shader.
	 */
	bool get_attrib_location(const GLchar* name, GLint& location) const 
		noexcept;

	/**
	 * @brief Get the number of active attributes.
	 * @return The number of active attributes.
	 */
	GLint num_active_attributes() const noexcept;

	/**
	 * @brief Get the index'th attribute.
	 * @details To get all uniform variables of a shader_program, you can do
	 * something like this:
	 * 
	 * @code
	 * auto N = shader_prog.num_active_attributes();
	 * for (GLint i = 0; i < N; ++i)
	 * {
	 *     auto attr = shader_prog.get_attribute(i);
	 *     // do stuff with attr ...
	 * }
	 * @endcode
	 * 
	 * @param index The index of the attribute.
	 * @return The index'th attribute.
	 */
	attribute get_attribute(const GLint index) const;

	///@}

	/**
	 * @name Uniform Management
	 * 
	 * Manage uniforms with these methods.
	 */

	///@{

	/**
	 * @brief Get the location of a uniform variable.
	 * @param name The name of the uniform variable.
	 * @return The location of the uniform variable.
	 * @exception Will throw an exception if the uniform is not present with
	 * the given name. If you don't want to catch exceptions, consider using
	 * the other flavour of this method that doesn't throw but only returns
	 * a boolean value indicating success or failure.
	 * @note Some OpenGL compilers will optimize away unused uniform
	 * variables.
	 */
	GLint get_uniform_location(const GLchar* name) const;

	/**
	 * @brief Get the location of a uniform variable, non-throwing version.
	 * @param name The name of the uniform variable.
	 * @param location A mutable reference. The method will set this value
	 * to the location of the uniform variable, or to -1 on failure.
	 * @return True on success, false if no uniform with the given name
	 * is present in the shader.
	 * @note Some OpenGL compilers will optimize away unused uniform
	 * variables.
	 */
	bool get_uniform_location(const GLchar* name, GLint& location) const 
		noexcept;

	/**
	 * @brief Get the number of active uniforms variables.
	 * @return The number of active uniform varibles.
	 */
	GLint num_active_uniforms() const noexcept;

	/**
	 * @brief Get the index'th uniform variable.
	 * @details To get all uniform variables of a shader_program, you can do
	 * something like this:
	 * 
	 * @code
	 * auto N = shader_prog.num_active_uniforms();
	 * for (GLint i = 0; i < N; ++i)
	 * {
	 *     auto uni = shader_prog.get_uniform(i);
	 *     // do stuff with uni ...
	 * }
	 * @endcode
	 * 
	 * @param index The index of the uniform.
	 * @return The index'th uniform variable.
	 */
	uniform get_uniform(const GLint index) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const GLfloat value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const vec2f& value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const vec3f& value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const vec4f& value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const mat3f& value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const mat4f& value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given value.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param value The value to set the uniform to.
	 */
	void set_uniform(const char* name, const GLint value) const;

	/**
	 * @brief Set a uniform variable with the given name to the given first
	 * value of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform1(const char* name, const GLfloat* values) const
	{
		glUniform1f(get_uniform_location(name), values[0]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given two
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform2(const char* name, const GLfloat* values) const
	{
		glUniform2f(get_uniform_location(name), values[0], values[1]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given three
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform3(const char* name, const GLfloat* values) const
	{
		glUniform3f(get_uniform_location(name), values[0], values[1], 
			values[2]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given four
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform4(const char* name, const GLfloat* values) const
	{
		glUniform4f(get_uniform_location(name), values[0], values[1], 
			values[2], values[3]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given first
	 * value of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform1(const char* name, const GLint* values) const
	{
		glUniform1i(get_uniform_location(name), values[0]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given two
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform2(const char* name, const GLint* values) const
	{
		glUniform2i(get_uniform_location(name), values[0], values[1]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given three
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform3(const char* name, const GLint* values) const
	{
		glUniform3i(get_uniform_location(name), values[0], values[1], 
			values[2]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given four
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform4(const char* name, const GLint* values) const
	{
		glUniform4i(get_uniform_location(name), values[0], values[1], 
			values[2], values[3]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given first
	 * value of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform1(const char* name, const GLuint* values) const
	{
		glUniform1ui(get_uniform_location(name), values[0]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given two
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform2(const char* name, const GLuint* values) const
	{
		glUniform2ui(get_uniform_location(name), values[0], values[1]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given three
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform3(const char* name, const GLuint* values) const
	{
		glUniform3ui(get_uniform_location(name), values[0], values[1], 
			values[2]);
	}

	/**
	 * @brief Set a uniform variable with the given name to the given four
	 * values of the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform4(const char* name, const GLuint* values) const
	{
		glUniform4ui(get_uniform_location(name), values[0], values[1], 
			values[2], values[3]);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform1(const char* name, const GLsizei count, 
		const GLfloat* values) const
	{
		glUniform1fv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform2(const char* name, const GLsizei count, 
		const GLfloat* values) const
	{
		glUniform2fv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform3(const char* name, const GLsizei count, 
		const GLfloat* values) const
	{
		glUniform3fv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform4(const char* name, const GLsizei count, 
		const GLfloat* values) const
	{
		glUniform4fv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform1(const char* name, const GLsizei count, 
		const GLint* values) const
	{
		glUniform1iv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform2(const char* name, const GLsizei count, 
		const GLint* values) const
	{
		glUniform2iv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform3(const char* name, const GLsizei count, 
		const GLint* values) const
	{
		glUniform3iv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform4(const char* name, const GLsizei count, 
		const GLint* values) const
	{
		glUniform4iv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform1(const char* name, const GLsizei count, 
		const GLuint* values) const
	{
		glUniform1uiv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform2(const char* name, const GLsizei count, 
		const GLuint* values) const
	{
		glUniform2uiv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform3(const char* name, const GLsizei count, 
		const GLuint* values) const
	{
		glUniform3uiv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform array with the given name to the given values of 
	 * the array.
	 * @details This calls shader_program::get_uniform_location internally,
	 * and that method may throw an exception if the uniform variable cannot
	 * be found. So be careful. If you set a lot of uniform variables, it is
	 * best to cache the location of the uniform variable (during a
	 * constructor phase, say).
	 * 
	 * @param name The name of the uniform variable.
	 * @param count The number of values in the array.
	 * @param values The value pointer with the values to set the uniform to.
	 */
	inline void set_uniform4(const char* name, const GLsizei count, 
		const GLuint* values) const
	{
		glUniform4uiv(get_uniform_location(name), count, values);
	}

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const GLfloat value) 
		noexcept;

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const vec2f& value) 
		noexcept;

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const vec3f& value) 
		noexcept;

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const vec4f& value) 
		noexcept;

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const mat3f& value) 
		noexcept;

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const mat4f& value) 
		noexcept;

	/**
	 * @brief Set a uniform with the given location to the given value.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param value The value for the uniform.
	 */
	static void set_uniform(const GLint location, const GLint value) 
		noexcept;
	
	/**
	 * @brief Set a uniform array with the given location to the given values.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param values The values for the uniform.
	 */
	static void set_uniform(const GLint location, 
		const std::vector<GLfloat>& values) noexcept;

	/**
	 * @brief Set a uniform array with the given location to the given values.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @param location The location of the uniform.
	 * @param values The values for the uniform.
	 */
	static void set_uniform(const GLint location, 
		const std::vector<GLint>& values) noexcept;

	/**
	 * @brief Set a uniform array with the given location to the given values.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @tparam Size The size of the array.
	 * @param location The location of the uniform.
	 * @param values The values for the uniform.
	 */
	template <std::size_t Size> 
	static void set_uniform(const GLint location, 
		const std::array<GLfloat, Size>& values) noexcept
	{
		glUniform1fv(location, Size, values.data());
	}

	/**
	 * @brief Set a uniform array with the given location to the given values.
	 * @details This is a static method and does not throw any exception.
	 * 
	 * @tparam Size The size of the array.
	 * @param location The location of the uniform.
	 * @param values The values for the uniform.
	 */
	template <std::size_t Size>
	static void set_uniform(const GLint location, 
		const std::array<GLint, Size>& values) noexcept
	{
		glUniform1iv(location, Size, values.data());
	}

	///@}
};

} // namespace opengl
} // namespace gintonic

#endif