#include "Foundation/WriteLock.hpp"

namespace gintonic {

WriteLock::WriteLock()
: mMutex()
, mConditionVariable()
, mWaitingWriters(0)
, mIsLocked(false)
{
	/* Empty on purpose. */
}

void WriteLock::obtain()
{
	std::unique_lock<std::mutex> lock(mMutex);
	++mWaitingWriters;
	while (mIsLocked) mConditionVariable.wait(lock);
	mIsLocked = true;
	lock.unlock();
}

void WriteLock::release()
{
	std::unique_lock<std::mutex> lock(mMutex);
	--mWaitingWriters;
	mIsLocked = false;
	if (mWaitingWriters > 0) mConditionVariable.notify_one();
	lock.unlock();
}

} // namespace gintonic