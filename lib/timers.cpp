#include "timers.hpp"
#include <vector>

namespace
{
	std::vector<gintonic::timer*> s_timers;
}

namespace gintonic
{
	timer::timer(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW : m_time_left(time_left) {}
	timer::~timer() BOOST_NOEXCEPT_OR_NOTHROW
	{
		try
		{
			on_expired(this);
		}
		catch (...)
		{
			// do nothing
		}
	}
	void timer::reset(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_time_left = time_left;
		m_expired = false;
	}

	void timer::update_all(const duration_type& delta_time) BOOST_NOEXCEPT_OR_NOTHROW
	{
		auto t = s_timers.begin();
		while (t != s_timers.end())
		{
			if (*t)
			{
				(*t)->update(delta_time);
				if ((*t)->expired())
				{
					delete *t;
					t = s_timers.erase(t);
					continue;
				}
				++t;
			}
			else
			{
				t = s_timers.erase(t);
				continue;
			}
		}
	}

	void timer::add(timer* new_timer)
	{
		s_timers.push_back(new_timer);
	}

	one_shot_timer::one_shot_timer(const duration_type& time_left) : timer(time_left) {}
	one_shot_timer::~one_shot_timer() BOOST_NOEXCEPT_OR_NOTHROW {}
	void one_shot_timer::update(const duration_type& dt) BOOST_NOEXCEPT_OR_NOTHROW
	{
		if (!m_expired)
		{
			m_time_left -= dt;
			if (m_time_left <= duration_type::zero())
			{
				action(this);
				m_expired = true;
			}	
		}
	}

	loop_timer::loop_timer(const duration_type& time_left) : timer(time_left), m_original_duration(time_left) {}
	loop_timer::~loop_timer() BOOST_NOEXCEPT_OR_NOTHROW {}
	void loop_timer::update(const duration_type& dt) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_time_left -= dt;
		if (m_time_left <= duration_type::zero())
		{
			action(this);
			m_time_left = m_original_duration;
		}
	}
	void loop_timer::reset(const duration_type& time_left) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_time_left = time_left;
		m_original_duration = m_time_left;
	}
} // namespace gintonic