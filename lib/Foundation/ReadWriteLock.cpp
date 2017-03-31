#include <gintonic/Foundation/ReadWriteLock.hpp>

using namespace gintonic;

ReadWriteLock::ReadWriteLock()
    : mMutex(), mReader(), mWriter(), mActiveReaders(0), mWaitingWriters(0),
      mActiveWriters(0)
{
    /* Empty on purpose. */
}

void ReadWriteLock::obtainRead() const
{
    std::unique_lock<std::mutex> lock(mMutex);
    while (mWaitingWriters != 0) mReader.wait(lock);
    ++mActiveReaders;
    lock.unlock();
}

void ReadWriteLock::releaseRead() const
{
    std::unique_lock<std::mutex> lock(mMutex);
    --mActiveReaders;
    lock.unlock();
    mWriter.notify_one();
}

void ReadWriteLock::obtainWrite()
{
    std::unique_lock<std::mutex> lock(mMutex);
    ++mWaitingWriters;
    while (mActiveReaders != 0 || mActiveWriters != 0) mWriter.wait(lock);
    ++mActiveWriters;
    lock.unlock();
}

void ReadWriteLock::releaseWrite()
{
    std::unique_lock<std::mutex> lock(mMutex);
    --mWaitingWriters;
    --mActiveWriters;
    if (mWaitingWriters > 0)
        mWriter.notify_one();
    else
        mReader.notify_all();
    lock.unlock();
}
