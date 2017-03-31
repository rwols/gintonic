/**
 * @file ReadWriteLock.hpp
 * @brief Defines a read write lock class.
 * @author Raoul Wols
 */

#pragma once

#include <condition_variable>
#include <mutex>

#include <gintonic/nsbegin.hpp>

/**
 * @brief Read write lock.
 *
 * @details A read write lock is a lock where multiple threads that only need
 * read access to the resource can access the lock. If a thread needs to write
 * to the resource, it can obtain a read lock. A writer thread has to wait
 * until all reader threads are done. Once a writer thread has the lock, no
 * reader threads can access the lock.
 *
 * @sa WriteLock
 */
class ReadWriteLock
{
  public:
    /// Default constructor.
    ReadWriteLock();

    /**
     * @brief Obtain a read lock. Multiple threads can get a read lock.
     */
    void obtainRead() const;

    /**
     * @brief Release a read lock.
     */
    void releaseRead() const;

    /**
     * @brief Obtain a write lock.
     */
    void obtainWrite();

    /**
     * @brief Release a write lock.
     */
    void releaseWrite();

  private:
    mutable std::mutex mMutex;
    mutable std::condition_variable mReader;
    mutable std::condition_variable mWriter;
    mutable int mActiveReaders;
    int mWaitingWriters;
    int mActiveWriters;
};

#include <gintonic/nsend.hpp>
