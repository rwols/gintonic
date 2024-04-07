#include "SDLRenderContext.hpp"
#include "Math/vec4f.hpp"
#include "SDLWindow.hpp"
#include "glad/gl.h"
#include <SDL.h>

using namespace gintonic;

SDLRenderContext::SDLRenderContext(SDLWindow& owner, const int major,
                                   const int minor)
    : RenderContext(owner)
{
    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    mHandle = SDL_GL_CreateContext(owner.mHandle);
    if (!mHandle)
    {
        throw std::out_of_range(
            "no context with specified major.minor available");
    }
    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    resize();
}

SDLRenderContext::~SDLRenderContext() { SDL_GL_DeleteContext(mHandle); }

void SDLRenderContext::present()
{
    SDL_GL_SwapWindow(static_cast<SDLWindow&>(getWindow()).mHandle);
}

void SDLRenderContext::resize()
{
    glViewport(0, 0, getWindow().getWidth(), getWindow().getHeight());
}

void SDLRenderContext::focus() const noexcept
{
    SDL_GL_MakeCurrent(static_cast<const SDLWindow&>(getWindow()).mHandle,
                       mHandle);
}

const char* SDLRenderContext::getName() const noexcept
{
    return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

const char* SDLRenderContext::getVersion() const noexcept
{
    return reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

void SDLRenderContext::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SDLRenderContext::setClearColor(const vec4f& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}

void SDLRenderContext::setVirtualSynchronization(const bool yesOrNo)
{
    SDL_GL_SetSwapInterval(yesOrNo ? 1 : 0);
}
