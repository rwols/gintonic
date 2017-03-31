#pragma once

#include <chrono>
#include <iosfwd>

#define GT_IDEAL_FRAMES_PER_SECOND 1.0f / 60.0f

namespace gintonic
{

/**
 * @brief      A floating point clock with an update function for use in a
 *             rendering main-loop.
 */
class Clock
{
  public:
    /**
     * The internal representation of this clock is a floating point number.
     */
    using rep = float;

    /*
     * The period type of this clock is 1.
     */
    using period = std::ratio<1, 1>;

    /*
     * The duration type of this clock is measured in seconds, with an internal
     * representation using floating point numbers.
     */
    using duration = std::chrono::duration<rep, period>;

    /*
     * The time point type of this clock uses the standard machinery of the
     * chrono library, using the duration type of this clock.
     */
    using time_point = std::chrono::time_point<Clock>;

    /**
     * @brief      Always returns true.
     *
     * @return     True.
     */
    static const bool is_steady() noexcept { return true; }

    /**
     * @brief      Sets the current now and nowUnscaled to zero, and sets the
     *             the delta times to an "ideal" 1/60 seconds.
     */
    Clock();

    /**
     * The time scale determines how fast or slow this clock is actually
     * ticking. Negative time scales are possible. A time scale of zero means
     * that time is frozen, and in that case the delta time will always be zero.
     */
    float timeScale = 1.0f;

    /**
     * @brief      Get the current delta time, scaled with the current
     *             timeScale. This function only changes its value once update()
     *             is called.
     *
     * @return     The current scaled delta time.
     */
    inline duration deltaTime() const noexcept { return mDeltaTime; }

    /**
     * @brief      Get the current unscaled delta time. This method only changes
     *             its value once update() is called.
     *
     * @return     The current unscaled delta time.
     */
    inline duration deltaTimeUnscaled() const noexcept
    {
        return mDeltaUnscaled;
    }

    /**
     * @brief      Get the current elapsed time since construction of this
     *             clock, scaled with a history of timeScale. This function only
     *             changes its value once update() is called.
     *
     * @return     The current elapsed time.
     */
    inline time_point now() const noexcept { return mNow; }

    /**
     * @brief      Get the current unscaled elapsed time since construction of
     *             this clock. This function only changes its vlaue once
     *             update() is called.
     *
     * @return     The current unscaled elapsed time.
     */
    inline time_point nowUnscaled() const noexcept { return mNowUnscaled; }

    /**
     * @brief      Updates the now, nowUnscaled, deltaTime and deltaTimeUnscaled
     *             methods since the last call of update. You should call this
     *             method at the start or end of your rendering loop.
     */
    void update() noexcept;

    /**
     * @brief      Debug print to an ostream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Clock& clock);

  private:
#ifdef __APPLE__

    const uint64_t mEpochStart;
    uint64_t mNanoSecs;

#elif defined(_WIN32)

#elif defined(__linux__)

#endif

    time_point mNowUnscaled;
    time_point mNow;
    duration mDeltaUnscaled = duration(GT_IDEAL_FRAMES_PER_SECOND);
    duration mDeltaTime = duration(GT_IDEAL_FRAMES_PER_SECOND);
};

using TimePoint = Clock::time_point;
using Seconds = std::chrono::duration<Clock::rep, std::ratio<1, 1>>;
using MilliSeconds = std::chrono::duration<Clock::rep, std::ratio<1, 1000>>;
using MicroSeconds = std::chrono::duration<Clock::rep, std::ratio<1, 1000000>>;
using Minutes = std::chrono::duration<Clock::rep, std::ratio<60, 1>>;

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const Clock::time_point& timePoint);

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const Clock::duration& duration);

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const TimePoint& timePoint);

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const Seconds& duration);

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const MilliSeconds& duration);

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const MicroSeconds& duration);

/**
 * @brief      Debug print to an ostream.
 */
std::ostream& operator<<(std::ostream& os, const Minutes& duration);

} // namespace gintonic
