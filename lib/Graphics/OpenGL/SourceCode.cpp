#include <boost/filesystem.hpp>
#include <fstream>
#include <gintonic/Foundation/exception.hpp>
#include <gintonic/Graphics/OpenGL/SourceCode.hpp>

using namespace gintonic;
using namespace gintonic::OpenGL;

SourceCode SourceCode::fromFile(const char* filename)
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
        string[i][line.size()] = '\n';     // ... for the line-end ...
        string[i][line.size() + 1] = '\0'; // ... and for a terminator.
        // length[i] = static_cast<GLint>(line.size() + 1);
    }
    return SourceCode(string, count);
}

SourceCode SourceCode::fromFile(const std::string& filename)
{
    return SourceCode::fromFile(filename.c_str());
}

SourceCode::SourceCode(const boost::filesystem::path& p)
    : mString(nullptr), mCount(0)
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
    while (std::getline(input, line)) ++mCount;
    if (!mCount)
    {
        exception e(p.string());
        e.append(" has no content.");
        throw e;
    }
    input.clear();
    input.seekg(0, std::ios::beg);
    mString = new GLchar*[mCount];
    for (decltype(mCount) i = 0; i < mCount; ++i)
    {
        std::getline(input, line);
        mString[i] = new GLchar[line.size() + 2]; // Note the +2 here ...
        std::memcpy(mString[i], line.data(), sizeof(GLchar) * line.size());
        mString[i][line.size()] = '\n';     // ... for the line-end ...
        mString[i][line.size() + 1] = '\0'; // ... and for a terminator.
    }
}

SourceCode SourceCode::fromMemory(const GLchar* str)
{
    if (str == nullptr)
        throw std::invalid_argument("pointer is null.");
    else if (*str == '\0')
        throw std::invalid_argument("pointer points to null.");
    GLchar** string;
    GLsizei count(1);
    // GLint* length;
    string = new GLchar*[count];
    // length = new GLint[count];
    const std::size_t len = static_cast<GLint>(std::strlen(str));
    string[0] = new char[len + 2];
    std::memcpy(string[0], str, sizeof(GLchar) * len);
    string[0][len] = '\n';
    string[0][len + 1] = '\0';
    return SourceCode(string, count);
}

SourceCode::SourceCode(const SourceCode& other)
    : mString(nullptr), mCount(other.mCount)
{
    mString = new GLchar*[mCount];
    // m_length = new GLint[mCount];
    std::size_t len;
    for (decltype(mCount) i = 0; i < mCount; ++i)
    {
        len = std::strlen(other.mString[i]);
        // m_length[i] = other.m_length[i];
        mString[i] = new GLchar[len];
        std::memcpy(mString[i], other.mString[i], sizeof(GLchar) * len);
    }
}

SourceCode::SourceCode(SourceCode&& other)
    : mString(other.mString), mCount(other.mCount)
// , m_length(other.m_length)
{
    other.mString = nullptr;
    other.mCount = 0;
    // m_length = nullptr;
}

SourceCode& SourceCode::operator=(const SourceCode& other)
{
    if (this == &other) return *this;
    this->~SourceCode();
    mCount = other.mCount;
    mString = new GLchar*[mCount];
    // m_length = new GLint[mCount];
    std::size_t len;
    for (decltype(mCount) i = 0; i < mCount; ++i)
    {
        len = std::strlen(other.mString[i]);
        // m_length[i] = other.m_length[i];
        mString[i] = new GLchar[len];
        std::memcpy(mString[i], other.mString[i], sizeof(GLchar) * len);
    }
    return *this;
}

SourceCode& SourceCode::operator=(SourceCode&& other)
{
    if (this == &other) return *this;
    mString = other.mString;
    mCount = other.mCount;
    // m_length = other.m_length;
    other.mString = nullptr;
    other.mCount = 0;
    // other.m_length = nullptr;
    return *this;
}

SourceCode::~SourceCode()
{
    // delete [] m_length;
    for (GLsizei i = 0; i < mCount; ++i) delete[] mString[i];
    delete[] mString;
}
