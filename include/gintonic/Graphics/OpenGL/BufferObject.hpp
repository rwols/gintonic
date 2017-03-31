/**
 * @file BufferObject.hpp
 * @brief Defines the OpenGL Buffer Object class.
 * @author Raoul Wols
 */

#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <gintonic/Graphics/OpenGL/utilities.hpp>

#include <gintonic/nsbegin.hpp>

namespace OpenGL
{

/**
 * @brief Buffer object that encapsulates an
 * [OpenGL buffer object](https://www.opengl.org/wiki/BufferObject).
 */
class BufferObject
{
  private:
    GLuint mHandle;

  public:
    /// You can access the underlying OpenGL handle via a static_cast.
    inline operator GLuint() const noexcept { return mHandle; }

    /// Default constructor.
    BufferObject();

    /**
     * @brief Constructor.
     * @details The constructor makes the BufferObject act as an OpenGL
     * Vertex Buffer. It binds the buffer as defined by the target parameter
     * and fills the buffer with the contents located at the data pointer.
     *
     * @param target The target of the binding point. e.g. GL_ARRAY_BUFFER.
     * @param size The number of bytes located at the data pointer.
     * @param data The raw data pointer.
     * @param usage A usage hint. e.g. GL_STATIC_DRAW or GL_DYNAMIC_DRAW.
     */
    BufferObject(const GLenum target, const GLsizei size, const GLvoid* data,
                 const GLenum usage);

    /**
     * @brief Constructor.
     * @details The constructor makes the BufferObject act as an OpenGL
     * Vertex Buffer. It binds the buffer as defined by the target parameter
     * and fills the buffer with the contents of the contiguous array.
     *
     * @param target The target of the binding point. e.g. GL_ARRAY_BUFFER.
     * @param data An STL-compliant contiguous array, like std::vector or
     *        std::array.
     * @param usage A usage hint. e.g. GL_STATIC_DRAW or GL_DYNAMIC_DRAW.
     */
    template <class ContiguousArrayContainer>
    BufferObject(const GLenum target, const ContiguousArrayContainer& data,
                 const GLenum usage);

    /**
     * @brief Copy constructor. (Yes, you can copy OpenGL buffers!)
     * @details The copying is all done inside the GPU. There are no memory
     * transfers from GPU memory to CPU memory (and vice versa).
     *
     * Copying buffers is slow, so try to avoid it.
     *
     * @param other Another BufferObject.
     */
    BufferObject(const BufferObject& other);

    /**
     * @brief Move constructor.
     * @details Moving a BufferObject is very simple. We copy over the OpenGL
     * handle and set the OpenGL handle of the other BufferObject to zero.
     * When the destructor of the other BufferObject is called, it will do
     * nothing.
     *
     * @param other Another BufferObject.
     */
    inline BufferObject(BufferObject&& other) noexcept : mHandle(other.mHandle)
    {
        other.mHandle = 0;
    }

    /**
     * @brief Copy assignment operator (Yes, you can copy OpenGL buffers!)
     * @details The copying is all done inside the GPU. There are no memory
     * transfers from GPU memory to CPU memory (and vice versa).
     *
     * Copying buffers is slow, so try to avoid it.
     *
     * @param other Another BufferObject.
     *
     * @return *this.
     */
    BufferObject& operator=(const BufferObject& other);

    /**
     * @brief Move constructor.
     * @details Moving a BufferObject is very simple. We copy over the OpenGL
     * handle and set the OpenGL handle of the other BufferObject to zero.
     * When the destructor of the other BufferObject is called, it will do
     * nothing.
     *
     * @param other Another BufferObject.
     *
     * @return *this.
     */
    BufferObject& operator=(BufferObject&& other) noexcept;

    /**
     * @brief Destructor.
     * @details The destructor makes a call to the OpenGL API to delete the
     * contents of the buffer via the encapsulated handle.
     */
    inline ~BufferObject() noexcept { glDeleteBuffers(1, &mHandle); }

    /**
     * @brief Swap this BufferObject with another BufferObject.
     * @param other Another BufferObject.
     */
    inline void swap(BufferObject& other) { std::swap(mHandle, other.mHandle); }

    /**
     * @brief Retrieve the data contained in this BufferObject.
     * @tparam Type The type of data to fetch.
     * @tparam Allocator The allocator for the std::vector.
     * @param [out] usage The usage type.
     * @return The data contained in this BufferObject.
     */
    template <class Type, class Allocator = std::allocator<Type>>
    std::vector<Type, Allocator> retrieveDataAs(GLint& usage) const
    {
        GLint lSize;
        glBindBuffer(GL_COPY_READ_BUFFER, mHandle);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &lSize);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
        std::vector<Type, Allocator> lResult(lSize);
        glGetBufferSubData(GL_COPY_READ_BUFFER, 0, lSize,
                           (GLvoid*)lResult.data());
        return lResult;
    }

  private:
    friend boost::serialization::access;

    template <class Archive>
    void save(Archive& ar, const unsigned /*version*/) const
    {
        GLint size;
        GLint usage;
        glBindBuffer(GL_COPY_READ_BUFFER, mHandle);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
        std::vector<char> data(size);
        glGetBufferSubData(GL_COPY_READ_BUFFER, 0, size, (GLvoid*)data.data());
        ar& data& usage;
    }

    template <class Archive> void load(Archive& ar, const unsigned /*version*/)
    {
        GLint usage;
        std::vector<char> data;
        ar& data& usage;
        glBindBuffer(GL_COPY_WRITE_BUFFER, mHandle);
        gtBufferData(GL_COPY_WRITE_BUFFER, data, static_cast<GLenum>(usage));
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
};

//!@cond
template <class C>
BufferObject::BufferObject(const GLenum target, const C& data,
                           const GLenum usage)
    : mHandle(0)
{
    glGenBuffers(1, &mHandle);
    if (!mHandle) throw std::bad_alloc();
    glBindBuffer(target, mHandle);
    gtBufferData(target, data, usage);
    glBindBuffer(target, 0);
}
//!@endcond

} // namespace OpenGL

#include <gintonic/nsend.hpp>
