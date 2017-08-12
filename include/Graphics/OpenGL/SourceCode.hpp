/**
 * @file SourceCode.hpp
 * @brief Defines a class that encapsulates GLSL source code.
 * @author Raoul Wols
 */

#pragma once

#include "../../Foundation/filesystem.hpp"
#include "utilities.hpp"

namespace gintonic {
namespace OpenGL {

/**
 * @brief Encapsulates GLSL source code.
 */
class GINTONIC_EXPORT SourceCode
{
  public:
    /**
     * @brief Construct a SourceCode object from a filename.
     *
     * @param filename The filename where the GLSL source code resides.
     * @return A new SourceCode object.
     */
    static SourceCode fromFile(const char* filename);

    /**
     * @brief Construct a SourceCode object from a filename.
     *
     * @param filename The filename where the GLSL source code resides.
     * @return A new SourceCode object.
     */
    static SourceCode fromFile(const std::string& filename);

    /**
     * @brief Construct a SourceCode object from memory.
     *
     * @param str The string that contains the GLSL source code.
     * @return A new SourceCode object.
     */
    static SourceCode fromMemory(const GLchar* str);

    /**
     * @brief Construct a SourceCode object from a filepath.
     *
     * @param p The filepath that contains GLSL source code.
     */
    SourceCode(const boost::filesystem::path& p);

    /// Copy constructor.
    SourceCode(const SourceCode& other);

    /// Move constructor.
    SourceCode(SourceCode&& other);

    /// Copy assignment operator.
    SourceCode& operator=(const SourceCode& other);

    /// Move assignment operator.
    SourceCode& operator=(SourceCode&& other);

    /// Destructor.
    ~SourceCode();

    /**
     * @brief Get the string representation of the SourceCode object.
     * @details You use this method in a call to glShaderSource.
     *
     * @return The string representation of the source.
     */
    inline const GLchar** string() const noexcept
    {
        return const_cast<const GLchar**>(mString);
    }

    /**
     * @brief Get the number of source lines.
     * @details You use this method in a call to glShaderSource.
     *
     * @return The number of source lines.
     */
    inline GLsizei count() const noexcept { return mCount; }

  private:
    GLchar** mString;
    GLsizei  mCount;

    inline SourceCode(GLchar** string, GLsizei count)
        : mString(string), mCount(count)
    {
    }
};

} // namespace OpenGL
} // namespace gintonic
