/**
 * @file timers.hpp
 * @brief Defines the base class for a Timer.
 * @author Raoul Wols
 */

#pragma once

namespace gintonic {

/**
 * @brief Base class for timers.
 */
class Timer
{
public:

	typedef float duration_type;
	
	/**
	 * @brief Constructor.
	 * 
	 * @param time_left Specifies the timing interval.
	 */
	timer(const duration_type& timeLeft) noexcept;

	/// Destructor.
	virtual ~timer() noexcept = default;

	/**
	 * @brief Get the current time left before the event fires.
	 * @return The time left before the timer event fires.
	 */
	inline const duration_type& timeLeft() const noexcept
	{
		return mTimeLeft;
	}
	
	/**
	 * @brief Check wether this timer is expired.
	 * @return True if the timer is expired, false if not.
	 */
	inline bool isExpired() const noexcept
	{
		return mExpired;
	}

	/**
	 * @brief Explicitly set this timer to be expired.
	 */
	inline void setExpired() noexcept
	{
		mExpired = true;
	}
	
	/**
	 * @brief Reset the timer with a new firing timing interval.
	 * @param interval The event fire interval.
	 */
	virtual void reset(const duration_type& interval) noexcept;

	/// The event that fires periodically.
	boost::signals2::signal<void(timer*)> onFire;

	/// The event that fires when the timer expires.
	boost::signals2::signal<void(timer*)> onExpired;

	/**
	 * @brief Update all alive timers.
	 * 
	 * @param deltaTime The delta time. You probably want this to always be
	 * Renderer::deltaTime() unless you have a specific reason not to.
	 */
	static void updateAll(const duration_type& deltaTime) 
		noexcept;

	/**
	 * @brief Add a new timer to the global timer container.
	 * 
	 * @param newTimer A pointer to the new timer.
	 */
	static void add(Timer* newTimer);

	/**
	 * @brief Release all timers in the global timer container.
	 */
	static void release();

	/**
	 * @brief Initialize the global timer container.
	 */
	static void initialize();

protected:

	/// The duration left (or timing interval).
	duration_type mTimeLeft;

	/// Wether the timer is expired.
	bool mExpired = false;

private:

	/**
	 * @brief Update the timer with the given delta time from the renderer.
	 * 
	 * @param deltaTime The delta time. You probably want this to always be
	 * Renderer::deltaTime() unless you have a specific reason not to.
	 */
	virtual void update(const duration_type& deltaTime) noexcept = 0;
};

/**
 * @brief One-shot-timer that only fires once and then expires.
 */
class OneShotTimer : public Timer
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param timeLeft Specifies the timing interval.
	 */
	OneShotTimer(const duration_type& timeLeft);

	/// Destructor.
	virtual ~OneShotTimer() noexcept = default;

private:

	virtual void update(const duration_type& dt) noexcept final;
};

/**
 * @brief Loop timer that continually fires in intervals.
 */
class LoopTimer : public Timer
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param timeLeft Specifies the timing interval.
	 */
	LoopTimer(const duration_type& timeLeft);

	/// Destructor.
	virtual ~LoopTimer() noexcept = default;

	virtual void reset(const duration_type& timeLeft) noexcept;
		
private:

	duration_type moriginal_duration;

	virtual void update(const duration_type& dt) noexcept final;

};

} // namespace gintonic
