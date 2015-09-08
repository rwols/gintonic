/**
 * @file texture_object.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_texture_object_hpp
#define gintonic_opengl_texture_object_hpp

#include "utilities.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief Encapsulates an 
 * [OpenGL Texture Object](https://www.opengl.org/wiki/Texture)
 */
class texture_object
{
private:
	GLuint m_handle;
public:

	/// Get the underlying OpenGL handle with a static_cast.
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_handle;
	}

	/// Constructor.
	texture_object();

	/// You cannot copy construct a texture object.
	texture_object(const texture_object& other) = delete;

	/// You cannot copy assign a texture object.
	texture_object& operator=(const texture_object& other) = delete;

	/// You can, however, move construct a texture object.
	inline texture_object(texture_object&& other) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_handle(other.m_handle)
	{
		other.m_handle = 0;
	}
	
	/// You can, however, move assign a texture object.
	texture_object& operator=(texture_object&& other) BOOST_NOEXCEPT_OR_NOTHROW;
	
	/// Destructor destroys the OpenGL handle.
	inline ~texture_object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDeleteTextures(1, &m_handle);
	}
};

} // namespace opengl
} // namespace gintonic

#endif