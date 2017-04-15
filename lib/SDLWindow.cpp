#include "SDLWindow.hpp"
#include "QuitApplication.hpp"
#include "RenderContext.hpp"
#include <../src/video/SDL_sysvideo.h>
#include <SDL.h>

using namespace gintonic;

SDLWindow::SDLWindow(const char* title)
{
    preInit();
    const auto numDisplays = SDL_GetNumVideoDisplays();
    if (numDisplays <= 0)
    {
        throw std::logic_error("no displays");
    }
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);
    postInit(title, rect.w, rect.h,
             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
                 SDL_WINDOW_FULLSCREEN_DESKTOP);
}

SDLWindow::SDLWindow(const char* title, const int width, const int height)
{
    preInit();
    postInit(title, width, height,
             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
}

void SDLWindow::preInit()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

void SDLWindow::postInit(const char* title, const int width, const int height,
                         const uint32_t flags)
{
    mHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (!mHandle)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        throw std::bad_alloc();
    }
    updateAspectRatio();
}

SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(mHandle);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

int SDLWindow::getWidth() const noexcept { return mHandle->w; }

int SDLWindow::getHeight() const noexcept { return mHandle->h; }

void SDLWindow::updateAspectRatio() noexcept
{
    mAspectRatio =
        static_cast<float>(getWidth()) / static_cast<float>(getHeight());
}

void SDLWindow::resize(const int /*newWidth*/, const int /*newHeight*/)
{
    updateAspectRatio();
    if (context) context->resize();
}

void SDLWindow::show() noexcept { SDL_ShowWindow(mHandle); }

void SDLWindow::hide() noexcept { SDL_HideWindow(mHandle); }

int SDLWindow::getID() const noexcept { return SDL_GetWindowID(mHandle); }
