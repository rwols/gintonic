#include "ApplicationStateMachine.hpp"
#include "QuitApplication.hpp"
#include "SDLRenderContext.hpp"
#include "SDLRunLoop.hpp"
#include "SDLWindow.hpp"
#include <SDL.h>

int main(int argc, char** argv)
{
    namespace gt = gintonic;
    gt::SDLRunLoop loop;
    loop.machine.reset(new gt::ApplicationStateMachine(loop));
    {
        std::unique_ptr<gt::Window> window(new gt::SDLWindow("test", 500, 500));
        try
        {
            window->context.reset(new gt::SDLRenderContext(
                static_cast<gt::SDLWindow&>(*window), 4, 1));
        }
        catch (...)
        {
        }
        if (!window->context)
        {
            window->context.reset(new gt::SDLRenderContext(
                static_cast<gt::SDLWindow&>(*window), 3, 3));
        }
        window->show();
        loop.windows.push_back(std::move(window));
    }
    try
    {
        loop.run();
    }
    catch (const gt::QuitApplication&)
    {
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// int main()
// {

//     statemachine.process_event(EvStartStop());
//     statemachine.process_event(EvStartStop());
//     statemachine.process_event(EvStartStop());
//     statemachine.process_event(EvReset());
//     return 0;
// }
