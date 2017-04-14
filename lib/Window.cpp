#include "Window.hpp"
#include "RenderContext.hpp"

using namespace gintonic;

Window::Window() {}
Window::~Window() {}

void Window::present() { context->present(); }
