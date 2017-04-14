#pragma once

#include "RunLoop.hpp"

namespace gintonic
{

class SDLWindow;

class SDLRunLoop : public RunLoop
{
  public:
    ~SDLRunLoop() override = default;

  private:
    void runOneFrame() override final;
    SDLWindow* mWindowInFocus = nullptr;
};

} // gintonic
