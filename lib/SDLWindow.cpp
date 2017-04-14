#include "SDLWindow.hpp"
#include "QuitApplication.hpp"
#include "RenderContext.hpp"
#include <SDL.h>

using namespace gintonic;

SDLWindow::SDLWindow(const char* title)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        throw std::runtime_error(SDL_GetError());
    }
    const auto numDisplays = SDL_GetNumVideoDisplays();
    if (numDisplays <= 0)
    {
        throw std::logic_error("no displays");
    }
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);
    mWidth = rect.w;
    mHeight = rect.h;
    mAspectRatio = static_cast<float>(mWidth) / static_cast<float>(mHeight);
    const Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                         SDL_WINDOW_HIDDEN | SDL_WINDOW_FULLSCREEN_DESKTOP;
    mHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, flags);
    if (!mHandle)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        throw std::bad_alloc();
    }
}

SDLWindow::SDLWindow(const char* title, const int width, const int height)
    : mWidth(width), mHeight(height),
      mAspectRatio(static_cast<float>(mWidth) / static_cast<float>(mHeight))
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        throw std::runtime_error(SDL_GetError());
    }
    const Uint32 flags =
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    mHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, flags);
    if (!mHandle)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        throw std::bad_alloc();
    }
}

SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(mHandle);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SDLWindow::resize(const int newWidth, const int newHeight)
{
    mWidth = newWidth;
    mHeight = newHeight;
    mAspectRatio = static_cast<float>(mWidth) / static_cast<float>(mHeight);
    context->resize();
}

void SDLWindow::show() noexcept { SDL_ShowWindow(mHandle); }

void SDLWindow::hide() noexcept { SDL_HideWindow(mHandle); }

int SDLWindow::getID() const noexcept { return SDL_GetWindowID(mHandle); }
