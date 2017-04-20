#include "ApplicationStateMachine.hpp"
#include "QuitApplication.hpp"
#include "RenderContext.hpp"
#include "RunLoop.hpp"
#include "Window.hpp"
#include <SDL.h>
#include <iostream>

using namespace gintonic;

ApplicationStateMachine::ApplicationStateMachine(RunLoop& runLoop)
    : runLoop(runLoop)
{
}

InitializeState::InitializeState(my_context ctx) : my_base(ctx)
{
    std::cout << context<ApplicationStateMachine>()
                     .runLoop.windows[0]
                     ->context->getName()
              << '\n'
              << context<ApplicationStateMachine>()
                     .runLoop.windows[0]
                     ->context->getVersion()
              << '\n';
    post_event(EvContinue());
}

SplashScreenState::SplashScreenState(my_context ctx)
    : my_base(ctx),
      mStart(context<ApplicationStateMachine>().runLoop.getElapsedTime())
{
    std::cout << "\n... Splash screen ...\n";
}

boost::statechart::result SplashScreenState::react(const EvUpdate&)
{
    const auto t = context<ApplicationStateMachine>().runLoop.getElapsedTime();
    if (t - mStart >= 2.0f)
    {
        post_event(EvContinue());
    }
    return discard_event();
}

MainMenuState::MainMenuState(my_context ctx)
    : my_base(ctx),
      keyHandler(context<ApplicationStateMachine>().runLoop.onKeyPress.connect(
          [this](const int code, const unsigned int mod) {
              switch (code)
              {
              case SDLK_c:
                  post_event(EvContinue());
                  break;
              case SDLK_n:
                  post_event(EvNewGame());
                  break;
              case SDLK_o:
                  post_event(EvOptions());
                  break;
              case SDLK_q:
                  throw gintonic::QuitApplication();
              }
          })),
      scene("none")
{
    std::cout << "\nMain Menu:\n"
              << " * [c]ontinue\n"
              << " * [n]ew game\n"
              << " * [o]ptions\n"
              << " * [q]uit\n";
}

boost::statechart::result MainMenuState::react(const EvUpdate&)
{
    scene.update();
    return discard_event();
}
