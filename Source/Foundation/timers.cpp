#include "timers.hpp"
#include <vector>

namespace { // anonymous namespace

std::vector<gintonic::Timer*> sTimers;

} // anonymous namespace

namespace gintonic {

timer::timer(const duration_type& interval) noexcept : mTimeLeft(interval) {}
timer::~timer() noexcept
{
	try
	{
		OnExpired(this);
	}
	catch (...)
	{
		// do nothing
	}
}

// do NOT call this method in the destructor !!!!!!!!!!!!
void timer::reset(const duration_type& interval) noexcept
{
	mTimeLeft = interval;
	mExpired = false;
}

void timer::updateAll(const duration_type& deltaTime) noexcept
{
	auto t = sTimers.begin();
	while (t != sTimers.end())
	{
		if (*t)
		{
			(*t)->update(deltaTime);
			if ((*t)->isExpired())
			{
				delete *t;
				t = sTimers.erase(t);
				continue;
			}
			++t;
		}
		else
		{
			t = sTimers.erase(t);
			continue;
		}
	}
}

	void timer::add(timer* new_timer)
	{
		sTimers.push_back(new_timer);
	}

	one_shot_timer::one_shot_timer(const duration_type& interval) : timer(interval) {}
	one_shot_timer::~one_shot_timer() noexcept {}
	void one_shot_timer::update(const duration_type& dt) noexcept
	{
		if (!mExpired)
		{
			mTimeLeft -= dt;
			if (mTimeLeft <= duration_type::zero())
			{
				action(this);
				mExpired = true;
			}	
		}
	}

	loop_timer::loop_timer(const duration_type& interval) : timer(interval), m_original_duration(interval) {}
	loop_timer::~loop_timer() noexcept {}
	void loop_timer::update(const duration_type& dt) noexcept
	{
		mTimeLeft -= dt;
		if (mTimeLeft <= duration_type::zero())
		{
			action(this);
			mTimeLeft = m_original_duration;
		}
	}
	void loop_timer::reset(const duration_type& interval) noexcept
	{
		mTimeLeft = interval;
		m_original_duration = mTimeLeft;
	}
} // namespace gintonic