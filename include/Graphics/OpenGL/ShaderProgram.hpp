/**
 * @file ShaderProgram.hpp
 * @brief Defines the base class for shader classes.
 * @author Raoul Wols
 */

#pragma once

#include "Asset.hpp"
#include "Foundation/filesystem.hpp"
#include "utilities.hpp"
#include <array>

namespace gintonic
{

union vec2f; // Forward declaration.
union vec3f; // Forward declaration.
union vec4f; // Forward declaration.
union mat2f; // Forward declaration.
union mat3f; // Forward declaration.
union mat4f; // Forward declaration.

namespace OpenGL
{

/**
 * @brief Encapsulates an
 * [OpenGL Shader Program](https://www.opengl.org/wiki/Shader_Compilation).
 *
 * @details A shader program is a link between at least a vertex shader and
 * a fragment shader. There can be more shader stages inbetween these two
 * shaders.
 *
 * Most of the functionality of the ShaderProgram is present in the
 * protected section. You are encouraged to derive from ShaderProgram and
 * use all the protected functionality.
 */
class ShaderProgram : public Asset
{
    GINTONIC_ASSET(ShaderProgram, "shaderprograms", ".shader")

  public:
    /**
     * @brief Encapsulates a Uniform variable of a shader.
     */
    struct Uniform
    {
        /// The name of the Uniform variable.
        std::basic_string<GLchar> name;

        /// The location of the Uniform variable in a shader.
        GLuint location;

        /// The size of the Uniform variable (in bytes).
        GLint size;

        /// The type of the Uniform variable.
        GLenum type;

        Uniform() = default;
        Uniform(const GLchar* name) : name(name) {}

        /// Compares the names lexicographically.
        inline bool operator<(const Uniform& other) const noexcept
        {
            return name < other.name;
        }

        /// Compares the names lexicographically.
        inline bool operator>=(const Uniform& other) const noexcept
        {
            return !operator<(other);
        }

        /// Compares the names lexicographically.
        inline bool operator>(const Uniform& other) const noexcept
        {
            return name > other.name;
        }

        /// Compares the names lexicographically.
        inline bool operator<=(const Uniform& other) const noexcept
        {
            return !operator>(other);
        }

        inline bool operator==(const Uniform& other) const noexcept
        {
            return name == other.name;
        }

        inline bool operator!=(const Uniform& other) const noexcept
        {
            return !operator==(other);
        }
    };

    /// An attribute is not really a Uniform, but C++-wise they're the same.
    typedef Uniform Attribute;

    /// You cannot copy ShaderPrograms.
    ShaderProgram(const ShaderProgram&) = delete;

    /// Move constructor.
    ShaderProgram(ShaderProgram&&);

    /// You cannot copy-assign ShaderPrograms.
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    /// Move assignment operator.
    ShaderProgram& operator=(ShaderProgram&&);

    /// Virtual destructor, so inheritance is possible.
    virtual ~ShaderProgram() noexcept;

    /// Get the underlying OpenGL handle with a static_cast.
    inline operator GLuint() const noexcept { return mHandle; }

    /**
     * @brief You can call operator() on a ShaderProgram to activate it.
     * @details This is just syntax-sugar.
     */
    inline void operator()() const noexcept { activate(); }

    /**
     * @brief Activate a ShaderProgram (or: use this ShaderProgram).
     */
    void activate() const noexcept;

    /**
     * @brief Deactivate a ShaderProgram (or: stop using this
     * ShaderProgram).
     *
     * @details Note that this is a static method. Indeed, in the
     * implementation all that is done is calling the OpenGL API to use the
     * 0 (zero) ShaderProgram.
     */
    static void deactivate() noexcept;

  protected:
    ShaderProgram() = default;

    /// Forwarding constructor.
    ShaderProgram(boost::filesystem::path vertexShaderFilename,
                  boost::filesystem::path fragmentShaderFilename);

    /// Forwarding constructor.
    ShaderProgram(boost::filesystem::path vertexShaderFilename,
                  boost::filesystem::path geometryShaderFilename,
                  boost::filesystem::path fragmentShaderFilename);

  private:
    void initUniforms();

    GLuint mHandle;
    std::vector<Uniform> mUniforms;

  public:
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
    GLint getAttributeLocation(const GLchar* name) const;

    /**
     * @brief Get the location of a attribute, non-throwing version.
     * @param name The name of the attribute.
     * @param location A mutable reference. The method will set this value
     * to the location of the attribute, or to -1 on failure.
     * @return True on success, false if no attribute with the given name
     * is present in the shader.
     */
    bool getAttributeLocation(const GLchar* name, GLint& location) const
        noexcept;

    /**
     * @brief Get the number of active attributes.
     * @return The number of active attributes.
     */
    GLint numActiveAttributes() const noexcept;

    /**
     * @brief Get the index'th attribute.
     * @details To get all Uniform variables of a ShaderProgram, you can do
     * something like this:
     *
     * @code
     * auto N = shader_prog.numActiveAttributes();
     * for (GLint i = 0; i < N; ++i)
     * {
     *     auto attr = shader_prog.getAttribute(i);
     *     // do stuff with attr ...
     * }
     * @endcode
     *
     * @param index The index of the attribute.
     * @return The index'th attribute.
     */
    Attribute getAttribute(const GLint index) const;

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
    GLint getUniformLocation(const GLchar* name) const;

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
    bool getUniformLocation(const GLchar* name, GLint& location) const noexcept;

    /**
     * @brief Get the index of a uniform block.
     * @param name The name of the uniform block.
     * @return The index of the uniform block.
     * @exception Will throw an exception if the uniform block is not present
     * with
     * the given name. If you don't want to catch exceptions, consider using
     * the other flavour of this method that doesn't throw but only returns
     * a boolean value indicating success or failure.
     * @note Some OpenGL compilers will optimize away unused uniform
     * blocks.
     */
    GLuint getUniformBlockIndex(const GLchar* name) const;

    /**
     * @brief Get the index of a uniform block, non-throwing version.
     * @param name The name of the uniform block.
     * @param index A mutable reference. The method will set this value
     * to the index of the uniform block, or to `GL_INVALID_INDEX` on failure.
     * @return True on success, false if no uniform with the given name
     * is present in the shader.
     * @note Some OpenGL compilers will optimize away unused uniform
     * blocks.
     */
    bool getUniformBlockIndex(const GLchar* name, GLuint& index) const noexcept;

    /**
     * @brief Get the number of active uniforms variables.
     * @return The number of active uniform varibles.
     */
    GLint numActiveUniforms() const noexcept;

    /**
     * @brief Get the index'th uniform variable.
     * @details To get all uniform variables of a ShaderProgram, you can do
     * something like this:
     *
     * @code
     * auto N = shader_prog.numActiveUniforms();
     * for (GLint i = 0; i < N; ++i)
     * {
     *     auto uni = shader_prog.getUniform(i);
     *     // do stuff with uni ...
     * }
     * @endcode
     *
     * @param index The index of the uniform.
     * @return The index'th uniform variable.
     */
    Uniform getUniform(const GLint index) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const GLfloat value) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const vec2f& value) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const vec3f& value) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const vec4f& value) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const mat3f& value) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const mat4f& value) const;

    /**
     * @brief Set a uniform variable with the given name to the given value.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param value The value to set the uniform to.
     */
    void setUniform(const char* name, const GLint value) const;

    /**
     * @brief Set a uniform variable with the given name to the given first
     * value of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform1(const char* name, const GLfloat* values) const
    {
        glUniform1f(getUniformLocation(name), values[0]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given two
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform2(const char* name, const GLfloat* values) const
    {
        glUniform2f(getUniformLocation(name), values[0], values[1]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given three
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform3(const char* name, const GLfloat* values) const
    {
        glUniform3f(getUniformLocation(name), values[0], values[1], values[2]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given four
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform4(const char* name, const GLfloat* values) const
    {
        glUniform4f(getUniformLocation(name), values[0], values[1], values[2],
                    values[3]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given first
     * value of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform1(const char* name, const GLint* values) const
    {
        glUniform1i(getUniformLocation(name), values[0]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given two
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform2(const char* name, const GLint* values) const
    {
        glUniform2i(getUniformLocation(name), values[0], values[1]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given three
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform3(const char* name, const GLint* values) const
    {
        glUniform3i(getUniformLocation(name), values[0], values[1], values[2]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given four
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform4(const char* name, const GLint* values) const
    {
        glUniform4i(getUniformLocation(name), values[0], values[1], values[2],
                    values[3]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given first
     * value of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform1(const char* name, const GLuint* values) const
    {
        glUniform1ui(getUniformLocation(name), values[0]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given two
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform2(const char* name, const GLuint* values) const
    {
        glUniform2ui(getUniformLocation(name), values[0], values[1]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given three
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform3(const char* name, const GLuint* values) const
    {
        glUniform3ui(getUniformLocation(name), values[0], values[1], values[2]);
    }

    /**
     * @brief Set a uniform variable with the given name to the given four
     * values of the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform4(const char* name, const GLuint* values) const
    {
        glUniform4ui(getUniformLocation(name), values[0], values[1], values[2],
                     values[3]);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform1(const char* name, const GLsizei count,
                            const GLfloat* values) const
    {
        glUniform1fv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform2(const char* name, const GLsizei count,
                            const GLfloat* values) const
    {
        glUniform2fv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform3(const char* name, const GLsizei count,
                            const GLfloat* values) const
    {
        glUniform3fv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform4(const char* name, const GLsizei count,
                            const GLfloat* values) const
    {
        glUniform4fv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform1(const char* name, const GLsizei count,
                            const GLint* values) const
    {
        glUniform1iv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform2(const char* name, const GLsizei count,
                            const GLint* values) const
    {
        glUniform2iv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform3(const char* name, const GLsizei count,
                            const GLint* values) const
    {
        glUniform3iv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform4(const char* name, const GLsizei count,
                            const GLint* values) const
    {
        glUniform4iv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform1(const char* name, const GLsizei count,
                            const GLuint* values) const
    {
        glUniform1uiv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform2(const char* name, const GLsizei count,
                            const GLuint* values) const
    {
        glUniform2uiv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform3(const char* name, const GLsizei count,
                            const GLuint* values) const
    {
        glUniform3uiv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform array with the given name to the given values of
     * the array.
     * @details This calls ShaderProgram::getUniformLocation internally,
     * and that method may throw an exception if the uniform variable cannot
     * be found. So be careful. If you set a lot of uniform variables, it is
     * best to cache the location of the uniform variable (during a
     * constructor phase, say).
     *
     * @param name The name of the uniform variable.
     * @param count The number of values in the array.
     * @param values The value pointer with the values to set the uniform to.
     */
    inline void setUniform4(const char* name, const GLsizei count,
                            const GLuint* values) const
    {
        glUniform4uiv(getUniformLocation(name), count, values);
    }

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const GLfloat value) noexcept;

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const vec2f& value) noexcept;

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const vec3f& value) noexcept;

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const vec4f& value) noexcept;

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const mat3f& value) noexcept;

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const mat4f& value) noexcept;

    /**
     * @brief Set a uniform with the given location to the given value.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param value The value for the uniform.
     */
    static void setUniform(const GLint location, const GLint value) noexcept;

    /**
     * @brief Set a uniform array with the given location to the given values.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param values The values for the uniform.
     */
    static void setUniform(const GLint location,
                           const std::vector<GLfloat>& values) noexcept;

    /**
     * @brief Set a uniform array with the given location to the given values.
     * @details This is a static method and does not throw any exception.
     *
     * @param location The location of the uniform.
     * @param values The values for the uniform.
     */
    static void setUniform(const GLint location,
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
    static void setUniform(const GLint location,
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
    static void setUniform(const GLint location,
                           const std::array<GLint, Size>& values) noexcept
    {
        glUniform1iv(location, Size, values.data());
    }

    ///@}
};

} // namespace OpenGL
} // namespace gintonic
