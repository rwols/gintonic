/**
 * @file shader.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_shader_hpp
#define gintonic_opengl_shader_hpp

#include "utilities.hpp"
#include "source_code.hpp"
#include "../exception.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief Encapsulates an
 * [OpenGL Shader Object](https://www.opengl.org/wiki/Shader_Compilation).
 * @tparam Type The shader type. Can be one of:
 * * GL_VERTEX_SHADER
 * * GL_TESS_CONTROL_SHADER
 * * GL_TESS_EVALUATION_SHADER
 * * GL_GEOMETRY_SHADER
 * * GL_FRAGMENT_SHADER
 * * GL_COMPUTE_SHADER
 */
template <GLint Type> class shader
{
public:
	
	/**
	 * @brief Construct a shader from the given GLSL file.
	 * @param p The filepath to a GLSL file.
	 */
	shader(const boost::filesystem::path& p);

	/**
	 * @brief Construct a shader from the given source_code object.
	 * @param source The source_code object.
	 */
	shader(const source_code& source);

	/// Destructor.
	inline ~shader() BOOST_NOEXCEPT_OR_NOTHROW { glDeleteShader(m_handle); }

	/// Get the underlying OpenGL handle via a static_cast.
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}

private:
	GLuint m_handle;
};

template <GLint Type> 
shader<Type>::shader(const boost::filesystem::path& p) 
: m_handle(glCreateShader(Type))
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

template <GLint Type> 
shader<Type>::shader(const source_code& source) 
: m_handle(glCreateShader(Type))
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

typedef shader<GL_VERTEX_SHADER>   vertex_shader;
typedef shader<GL_FRAGMENT_SHADER> fragment_shader;
typedef shader<GL_GEOMETRY_SHADER> geometry_shader;

} // namespace opengl
} // namespace gintonic

#endif