#include <Foundation/Clock.hpp>

#include <iostream>

#ifdef __APPLE__

#include <mach/mach.h>
#include <mach/mach_time.h>

static uint64_t getNanoSecondsNow()
{
    static mach_timebase_info_data_t sTimebaseInfo;
    if (sTimebaseInfo.denom == 0) mach_timebase_info(&sTimebaseInfo);
    auto elapsed = mach_absolute_time();
    return elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
}

#elif defined(_WIN32)

#elif defined(__linux__)

#include <time.h>

#endif

using namespace gintonic;

Clock::Clock()
#ifdef __APPLE__
    : mEpochStart(getNanoSecondsNow()), mNanoSecs(0), mNowUnscaled(duration(0)),
      mNow(duration(0))
{

#elif defined(_WIN32)
{
#elif defined(__linux__)
{
#endif
}

void Clock::update() noexcept
{
#ifdef __APPLE__

    const auto prevNanoSecs = mNanoSecs;
    mNanoSecs = getNanoSecondsNow() - mEpochStart;
    const auto deltaNanoSecs = mNanoSecs - prevNanoSecs;
    const auto deltaSecs = static_cast<double>(deltaNanoSecs) / 1e9;
    mNowUnscaled = time_point(duration((float)((double)mNanoSecs / 1e9)));
    mNow += duration((float)(deltaSecs * (double)timeScale));
    mDeltaUnscaled = duration(static_cast<float>(deltaSecs));
    mDeltaTime = mDeltaUnscaled * timeScale;

#elif defined(_WIN32)

#elif defined(__linux__)

#endif
}

std::ostream &gintonic::operator<<(std::ostream &os, const Clock &clock)
{
    os << "<gintonic::Clock " << &clock << " with now scaled at " << clock.now()
       << " and unscaled at " << clock.nowUnscaled()
       << ", current timeScale is " << clock.timeScale << ">";
    return os;
}
std::ostream &gintonic::operator<<(std::ostream &os,
                                   const Clock::time_point &timePoint)
{
    return os << timePoint.time_since_epoch();
}
std::ostream &gintonic::operator<<(std::ostream &os,
                                   const Clock::duration &duration)
{
    return os << duration.count() << "s";
}
std::ostream &gintonic::operator<<(std::ostream &os,
                                   const MilliSeconds &duration)
{
    return os << duration.count() << "ms";
}
std::ostream &gintonic::operator<<(std::ostream &os,
                                   const MicroSeconds &duration)
{
    return os << duration.count() << "us";
}
std::ostream &gintonic::operator<<(std::ostream &os, const Minutes &duration)
{
    return os << duration.count() << " minutes";
}
