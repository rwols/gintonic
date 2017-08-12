/**
 * @file Vector.hpp
 * @brief Defines the OpenGL Vector class wrapper and the array Vector class
 * wrapper.
 * @author Raoul Wols
 */

#pragma once

#include "BufferObject.hpp"
#include "BufferObjectArray.hpp"

namespace gintonic {
namespace OpenGL {

/**
 * @brief A Vector class for buffering data to GPU memory.
 *
 * @tparam Target Specifies the target of the buffer. For instance,
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * @tparam T Specifies the type of element to store.
 */
template <GLenum Target, class T> class Vector
{
  public:
    /**
     * @brief Constructor.
     *
     * @param usagehint Specifies the usage hint.
     */
    Vector(const GLenum usagehint)
    {
        bind();
        glBufferData(Target, sizeof(T) * mReserved, nullptr, usagehint);
    }

    /**
     * @brief Constructor.
     *
     * @param reserve Reserve this many elements.
     * @param usagehint Specifies the usage hint.
     */
    Vector(const GLsizei reserve, const GLenum usagehint) : mReserved(reserve)
    {
        bind();
        glBufferData(Target, sizeof(T) * mReserved, nullptr, usagehint);
    }

    /**
     * @brief Constructor.
     *
     * @tparam Alloc The allocator type of the std::vector.
     * @param v The std::vector to initialize this Vector with.
     * @param usagehint Specifies the usage hint.
     */
    template <class Alloc>
    Vector(const std::vector<T, Alloc>& v, const GLenum usagehint)
        : mCount(static_cast<GLsizei>(v.size())),
          mReserved(static_cast<GLsizei>(v.size()))
    {
        gtBufferData(Target, mBuffer, v, usagehint);
    }

    /**
     * @brief Get the size of the Vector.
     * @return The size of the Vector.
     */
    inline GLsizei size() const noexcept { return mCount; }

    /**
     * @brief Get the number of reserved elements of the Vector.
     * @return The number of reserved elements of the Vector.
     */
    inline GLsizei reserved() const noexcept { return mReserved; }

    /**
     * @brief Bind the Vector.
     * @details the Target template parameter will be used for the target.
     */
    inline void bind() const noexcept { glBindBuffer(Target, mBuffer); }

    /**
     * @brief Set the contents of this Vector from an std::vector.
     *
     * @details This method tries to use glBufferSubData as much as possible,
     * and only uses glBufferData if there is not enough space. The
     * reallocation strategy is to increase the size of the Vector by 2.
     *
     * @param v The std::vector to set the Vector with.
     * @param usagehint Specifies the usage hint.
     * @tparam Alloc The allocator type of the std::vector.
     */
    template <class Alloc>
    void set(const std::vector<T, Alloc>& v, const GLenum usagehint)
    {
        mCount = static_cast<GLsizei>(v.size());
        if (mCount < mReserved)
            gtBufferSubData(Target, 0, mCount, v);
        else
        {
            while (mCount >= mReserved) mReserved *= 2;
            glBufferData(Target, sizeof(T) * mReserved, nullptr, usagehint);
            gtBufferSubData(Target, 0, mCount, v);
        }
    }

    /**
     * @brief Clear this Vector of all its elements.
     */
    inline void clear() noexcept { mCount = 0; }

    inline operator GLuint() const noexcept
    {
        return static_cast<GLuint>(mBuffer);
    }

  private:
    BufferObject mBuffer;
    GLsizei      mCount = 0;
    GLsizei      mReserved = 1;
};

/**
 * @brief An array of Vector classes for buffering data to GPU memory.
 *
 * @tparam Target Specifies the target of the buffer. For instance,
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * @tparam T Specifies the type of element to store.
 * @tparam Size Specifies the number of Vectors to use.
 */
template <GLenum Target, class T, GLuint Size> class VectorArray
{
  public:
    /**
     * @brief Constructor.
     *
     * @param usagehint Specifies the usage hint.
     */
    VectorArray(const GLenum usagehint)
    {
        for (GLuint i = 0; i < Size; ++i)
        {
            bind(i);
            mCount[i] = 0;
            mReserved[i] = 1;
            glBufferData(Target, sizeof(T) * mReserved[i], nullptr, usagehint);
        }
    }

    /**
     * @brief Constructor.
     *
     * @param reserve Reserve this many elements.
     * @param usagehint Specifies the usage hint.
     */
    VectorArray(const GLsizei reserve, const GLenum usagehint)
    {
        for (GLuint i = 0; i < Size; ++i)
        {
            bind(i);
            mCount[i] = 0;
            mReserved[i] = reserve;
            glBufferData(Target, sizeof(T) * mReserved[i], nullptr, usagehint);
        }
    }

    /**
     * @brief Get the size of one of the Vectors in the array.
     *
     * @param index The index into the array.
     * @return The size of the Vector at the index location.
     */
    inline GLsizei size(const GLuint index) const noexcept
    {
        return mCount[index];
    }

    /**
     * @brief Get the reserved size of one of the Vectors in the array.
     *
     * @param index The index into the array.
     * @return The reserved size of the Vector at the index location.
     */
    inline GLsizei reserved(const GLuint index) const noexcept
    {
        return mReserved[index];
    }

    /**
     * @brief Bind the Vector at the given index in the array.
     * @details the Target template parameter will be used for the target.
     * @param index The index into the array of Vectors.
     */
    inline void bind(const GLuint index) const noexcept
    {
        glBindBuffer(Target, mBuffer[index]);
    }

    /**
     * @brief Set the contents of the Vector at a given index from an
     * std::vector.
     *
     * @details This method tries to use glBufferSubData as much as possible,
     * and only uses glBufferData if there is not enough space. The
     * reallocation strategy is to increase the size of the Vector by 2.
     *
     * @param index The index into the array of Vectors.
     * @param v The std::vector to set the Vector with.
     * @param usagehint Specifies the usage hint.
     * @tparam Alloc The allocator type of the std::vector.
     */
    template <class Alloc>
    void set(const GLuint index, const std::vector<T, Alloc>& v,
             const GLenum usagehint)
    {
        mCount[index] = static_cast<GLsizei>(v.size());
        if (mCount[index] < mReserved[index])
        {
            gtBufferSubData(Target, 0, mCount[index], v);
        }
        else
        {
            while (mCount[index] >= mReserved[index]) mReserved[index] *= 2;

            glBufferData(Target, sizeof(T) * mReserved[index], nullptr,
                         usagehint);

            gtBufferSubData(Target, 0, mCount[index], v);
        }
    }

    /**
     * @brief Clear the Vector at the index position of the Vector array.
     *
     * @param index The index into the array of Vectors.
     */
    inline void clear(const GLuint index) noexcept { mCount[index] = 0; }

    /**
     * @brief Clear all Vectors in the array of Vectors.
     */
    inline void clearAll() noexcept
    {
        for (GLuint i = 0; i < Size; ++i) mCount[i] = 0;
    }

  private:
    BufferObjectArray<Size> mBuffer;
    GLsizei                 mCount[Size];
    GLsizei                 mReserved[Size];
};

} // namespace OpenGL
} // namespace gintonic
