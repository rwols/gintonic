/**
 * @file opengl/utilities.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_utilities_hpp
#define gintonic_opengl_utilities_hpp

/*****************************************************************************
* We include the OpenGL API at this point. If on Windows, the Windows API    *
* must be included before we include the OpenGL API.                         *
*****************************************************************************/

#ifdef BOOST_MSVC
#include <windows.h>
#endif
#include "glad.hpp"
#include <vector>
#include <boost/config.hpp>

/*****************************************************************************
* DEBUG_GL_ERROR                                                             *
*                                                                            *
* Purpose: Put this after an OpenGL function call to see if there are any    *
*          OpenGL errors reported. The filename and line number will be      *
*          printed aswell.                                                   *
*          If the NDEBUG macro is defined, the DEBUG_GL_ERROR will resolve   *
*          to an empty statement, so no error checking whatsoever is done    *
*          in a non-debug build.                                             *
*****************************************************************************/

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

/*****************************************************************************
* gtBufferData                                                               *
*                                                                            *
* Purpose: To simplify buffering a vector.                                   *
*****************************************************************************/

/**
 * @brief Convenience function for 
 * [glBufferData](https://www.opengl.org/sdk/docs/man3/xhtml/glBufferData.xml).
 * 
 * @tparam ContiguousArrayContainer The type of the STL-compliant
 * contiguous array.
 * @param target Specifies the target buffer object.
 * @param v The contiguous array.
 * @param usage Usage hint.
 */
template <class ContiguousArrayContainer>
inline void gtBufferData(const GLenum target, 
	const ContiguousArrayContainer& v, const GLenum usage)
{
	glBufferData(target, v.size() 
		* sizeof(typename ContiguousArrayContainer::value_type), 
		reinterpret_cast<const GLvoid*>(v.data()), usage);
}

/*****************************************************************************
* gtBufferSubData                                                            *
*                                                                            *
* Purpose: To simplify (sub)buffering a vector.                              *
*****************************************************************************/

/**
 * @brief Convenience function for 
 * [glBufferSubData](https://www.opengl.org/sdk/docs/man3/xhtml/glBufferSubData.xml).
 * 
 * @tparam T The type of the elements in the vector.
 * @param target Specifies the target buffer object.
 * @param element_offset Specifies the offset into the buffer object's data 
 * store where data replacement will begin, measured in the size of T.
 * @param num_elements Specifies the size in the size of T of the data store
 * region being replaced.
 */
template <class T, class Alloc> inline void gtBufferSubData(
	const GLenum target,
	const std::size_t element_offset,
	const std::size_t num_elements,
	const std::vector<T, Alloc>& v)
{
	glBufferSubData(target, sizeof(T) * element_offset, 
		sizeof(T) * num_elements, v.data());
}

namespace gintonic {
namespace opengl {

} // namespace opengl
} // namespace gintonic

#endif