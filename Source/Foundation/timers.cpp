#include "timers.hpp"
#include <vector>

namespace { // anonymous namespace

std::vector<gintonic::Timer*> sTimers;

} // anonymous namespace

namespace gintonic {

Timer::Timer(const duration_type& interval) noexcept : mTimeLeft(interval) {}
Timer::~Timer() noexcept
{
	try
	{
		onExpired(this);
	}
	catch (...)
	{
		// do nothing
	}
}

// do NOT call this method in the destructor !!!!!!!!!!!!
void Timer::reset(const duration_type& interval) noexcept
{
	mTimeLeft = interval;
	mExpired = false;
}

void Timer::updateAll(const duration_type& deltaTime) noexcept
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

void Timer::add(Timer* newTimer)
{
	sTimers.push_back(newTimer);
}

OneShotTimer::OneShotTimer(const duration_type& interval)
: Timer(interval)
{
	/* Empty on purpose. */
}

void OneShotTimer::update(const duration_type& dt) noexcept
{
	if (!mExpired)
	{
		mTimeLeft -= dt;
		if (mTimeLeft <= 0.0f)
		{
			onFire(this);
			mExpired = true;
		}	
	}
}

LoopTimer::LoopTimer(const duration_type& interval)
: Timer(interval)
, mOriginalDuration(interval)
{
	/* Empty on purpose. */
}

void LoopTimer::update(const duration_type& dt) noexcept
{
	mTimeLeft -= dt;
	if (mTimeLeft <= 0.0f)
	{
		onFire(this);
		mTimeLeft = mOriginalDuration;
	}
}

void LoopTimer::reset(const duration_type& interval) noexcept
{
	mTimeLeft = interval;
	mOriginalDuration = mTimeLeft;
}

} // namespace gintonic