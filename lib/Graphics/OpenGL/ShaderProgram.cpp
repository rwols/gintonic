#include <gintonic/Graphics/OpenGL/Shader.hpp>
#include <gintonic/Graphics/OpenGL/ShaderProgram.hpp>
#include <gintonic/Math/mat2f.hpp>
#include <gintonic/Math/mat3f.hpp>
#include <gintonic/Math/mat4f.hpp>
#include <gintonic/Math/vec2f.hpp>
#include <gintonic/Math/vec3f.hpp>
#include <gintonic/Math/vec4f.hpp>

using namespace gintonic;
using namespace gintonic::OpenGL;

ShaderProgram::ShaderProgram(const char* name)
    : Asset(name), mHandle(glCreateProgram())
{
    if (!mHandle) throw std::bad_alloc();
}

ShaderProgram::ShaderProgram(const std::string& name)
    : Asset(name), mHandle(glCreateProgram())
{
    if (!mHandle) throw std::bad_alloc();
}

ShaderProgram::ShaderProgram(std::string&& name)
    : Asset(std::move(name)), mHandle(glCreateProgram())
{
    if (!mHandle) throw std::bad_alloc();
}

std::shared_ptr<ShaderProgram>
ShaderProgram::fromShaderFiles(std::string name, std::string vertexShaderFile,
                               std::string geometryShaderFile,
                               std::string fragmentShaderFile)
{
    auto program = std::make_shared<ShaderProgram>(std::move(name));
    program->setVertexShaderFile(std::move(vertexShaderFile));
    program->setGeometryShaderFile(std::move(geometryShaderFile));
    program->setFragmentShaderFile(std::move(fragmentShaderFile));
    std::basic_string<GLchar> errorMessage;
    if (!program->compileAndLink(errorMessage))
    {
        throw gintonic::exception(std::move(errorMessage));
    }
    return program;
}

void ShaderProgram::setVertexShaderFile(std::string vertexShaderFile)
{
    mVertexShader = std::move(vertexShaderFile);
    mIsDirty = true;
}
void ShaderProgram::setGeometryShaderFile(std::string geometryShaderFile)
{
    mGeometryShader = std::move(geometryShaderFile);
    mIsDirty = true;
}
void ShaderProgram::setFragmentShaderFile(std::string fragmentShaderFile)
{
    mFragmentShader = std::move(fragmentShaderFile);
    mIsDirty = true;
}

const std::string& ShaderProgram::getVertexShaderFile() const noexcept
{
    return mVertexShader;
}
const std::string& ShaderProgram::getGeometryShaderFile() const noexcept
{
    return mGeometryShader;
}
const std::string& ShaderProgram::getFragmentShaderFile() const noexcept
{
    return mFragmentShader;
}

bool ShaderProgram::isDirty() const noexcept { return mIsDirty; }

struct AttachGuard
{
  private:
    GLuint mProgram;
    GLuint mShader;

  public:
    AttachGuard(const GLuint program, const GLuint shader)
        : mProgram(program), mShader(shader)
    {
        if (mShader) glAttachShader(mProgram, mShader);
    }
    ~AttachGuard()
    {
        if (mShader) glDetachShader(mProgram, mShader);
    }
};

template <GLenum ShaderType>
std::unique_ptr<Shader<ShaderType>>
makeOptionalShader(const boost::filesystem::path& basePath,
                   const std::string& shaderPathSuffix)
{
    if (shaderPathSuffix.empty())
        return nullptr;
    else
        return std::make_unique<Shader<ShaderType>>(basePath /
                                                    shaderPathSuffix);
}

bool ShaderProgram::compileAndLink(std::basic_string<GLchar>& errorMessage)
{
    if (!isDirty()) return true;

    const auto assetPath = boost::filesystem::path(getAssetFolder());

    const Shader<GL_VERTEX_SHADER> vertexshader(assetPath / mVertexShader);
    const auto geomshader =
        makeOptionalShader<GL_GEOMETRY_SHADER>(assetPath, mGeometryShader);
    const Shader<GL_FRAGMENT_SHADER> fragmentshader(assetPath /
                                                    mFragmentShader);

    const AttachGuard vertexAttach(*this, vertexshader);
    const AttachGuard geometryAttach(*this, geomshader ? *geomshader : 0);
    const AttachGuard fragmentAttach(*this, fragmentshader);

    if (!attemptLink(errorMessage)) return false;

    initUniforms();
    mIsDirty = false;
    return true;
}

bool ShaderProgram::attemptLink(std::basic_string<GLchar>& errorMessage)
{
    GLint retValue;
    glLinkProgram(*this);
    glGetProgramiv(*this, GL_LINK_STATUS, &retValue);
    if (retValue == GL_FALSE)
    {
        glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &retValue);
        errorMessage.resize(retValue);
        glGetProgramInfoLog(*this, retValue, nullptr, &errorMessage[0]);
        return false;
    }
    return true;
}

void ShaderProgram::initUniforms()
{
    const auto count = numActiveUniforms();
    mUniforms.reserve(count);
    for (GLint i = 0; i < count; ++i)
    {
        mUniforms.emplace_back(getUniform(i));
    }
    // Sorts uniforms by their name
    std::sort(mUniforms.begin(), mUniforms.end());

    if (mUniforms.empty())
    {
        std::cerr << "No uniforms!\n";
    }
    else
    {
        std::cerr << "Uniforms:\n";
        for (const auto& u : mUniforms)
        {
            std::cout << '\t' << u.name << '\n';
        }
    }
}

GLint ShaderProgram::numActiveAttributes() const noexcept
{
    GLint result;
    glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTES, &result);
    return result;
}

GLint ShaderProgram::numActiveUniforms() const noexcept
{
    GLint result;
    glGetProgramiv(*this, GL_ACTIVE_UNIFORMS, &result);
    return result;
}

ShaderProgram::Uniform ShaderProgram::getUniform(const GLint index) const
{
    Uniform result;
    GLint bufsize, length;
    glGetProgramiv(*this, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufsize);
    result.name.resize(bufsize);
    glGetActiveUniform(*this, index, bufsize, &length, &result.size,
                       &result.type, &result.name[0]);
    result.name.resize(length);
    if (result.name.length() > 3 &&
        0 ==
            result.name.compare(result.name.length() - 3,
                                std::basic_string<GLchar>::npos, "[0]"))
    {
        result.name.resize(result.name.length() - 3);
    }
    result.location = glGetUniformLocation(*this, result.name.c_str());
    return result;
}

ShaderProgram::Attribute ShaderProgram::getAttribute(const GLint location) const
{
    Attribute result;
    GLint bufsize;
    glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufsize);
    result.name.resize(bufsize);
    glGetActiveAttrib(*this, location, bufsize, nullptr, &result.size,
                      &result.type, &result.name[0]);
    result.location = glGetAttribLocation(*this, result.name.c_str());
    return result;
}

ShaderProgram::~ShaderProgram() noexcept { glDeleteProgram(*this); }

void ShaderProgram::activate() const noexcept { glUseProgram(mHandle); }
void ShaderProgram::deactivate() noexcept { glUseProgram(0); }

GLint ShaderProgram::getUniformLocation(const GLchar* name) const
{
    const auto r =
        std::find_if(mUniforms.begin(), mUniforms.end(),
                     [=](const auto& item) { return item.name == name; });
    if (r == mUniforms.end())
        throw exception("Uniform \"" + std::string(name) + "\" not present.");
    else
        return r->location;
    // Uniform temp(name);
    // auto r = std::lower_bound(mUniforms.begin(), mUniforms.end(), temp);
    // if (r->name == name) return r->location;
    // else throw exception("Uniform \"" + temp.name + "\" not present.");

    // if (r == mUniforms.end())
    // {
    //  throw exception("Uniform \"" + std::basic_string<GLchar>(name) + "\" not
    // present.");
    // }

    // const auto r = glGetUniformLocation(*this, name);
    // if (r == -1)
    // {
    //  throw exception("Uniform \"" + std::basic_string<GLchar>(name) + "\" not
    // present.");
    // }
    // else
    // {
    //  return r;
    // }
}

bool ShaderProgram::getUniformLocation(const GLchar* name,
                                       GLint& location) const noexcept
{
    const auto r =
        std::find_if(mUniforms.begin(), mUniforms.end(),
                     [=](const auto& item) { return item.name == name; });
    location = r == mUniforms.end() ? -1 : r->location;
    return location == -1 ? false : true;
}

GLuint ShaderProgram::getUniformBlockIndex(const GLchar* name) const
{
    const auto r = glGetUniformBlockIndex(*this, name);
    if (r == GL_INVALID_INDEX)
    {
        throw exception("Uniform block \"" + std::basic_string<GLchar>(name) +
                        "\" not present.");
    }
    else
    {
        return r;
    }
}

bool ShaderProgram::getUniformBlockIndex(const GLchar* name,
                                         GLuint& index) const noexcept
{
    index = glGetUniformBlockIndex(*this, name);
    return index == GL_INVALID_INDEX ? false : true;
}

GLint ShaderProgram::getAttributeLocation(const GLchar* name) const
{
    const auto r = glGetAttribLocation(*this, name);
    if (r == -1)
    {
        throw std::runtime_error(
            "ShaderProgram::getAttributeLocation: attrib not found.");
    }
    else
    {
        return r;
    }
}

bool ShaderProgram::getAttributeLocation(const GLchar* name,
                                         GLint& location) const noexcept
{
    location = glGetAttribLocation(*this, name);
    return location == -1 ? false : true;
}

void ShaderProgram::setUniform(const char* uniformName,
                               const GLfloat value) const
{
    glUniform1f(getUniformLocation(uniformName), value);
}
void ShaderProgram::setUniform(const char* uniformName, const vec2f& v) const
{
    glUniform2f(getUniformLocation(uniformName), v.x, v.y);
}
void ShaderProgram::setUniform(const char* uniformName, const vec3f& v) const
{
    glUniform3f(getUniformLocation(uniformName), v.x, v.y, v.z);
}
void ShaderProgram::setUniform(const char* uniformName, const vec4f& v) const
{
    glUniform4f(getUniformLocation(uniformName), v.x, v.y, v.z, v.w);
}
void ShaderProgram::setUniform(const char* uniformName, const mat3f& m) const
{
    // float temp[9];
    // const float* ptr = m.value_ptr();
    // temp[0] = ptr[0];
    // temp[1] = ptr[1];
    // temp[2] = ptr[2];

    // temp[3] = ptr[4];
    // temp[4] = ptr[5];
    // temp[5] = ptr[6];

    // temp[6] = ptr[8];
    // temp[7] = ptr[9];
    // temp[8] = ptr[10];

    glUniformMatrix3fv(getUniformLocation(uniformName), 1, GL_FALSE,
                       m.value_ptr());
}
void ShaderProgram::setUniform(const char* uniformName, const mat4f& m) const
{
    glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE,
                       m.value_ptr());
}
void ShaderProgram::setUniform(const char* uniformName, const GLint i) const
{
    glUniform1i(getUniformLocation(uniformName), i);
}
void ShaderProgram::setUniform(const GLint location,
                               const GLfloat value) noexcept
{
    glUniform1f(location, value);
}
void ShaderProgram::setUniform(const GLint location, const vec2f& v) noexcept
{
    glUniform2f(location, v.x, v.y);
}
void ShaderProgram::setUniform(const GLint location, const vec3f& v) noexcept
{
    glUniform3f(location, v.x, v.y, v.z);
}
void ShaderProgram::setUniform(const GLint location, const vec4f& v) noexcept
{
    glUniform4f(location, v.x, v.y, v.z, v.w);
}
void ShaderProgram::setUniform(const GLint location, const mat3f& m) noexcept
{
    // float temp[9];
    // const float* ptr = m.value_ptr();
    // temp[0] = ptr[0];
    // temp[1] = ptr[1];
    // temp[2] = ptr[2];

    // temp[3] = ptr[4];
    // temp[4] = ptr[5];
    // temp[5] = ptr[6];

    // temp[6] = ptr[8];
    // temp[7] = ptr[9];
    // temp[8] = ptr[10];

    glUniformMatrix3fv(location, 1, GL_FALSE, m.value_ptr());
}
void ShaderProgram::setUniform(const GLint location, const mat4f& m) noexcept
{
    glUniformMatrix4fv(location, 1, GL_FALSE, m.value_ptr());
}
void ShaderProgram::setUniform(const GLint location, const GLint i) noexcept
{
    glUniform1i(location, i);
}
void ShaderProgram::setUniform(const GLint location,
                               const std::vector<GLfloat>& values) noexcept
{
    glUniform1fv(location, values.size(), values.data());
}
void ShaderProgram::setUniform(const GLint location,
                               const std::vector<GLint>& values) noexcept
{
    glUniform1iv(location, values.size(), values.data());
}
