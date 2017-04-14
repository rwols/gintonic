#include "SDLRenderContext.hpp"
#include "SDLWindow.hpp"
#include "glad/glad.h"
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
    if (gladLoadGL() != 1)
    {
        SDL_GL_DeleteContext(mHandle);
        throw std::bad_alloc();
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    resize();
}

SDLRenderContext::~SDLRenderContext() { SDL_GL_DeleteContext(mHandle); }

void SDLRenderContext::present()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
