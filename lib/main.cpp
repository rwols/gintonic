#include "QuitApplication.hpp"
#include "SDLRenderContext.hpp"
#include "SDLRunLoop.hpp"
#include "SDLWindow.hpp"

int main(int argc, char** argv)
{
    namespace gt = gintonic;
    gt::SDLRunLoop loop;
    {
        std::unique_ptr<gt::Window> window(new gt::SDLWindow("test", 500, 500));
        window->context.reset(new gt::SDLRenderContext(
            static_cast<gt::SDLWindow&>(*window), 3, 3));
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
