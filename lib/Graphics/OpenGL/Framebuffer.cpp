#include <gintonic/Graphics/OpenGL/Framebuffer.hpp>

using namespace gintonic;
using namespace gintonic::OpenGL;

const char*
Framebuffer::Exception::statusCodeToErrorString(const GLenum status) noexcept
{
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        return "GL_FRAMEBUFFER_COMPLETE";
    case GL_FRAMEBUFFER_UNDEFINED:
        return "GL_FRAMEBUFFER_UNDEFINED";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "GL_FRAMEBUFFER_UNSUPPORTED";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    default:
        return "Unknown status code";
    }
}

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &mHandle);
    if (!mHandle) throw std::bad_alloc();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other)
{
    this->~Framebuffer();
    mHandle = other.mHandle;
    other.mHandle = 0;
    return *this;
}

void Framebuffer::bind(const GLenum token) const noexcept
{
    glBindFramebuffer(token, mHandle);
}

void Framebuffer::checkStatus() const
{
    const auto lStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (lStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        throw Exception(lStatus);
    }
}

void Framebuffer::checkStatus(bool& r) const noexcept
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        r = false;
    }
    else
        r = true;
}
