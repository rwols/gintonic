#include "RunLoop.hpp"
#include "ApplicationStateMachine.hpp"
#include "Window.hpp"
#include <chrono>

using namespace gintonic;

RunLoop::RunLoop() : mStartTime(std::chrono::high_resolution_clock::now()) {}

RunLoop::~RunLoop() {}

void RunLoop::run()
{
    if (machine) machine->initiate();
    while (true)
    {
        updateTime();
        if (machine) machine->process_event(EvUpdate());
        runOneFrame();
    }
}

void RunLoop::updateTime() noexcept
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::seconds;
    mPrevElapsedTime = mElapsedTime;
    const duration<float> d = high_resolution_clock::now() - mStartTime;
    mElapsedTime = d.count();
    mDeltaTime = mElapsedTime - mPrevElapsedTime;
}
