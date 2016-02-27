/**
 * @file source_code.hpp
 * @brief Defines a class that encapsulates GLSL source code.
 * @author Raoul Wols
 */

#ifndef gintonic_opengl_source_code_hpp
#define gintonic_opengl_source_code_hpp

#include "utilities.hpp"
#include "../filesystem.hpp"

namespace gintonic {
namespace opengl {

/**
 * @brief Encapsulates GLSL source code.
 */
class source_code
{
public:

	/**
	 * @brief Construct a source_code object from a filename.
	 * 
	 * @param filename The filename where the GLSL source code resides.
	 * @return A new source_code object.
	 */
	static source_code from_file(const char* filename);

	/**
	 * @brief Construct a source_code object from a filename.
	 * 
	 * @param filename The filename where the GLSL source code resides.
	 * @return A new source_code object.
	 */
	static source_code from_file(const std::string& filename);

	/**
	 * @brief Construct a source_code object from memory.
	 * 
	 * @param str The string that contains the GLSL source code.
	 * @return A new source_code object.
	 */
	static source_code from_memory(const GLchar* str);

	/**
	 * @brief Construct a source_code object from a filepath.
	 * 
	 * @param p The filepath that contains GLSL source code.
	 */
	source_code(const boost::filesystem::path& p);

	/// Copy constructor.
	source_code(const source_code& other);

	/// Move constructor.
	source_code(source_code&& other);

	/// Copy assignment operator.
	source_code& operator=(const source_code& other);

	/// Move assignment operator.
	source_code& operator=(source_code&& other);
	
	/// Destructor.
	~source_code();

	/**
	 * @brief Get the string representation of the source_code object.
	 * @details You use this method in a call to glShaderSource.
	 * 
	 * @return The string representation of the source.
	 */
	inline const GLchar** string() const noexcept
	{
		return const_cast<const GLchar**>(m_string);
	}

	/**
	 * @brief Get the number of source lines.
	 * @details You use this method in a call to glShaderSource.
	 * 
	 * @return The number of source lines.
	 */
	inline GLsizei count() const noexcept
	{
		return m_count;
	}

private:
	GLchar** m_string;
	GLsizei m_count;

	inline source_code(GLchar** string, GLsizei count)
	: m_string(string), m_count(count) {}
};

} // namespace opengl
} // namespace gintonic

#endif