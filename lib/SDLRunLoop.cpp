#include "SDLRunLoop.hpp"
#include "QuitApplication.hpp"
#include "SDLWindow.hpp"
#include <SDL.h>

using namespace gintonic;

void SDLRunLoop::runOneFrame()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_WINDOWEVENT:
        {
            for (auto& w : windows)
            {
                auto wind = static_cast<SDLWindow*>(w.get());
                if (wind->getID() == event.window.windowID)
                {
                    switch (event.window.event)
                    {
                    case SDL_WINDOWEVENT_ENTER:
                        wind->onEnter();
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        wind->onLeave();
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        wind->resize((int)event.window.data1,
                                     (int)event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        mWindowInFocus = wind;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        break;
                    }
                }
            }
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if (!mWindowInFocus) break;
            // Make the position relative
            const vec2f pos(
                2.f * event.motion.x / mWindowInFocus->getWidth() - 1.f,
                2.f * (1.f - event.motion.y / mWindowInFocus->getHeight()) -
                    1.f);
            const vec2f delta(static_cast<float>(event.motion.xrel),
                              static_cast<float>(event.motion.yrel));
            onMouseMove(pos, delta);
            break;
        }
        case SDL_MOUSEWHEEL:
            onMouseWheel(vec2f(static_cast<float>(event.wheel.x),
                               static_cast<float>(event.wheel.y)));
            break;
        case SDL_FINGERMOTION:
        {
            const vec2f pos(event.tfinger.x, event.tfinger.y);
            const vec2f delta(event.tfinger.dx, event.tfinger.dy);
            onFingerMotion(pos, delta);
            break;
        }

        case SDL_KEYDOWN:
            if (event.key.repeat) break;
            onKeyPress((int)event.key.keysym.sym,
                       (unsigned short)event.key.keysym.mod);
            break;
        case SDL_KEYUP:
            onKeyRelease((int)event.key.keysym.scancode,
                         (unsigned short)event.key.keysym.mod);
            break;
        case SDL_QUIT:
            throw QuitApplication();
        }
    }
    for (auto&& w : windows)
    {
        w->present();
    }
}
