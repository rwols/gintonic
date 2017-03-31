/**
 * @file WriteLock.hpp
 * @brief Defines a write lock class.
 * @author Raoul Wols
 */

#pragma once

#include <condition_variable>
#include <mutex>

#include <gintonic/nsbegin.hpp>

// Unfortunately, std::condition_variable
// can throw all sorts of exceptions, so
// there's no no-exception guarantee of any sort.

/**
 * @brief Write lock.
 *
 * @details A write lock is a simple lock that any thread can obtain. Once a
 * thread has obtained the lock, no other threads can access the lock. There
 * is no possibility to have multiple readers. For that, use the
 * ReadWriteLock.
 *
 * @sa ReadWriteLock
 */
class WriteLock
{
  public:
    /// Default constructor.
    WriteLock();

    /**
     * @brief Obtain the lock.
     *
     * @details If another thread already owns the lock, then the thread
     * that attempts to obtain the lock uses a condition variable to wait for
     * the lock.
     */
    void obtain();

    /**
     * @brief Release the lock.
     */
    void release();

  private:
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    int mWaitingWriters;
    bool mIsLocked;
};

#include <gintonic/nsend.hpp>
