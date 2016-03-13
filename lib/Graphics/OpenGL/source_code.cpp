#include "source_code.hpp"
#include "../../Foundation/exception.hpp"
#include <fstream>

namespace gintonic {
namespace opengl {

source_code source_code::from_file(const char* filename)
{
	GLchar** string;
	GLsizei count(0);
	// GLint* length;
	std::basic_string<GLchar> line;
	std::ifstream input(filename);
	if (!input) throw std::runtime_error("couldn't open file.");
	while (std::getline(input, line)) ++count;
	if (!count) throw std::runtime_error("file has no content.");
	input.clear();
	input.seekg(0, std::ios::beg);
	string = new GLchar*[count];
	// length = new GLint[count];
	for (decltype(count) i = 0; i < count; ++i)
	{
		std::getline(input, line);
		string[i] = new GLchar[line.size() + 2]; // Note the +2 here ...
		std::memcpy(string[i], line.data(), sizeof(GLchar) * line.size());
		string[i][line.size()] = '\n'; // ... for the line-end ...
		string[i][line.size()+1] = '\0'; // ... and for a terminator.
		// length[i] = static_cast<GLint>(line.size() + 1);
	}
	return source_code(string, count);
}

source_code source_code::from_file(const std::string& filename)
{
	return source_code::from_file(filename.c_str());
}

source_code::source_code(const boost::filesystem::path& p)
	: m_string(nullptr)
	, m_count(0)
{
	if (boost::filesystem::is_regular_file(p) == false)
	{
		exception e(p.string());
		e.append(" is not a regular file.");
		throw e;
	}
	std::basic_string<GLchar> line;
	std::basic_ifstream<GLchar> input(p.string());
	if (!input)
	{
		exception e("Could not open file ");
		e.append(p.string());
		throw e;
	}
	while (std::getline(input, line)) ++m_count;
	if (!m_count)
	{
		exception e(p.string());
		e.append(" has no content.");
		throw e;
	}
	input.clear();
	input.seekg(0, std::ios::beg);
	m_string = new GLchar*[m_count];
	for (decltype(m_count) i = 0; i < m_count; ++i)
	{
		std::getline(input, line);
		m_string[i] = new GLchar[line.size() + 2]; // Note the +2 here ...
		std::memcpy(m_string[i], line.data(), sizeof(GLchar) * line.size());
		m_string[i][line.size()] = '\n'; // ... for the line-end ...
		m_string[i][line.size() + 1] = '\0'; // ... and for a terminator.
	}
}

source_code source_code::from_memory(const GLchar* str)
{
	if (str == nullptr) throw std::invalid_argument("pointer is null.");
	else if (*str == '\0') throw std::invalid_argument("pointer points to null.");
	GLchar** string;
	GLsizei count(1);
	// GLint* length;
	string = new GLchar*[count];
	// length = new GLint[count];
	const std::size_t len = static_cast<GLint>(std::strlen(str));
	string[0] = new char[len + 2];
	std::memcpy(string[0], str, sizeof(GLchar) * len);
	string[0][len] = '\n';
	string[0][len+1] = '\0';
	return source_code(string, count);
}

source_code::source_code(const source_code& other) : m_string(nullptr), m_count(other.m_count)
{
	m_string = new GLchar*[m_count];
	// m_length = new GLint[m_count];
	std::size_t len;
	for (decltype(m_count) i = 0; i < m_count; ++i)
	{
		len = std::strlen(other.m_string[i]);
		// m_length[i] = other.m_length[i];
		m_string[i] = new GLchar[len];
		std::memcpy(m_string[i], other.m_string[i], sizeof(GLchar) * len);
	}
}

source_code::source_code(source_code&& other)
: m_string(other.m_string)
, m_count(other.m_count) 
// , m_length(other.m_length)
{
	other.m_string = nullptr;
	other.m_count = 0;
	// m_length = nullptr;
}

source_code& source_code::operator=(const source_code& other)
{
	if (this == &other) return *this;
	this->~source_code();
	m_count = other.m_count;
	m_string = new GLchar*[m_count];
	// m_length = new GLint[m_count];
	std::size_t len;
	for (decltype(m_count) i = 0; i < m_count; ++i)
	{
		len = std::strlen(other.m_string[i]);
		// m_length[i] = other.m_length[i];
		m_string[i] = new GLchar[len];
		std::memcpy(m_string[i], other.m_string[i], sizeof(GLchar) * len);
	}
	return *this;
}

source_code& source_code::operator=(source_code&& other)
{
	if (this == &other) return *this;
	m_string = other.m_string;
	m_count = other.m_count;
	// m_length = other.m_length;
	other.m_string = nullptr;
	other.m_count = 0;
	// other.m_length = nullptr;
	return *this;
}

source_code::~source_code()
{
	// delete [] m_length;
	for (GLsizei i = 0; i < m_count; ++i) delete [] m_string[i];
	delete [] m_string;
}

} // namespace opengl
} // namespace gintonic