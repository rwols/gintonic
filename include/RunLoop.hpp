#pragma once

#include "Math/vec2f.hpp"
#include "Signal.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace gintonic
{

class Window;

class RunLoop
{
  public:
    Signal<void(const vec2f&, const vec2f&)> onMouseMove;
    Signal<void(const vec2f&)> onMouseWheel;
    Signal<void(const vec2f&, const vec2f&)> onFingerMotion;
    Signal<void(int, unsigned short)> onKeyPress;
    Signal<void(int, unsigned short)> onKeyRelease;

    std::vector<std::unique_ptr<Window>> windows;

    RunLoop();

    float getDeltaTime() const noexcept { return mDeltaTime; }
    float getElapsedTime() const noexcept { return mElapsedTime; }

    void run();
    virtual ~RunLoop();

  private:
    float mDeltaTime = 1.0f / 60.0f;
    float mElapsedTime = 0.0f;
    float mPrevElapsedTime = 0.0f;
    std::chrono::high_resolution_clock::time_point mStartTime;

    virtual void runOneFrame() = 0;
    void updateTime() noexcept;
};

} // gintonic
