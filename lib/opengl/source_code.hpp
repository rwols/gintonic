#ifndef gintonic_opengl_source_code_hpp
#define gintonic_opengl_source_code_hpp

#include "utilities.hpp"
#include "../filesystem.hpp"

namespace gintonic {
namespace opengl {

class source_code
{
public:

	static source_code from_file(const char* filename);

	static source_code from_file(const std::string& filename);

	static source_code from_memory(const GLchar* str);

	source_code(const boost::filesystem::path& p);

	source_code(const source_code& other);

	source_code(source_code&& other);

	source_code& operator=(const source_code& other);

	source_code& operator=(source_code&& other);
	
	~source_code();

	inline const GLchar** string() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_cast<const GLchar**>(m_string);
	}

	inline GLsizei count() const BOOST_NOEXCEPT_OR_NOTHROW
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