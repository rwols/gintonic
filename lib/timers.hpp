#ifndef gintonic_timers_hpp
#define gintonic_timers_hpp

#include "renderer.hpp"

namespace gintonic
{
	class timer
	{
	public:
		typedef renderer::duration_type duration_type;
		
		timer(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW;

		virtual ~timer() BOOST_NOEXCEPT_OR_NOTHROW;
		
		virtual void update(const duration_type& delta_time) BOOST_NOEXCEPT_OR_NOTHROW = 0;

		inline const duration_type& time_left() const BOOST_NOEXCEPT_OR_NOTHROW  { return m_time_left; }
		
		inline bool expired() const BOOST_NOEXCEPT_OR_NOTHROW { return m_expired; }

		inline void set_expired() BOOST_NOEXCEPT_OR_NOTHROW { m_expired = true; }
		
		virtual void reset(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW;

		boost::signals2::signal<void(timer*)> action;
		boost::signals2::signal<void(timer*)> on_expired;

		static void update_all(const duration_type& delta_time) BOOST_NOEXCEPT_OR_NOTHROW;
		static void add(timer* new_timer);
		static void release();
		static void init();

	protected:
		duration_type m_time_left;
		bool m_expired = false;
	};

	class one_shot_timer : public timer
	{
	public:
		one_shot_timer(const duration_type& time_left);
		virtual ~one_shot_timer() BOOST_NOEXCEPT_OR_NOTHROW;
		virtual void update(const duration_type& dt) BOOST_NOEXCEPT_OR_NOTHROW final;
	};

	class loop_timer : public timer
	{
	public:
		loop_timer(const duration_type& time_left);
		virtual ~loop_timer() BOOST_NOEXCEPT_OR_NOTHROW;
		virtual void update(const duration_type& dt) BOOST_NOEXCEPT_OR_NOTHROW final;
		virtual void reset(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW;
	private:
		duration_type m_original_duration;
	};

} // namespace gintonic

#endif