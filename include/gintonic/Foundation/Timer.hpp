/**
 * @file timers.hpp
 * @brief Defines the base class for a Timer.
 * @author Raoul Wols
 */

#pragma once

#include <boost/signals2.hpp>

#include <gintonic/nsbegin.hpp>

template <class DurationType> struct Timer
{
    using duration_type = DurationType;

    enum class Type
    {
        kOneShot,
        kLoop
    };

    Type type;

    duration_type timeLeft;

    bool expired{false};

    boost::signals2::signal<void()> onFire;

    Timer(const Type type, const duration_type timeLeft)
        : type(type), timeLeft(timeLeft), mOriginalDuration(timeLeft)
    {
        assert(timeLeft > 0);
    }

    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;
    Timer(Timer&&) = default;
    Timer& operator=(Timer&&) = default;
    ~Timer() noexcept = default;

    bool update(const duration_type deltaTime) noexcept
    {
        if (!expired)
        {
            switch (type)
            {
            case Type::kOneShot:
                timeLeft -= deltaTime;
                if (timeLeft <= 0)
                {
                    expired = true;
                    onFire();
                }
                break;
            case Type::kLoop:
                timeLeft -= deltaTime;
                if (timeLeft <= 0)
                {
                    onFire();
                    timeLeft = mOriginalDuration;
                }
                break;
            default:
                expired = true;
                break;
            }
        }
        return expired;
    }

  private:
    duration_type mOriginalDuration;
};

#include <gintonic/nsend.hpp>
