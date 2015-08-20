//
// opengl.cpp
//

#include "opengl.hpp"
#ifdef _MSC_VER
	#include <intrin.h>
#else
	#include <cpuid.h>
#endif
#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <boost/filesystem.hpp>

#ifndef DEBUG_PRINT
#define DEBUG_PRINT std::cerr << __FILE__ << ':' << __LINE__ << '\n';
#endif

#define SHADERS_PATH "Shaders/"

#define VERTEX_SHADER_EXTENSION "vs"
#define FRAGMENT_SHADER_EXTENSION "fs"
#define GEOMETRY_SHADER_EXTENSION "gs"

#ifdef _MSC_VER
	#define cpuid __cpuid
#else
	void cpuid(int CPUInfo[4],int InfoType)
	{
		// I have no idea what happens here... But it works.
	    __asm__ __volatile__ 
	    (
	        "cpuid":
	        "=a" (CPUInfo[0]),
	        "=b" (CPUInfo[1]),
	        "=c" (CPUInfo[2]),
	        "=d" (CPUInfo[3]) :
	        "a" (InfoType)
	    );
	}
#endif

void glDebugPrintBuffers(const GLint expected_vao, const GLint expected_vbo, const GLint expected_ibo)
{
	GLint actual_vao, actual_vbo, actual_ibo;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &actual_vao);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &actual_vbo);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &actual_ibo);
	if (actual_vao != expected_vao)
	{
		std::cerr << "GL_VERTEX_ARRAY mismatch: actual=" << actual_vao << ", expected=" << expected_vao << '\n';
	}
	if (actual_vbo != expected_vbo)
	{
		std::cerr << "GL_VERTEX_ARRAY mismatch: actual=" << actual_vbo << ", expected=" << expected_vbo << '\n';
	}
	if (actual_ibo != expected_ibo)
	{
		std::cerr << "GL_VERTEX_ARRAY mismatch: actual=" << actual_ibo << ", expected=" << expected_ibo << '\n';
	}
}


namespace gintonic {
namespace opengl {

// std::map<shader::key_type, std::vector<boost::filesystem::path>> shader::s_shader_map = std::map<shader::key_type, std::vector<boost::filesystem::path>>();

void driverVersion(int& major, int& minor)
{
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
}

void driver
(
	std::string& vendor,
	std::string& renderer,
	std::string& version,
	std::string& shading_language_version,
	std::vector<std::string>& extensions
)
{
	GLint n;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	shading_language_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	for (GLint i = 0; i < n; ++i)
		extensions.push_back(std::string(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i))));
}

cpu_features::cpu_features()
{
	clear();
	query();
}

void cpu_features::clear()
{
	std::memset(this, 0, sizeof(*this));
}
void cpu_features::query()
{
	int info[4];
	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	unsigned int nExIds = info[0];

	//  Detect Instruction Set
	if (nIds >= 1)
	{
	    cpuid(info,0x00000001);
	    mmx   = (info[3] & ((int)1 << 23)) != 0;
	    sse   = (info[3] & ((int)1 << 25)) != 0;
	    sse2  = (info[3] & ((int)1 << 26)) != 0;
	    sse3  = (info[2] & ((int)1 <<  0)) != 0;

	    ssse3 = (info[2] & ((int)1 <<  9)) != 0;
	    sse41 = (info[2] & ((int)1 << 19)) != 0;
	    sse42 = (info[2] & ((int)1 << 20)) != 0;

	    avx   = (info[2] & ((int)1 << 28)) != 0;
	    fma3  = (info[2] & ((int)1 << 12)) != 0;
	}
	if (nExIds >= 0x80000001)
	{
	    cpuid(info,0x80000001);
	    x64   = (info[3] & ((int)1 << 29)) != 0;
	    sse4a = (info[2] & ((int)1 <<  6)) != 0;
	    fma4  = (info[2] & ((int)1 << 16)) != 0;
	    xop   = (info[2] & ((int)1 << 11)) != 0;
	}
}
std::ostream& operator << (std::ostream& os, const cpu_features& feat)
{
	if (feat.x64) os << "x64 ";
	if (feat.mmx) os << "mmx ";
	if (feat.sse) os << "sse ";
	if (feat.sse2) os << "sse2 ";
	if (feat.sse3) os << "sse3 ";
	if (feat.ssse3) os << "ssse3 ";
	if (feat.sse41) os << "sse4.1 ";
	if (feat.sse42) os << "sse4.2 ";
	if (feat.sse4a) os << "sse4.a ";
	if (feat.avx) os << "avx ";
	if (feat.xop) os << "xop ";
	if (feat.fma3) os << "fma3 ";
	if (feat.fma4) os << "fma4 ";
	return os;
}

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

shader::shader() : m_handle(0)
{
	/* Empty on purpose */
}

shader::shader(
	boost::filesystem::path path_vertex, 
	boost::filesystem::path path_fragment)
	: m_handle(glCreateProgram())
{
	if (!m_handle) throw std::bad_alloc();

	GLint r;

	basic_shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
	basic_shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);

	glAttachShader(*this, vertexshader);
	glAttachShader(*this, fragmentshader);

	glLinkProgram(*this);
	glGetProgramiv(*this, GL_LINK_STATUS, &r);
	
	glDetachShader(*this, vertexshader);
	glDetachShader(*this, fragmentshader);

	if (r == GL_FALSE)
	{
		std::basic_string<GLchar> infolog;
		glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &r);
		infolog.resize(r);
		glGetProgramInfoLog(*this, r, nullptr, &infolog[0]);
		glDeleteProgram(m_handle);
		m_handle = 0;
		throw exception(std::move(infolog));
	}
}

shader::shader(
	boost::filesystem::path path_vertex, 
	boost::filesystem::path path_geometry, 
	boost::filesystem::path path_fragment)
	: m_handle(glCreateProgram())
{
	if (!m_handle) throw std::bad_alloc();

	GLint r;

	basic_shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
	basic_shader<GL_GEOMETRY_SHADER> geometryshader(path_geometry);
	basic_shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);

	glAttachShader(*this, vertexshader);
	glAttachShader(*this, geometryshader);
	glAttachShader(*this, fragmentshader);

	glLinkProgram(*this);
	glGetProgramiv(*this, GL_LINK_STATUS, &r);

	glDetachShader(*this, vertexshader);
	glDetachShader(*this, geometryshader);
	glDetachShader(*this, fragmentshader);

	if (r == GL_FALSE)
	{
		std::basic_string<GLchar> infolog;
		glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &r);
		infolog.resize(r);
		glGetProgramInfoLog(*this, r, nullptr, &infolog[0]);
		glDeleteProgram(m_handle);
		m_handle = 0;
		throw exception(std::move(infolog));
	}
}

// shader::shader(const key_type& key) : object<shader, key_type>(key), m_handle(0)
// {
// 	construct_from_key();
// }

// shader::shader(key_type&& key) : object<shader, key_type>(std::move(key)), m_handle(0)
// {
// 	construct_from_key();
// }

// void shader::construct_from_key()
// {
// 	const auto& path_vertex = std::get<0>(key());
// 	const auto& path_geometry = std::get<1>(key());
// 	const auto& path_fragment = std::get<2>(key());

// 	if (path_vertex == "" && path_geometry == "" && path_fragment == "") return;

// 	std::cout << path_vertex << ' ' << path_geometry << ' ' << path_fragment << '\n';

// 	m_handle = glCreateProgram();
// 	if (!m_handle) throw std::bad_alloc();

// 	GLint r;

// 	if (path_geometry == "")
// 	{
// 		// vertex shader and fragment shader
// 		basic_shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
// 		basic_shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);
// 		glAttachShader(*this, vertexshader);
// 		glAttachShader(*this, fragmentshader);
// 		glLinkProgram(*this);
// 		glGetProgramiv(*this, GL_LINK_STATUS, &r);
// 		glDetachShader(*this, vertexshader);
// 		glDetachShader(*this, fragmentshader);
// 	}
// 	else
// 	{
// 		// vertex shader, geometry shader and fragment shader
// 		basic_shader<GL_VERTEX_SHADER> vertexshader(path_vertex);
// 		basic_shader<GL_GEOMETRY_SHADER> geometryshader(path_geometry);
// 		basic_shader<GL_FRAGMENT_SHADER> fragmentshader(path_fragment);
// 		glAttachShader(*this, vertexshader);
// 		glAttachShader(*this, geometryshader);
// 		glAttachShader(*this, fragmentshader);
// 		glLinkProgram(*this);
// 		glGetProgramiv(*this, GL_LINK_STATUS, &r);
// 		glDetachShader(*this, vertexshader);
// 		glDetachShader(*this, geometryshader);
// 		glDetachShader(*this, fragmentshader);
// 	}
	
// 	if (r == GL_FALSE)
// 	{
// 		std::string infolog;
// 		glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &r);
// 		infolog.resize(r);
// 		glGetProgramInfoLog(*this, r, nullptr, &infolog[0]);
// 		glDeleteProgram(m_handle);
// 		m_handle = 0;
// 		BOOST_THROW_EXCEPTION(link_error() << shader_errinfo(key()) << link_errinfo(infolog));
// 	}
// }

GLint shader::num_active_attributes() const BOOST_NOEXCEPT_OR_NOTHROW
{
	GLint result;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTES, &result);
	return result;
}

GLint shader::num_active_uniforms() const BOOST_NOEXCEPT_OR_NOTHROW
{
	GLint result;
	glGetProgramiv(*this, GL_ACTIVE_UNIFORMS, &result);
	return result;
}

shader::uniform shader::get_uniform(const GLint index) const
{
	uniform result;
	GLint bufsize;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
	result.name.resize(bufsize);
	glGetActiveUniform(*this, index, bufsize, nullptr, &result.size, &result.type, &result.name[0]);
	result.location = glGetUniformLocation(*this, result.name.c_str());
	return result;
}

shader::attribute shader::get_attribute(const GLint location) const
{
	attribute result;
	GLint bufsize;
	glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
	result.name.resize(bufsize);
	glGetActiveAttrib(*this, location, bufsize, nullptr, &result.size, &result.type, &result.name[0]);
	result.location = glGetAttribLocation(*this, result.name.c_str());
	return result;
}

shader::shader(shader&& other) : m_handle(other.m_handle)
{
	other.m_handle = 0;
}

shader& shader::operator = (shader&& other)
{
	this->~shader();
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

shader::~shader() BOOST_NOEXCEPT_OR_NOTHROW { glDeleteProgram(*this); }

void shader::activate() const BOOST_NOEXCEPT_OR_NOTHROW { glUseProgram(m_handle); }
void shader::deactivate() BOOST_NOEXCEPT_OR_NOTHROW { glUseProgram(0); }

GLint shader::get_uniform_location(const GLchar* name) const
{
	const auto r = glGetUniformLocation(*this, name);
	if (r == -1)
	{
		throw exception("Uniform \"" + std::string(name) + "\" not present.");
	}
	else
	{
		return r;
	}
}

bool shader::get_uniform_location(const GLchar* name, GLint& location) const BOOST_NOEXCEPT_OR_NOTHROW
{
	location = glGetUniformLocation(*this, name);
	return location == -1 ? false : true;
}

GLint shader::get_attrib_location(const GLchar* name) const
{
	const auto r = glGetAttribLocation(*this, name);
	if (r == -1)
	{
		throw std::runtime_error("shader::get_attrib_location: attrib not found.");
	}
	else
	{
		return r;
	}
}

bool shader::get_attrib_location(const GLchar* name, GLint& location) const BOOST_NOEXCEPT_OR_NOTHROW
{
	location = glGetAttribLocation(*this, name);
	return location == -1 ? false : true;
}

void shader::set_uniform(const char* uniformName, const GLfloat value) const
{
	glUniform1f(get_uniform_location(uniformName), value);
}
void shader::set_uniform(const char* uniformName, const vec2f& v) const
{
	glUniform2f(get_uniform_location(uniformName), v.x, v.y);
}
void shader::set_uniform(const char* uniformName, const vec3f& v) const
{
	glUniform3f(get_uniform_location(uniformName), v.x, v.y, v.z);
}
void shader::set_uniform(const char* uniformName, const vec4f& v) const
{
	glUniform4f(get_uniform_location(uniformName), v.x, v.y, v.z, v.w);
}
void shader::set_uniform(const char* uniformName, const mat3f& m) const
{
	float temp[9];
	const float* ptr = m.value_ptr();
	temp[0] = ptr[0];
	temp[1] = ptr[1];
	temp[2] = ptr[2];

	temp[3] = ptr[4];
	temp[4] = ptr[5];
	temp[5] = ptr[6];
	
	temp[6] = ptr[8];
	temp[7] = ptr[9];
	temp[8] = ptr[10];
	
	glUniformMatrix3fv(get_uniform_location(uniformName), 1, GL_FALSE, temp);
}
void shader::set_uniform(const char* uniformName, const mat4f& m) const
{
	glUniformMatrix4fv(get_uniform_location(uniformName), 1, GL_FALSE, m.value_ptr());
}
void shader::set_uniform(const char* uniformName, const GLint i) const
{
	glUniform1i(get_uniform_location(uniformName), i);
}
void shader::set_uniform(const GLint location, const GLfloat value) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform1f(location, value);
}
void shader::set_uniform(const GLint location, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform2f(location, v.x, v.y);
}
void shader::set_uniform(const GLint location, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform3f(location, v.x, v.y, v.z);
}
void shader::set_uniform(const GLint location, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform4f(location, v.x, v.y, v.z, v.w);
}
void shader::set_uniform(const GLint location, const mat3f& m) BOOST_NOEXCEPT_OR_NOTHROW
{
	float temp[9];
	const float* ptr = m.value_ptr();
	temp[0] = ptr[0];
	temp[1] = ptr[1];
	temp[2] = ptr[2];

	temp[3] = ptr[4];
	temp[4] = ptr[5];
	temp[5] = ptr[6];
	
	temp[6] = ptr[8];
	temp[7] = ptr[9];
	temp[8] = ptr[10];

	glUniformMatrix3fv(location, 1, GL_FALSE, temp);
}
void shader::set_uniform(const GLint location, const mat4f& m) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniformMatrix4fv(location, 1, GL_FALSE, m.value_ptr());
}
void shader::set_uniform(const GLint location, const GLint i) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform1i(location, i);
}
void shader::set_uniform(const GLint location, const std::vector<GLfloat>& values) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform1fv(location, values.size(), values.data());
}
void shader::set_uniform(const GLint location, const std::vector<GLint>& values) BOOST_NOEXCEPT_OR_NOTHROW
{
	glUniform1iv(location, values.size(), values.data());
}

} // end of namespace opengl
} // end of namespace gintonic
