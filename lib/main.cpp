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
        window->onEnter.connect([]() { std::cout << "Mouse entered.\n"; });
        window->onLeave.connect([]() { std::cout << "Mouse left.\n"; });
        window->show();
        loop.windows.push_back(std::move(window));
    }
    loop.onMouseMove.connect([](const gt::vec2f& pos, const gt::vec2f& delta) {
        std::cout << "Mouse moved: " << pos << ", delta: " << delta << '\n';
    });
    loop.onKeyPress.connect([](int code, unsigned short mod) {
        std::cout << "Key pressed: " << code << " (mod " << mod << ")\n";
    });
    loop.onKeyRelease.connect([](int code, unsigned short mod) {
        std::cout << "Key released: " << code << " (mod " << mod << ")\n";
    });
    loop.onFingerMotion.connect([](const gt::vec2f& pos,
                                   const gt::vec2f& delta) {
        std::cout << "Finger motion: " << pos << ", delta: " << delta << '\n';
    });
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
