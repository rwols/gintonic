#ifndef gintonic_timers_hpp
#define gintonic_timers_hpp

#include "renderer.hpp"

namespace gintonic {

/**
 * @brief Base class for timers.
 * 
 * @details A timer class is a class that fires an event after a certain
 * period of time. This event can either keep firing in intervals or it could
 * stop after the first shot, or some combination.
 */
class timer
{
public:

	/// We inherit the duration type of the renderer's duration type.
	typedef renderer::duration_type duration_type;
	
	/**
	 * @brief Constructor.
	 * 
	 * @param time_left Specifies the timing interval.
	 */
	timer(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW;

	/// Destructor.
	virtual ~timer() BOOST_NOEXCEPT_OR_NOTHROW;
	
	/**
	 * @brief Update the timer with the given delta time from the renderer.
	 * 
	 * @param delta_time The delta time. You probably want this to always be
	 * renderer::delta_time() unless you have a specific reason not to.
	 */
	virtual void update(const duration_type& delta_time) 
		BOOST_NOEXCEPT_OR_NOTHROW = 0;

	/**
	 * @brief Get the current time left before the event fires.
	 * @return The time left before the timer event fires.
	 */
	inline const duration_type& time_left() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_time_left;
	}
	
	/**
	 * @brief Check wether this timer is expired.
	 * @return True if the timer is expired, false if not.
	 */
	inline bool expired() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_expired;
	}

	/**
	 * @brief Explicitly set this timer to be expired.
	 */
	inline void set_expired() BOOST_NOEXCEPT_OR_NOTHROW { m_expired = true; }
	
	/**
	 * @brief Reset the timer with a new firing timing interval.
	 * @param time_left The event fire interval.
	 */
	virtual void reset(const duration_type& time_left)
		BOOST_NOEXCEPT_OR_NOTHROW;

	/// The event that fires periodically.
	boost::signals2::signal<void(timer*)> action;

	/// The event that fires when the timer expires.
	boost::signals2::signal<void(timer*)> on_expired;

	/**
	 * @brief Update all alive timers.
	 * 
	 * @param delta_time The delta time. You probably want this to always be
	 * renderer::delta_time() unless you have a specific reason not to.
	 */
	static void update_all(const duration_type& delta_time) 
		BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Add a new timer to the global timer container.
	 * 
	 * @param new_timer A pointer to the new timer.
	 */
	static void add(timer* new_timer);

	/**
	 * @brief Release all timers in the global timer container.
	 */
	static void release();

	/**
	 * @brief Initialize the global timer container.
	 */
	static void init();

protected:

	/// The duration left (or timing interval).
	duration_type m_time_left;

	/// Wether the timer is expired.
	bool m_expired = false;
};

/**
 * @brief One shot timer that only fires once and then expires.
 */
class one_shot_timer : public timer
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param time_left Specifies the timing interval.
	 */
	one_shot_timer(const duration_type& time_left);

	/// Destructor.
	virtual ~one_shot_timer() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void update(const duration_type& dt) 
		BOOST_NOEXCEPT_OR_NOTHROW final;
};

/**
 * @brief Loop timer that continually fires in intervals.
 */
class loop_timer : public timer
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param time_left Specifies the timing interval.
	 */
	loop_timer(const duration_type& time_left);

	/// Destructor.
	virtual ~loop_timer() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void update(const duration_type& dt) 
		BOOST_NOEXCEPT_OR_NOTHROW final;

	virtual void reset(const duration_type& time_left) 
		BOOST_NOEXCEPT_OR_NOTHROW;
		
private:
	duration_type m_original_duration;
};

} // namespace gintonic

#endif