/**
 * @file locks.hpp
 * @brief Defines various read and/or write locks.
 * @author Raoul Wols
 */

#ifndef gintonic_locks_hpp
#define gintonic_locks_hpp

#include <mutex>
#include <condition_variable>

namespace gintonic {

// Unfortunately, std::condition_variable
// can throw all sorts of exceptions, so
// there's no no-exception guarantee of any sort.

/**
 * @brief Write lock.
 *
 * @details A write lock is a simple lock that any thread can obtain. Once a 
 * thread has obtained the lock, no other threads can access the lock. There
 * is no possibility to have multiple readers. For that, use the
 * read_write_lock.
 *
 * @sa read_write_lock
 */
class write_lock
{
public:

	/// Default constructor.
	write_lock();

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
	std::mutex m_mut;
	std::condition_variable m_cond;
	int m_waiting_writers;
	bool m_is_locked;
};

/**
 * @brief Read write lock.
 *
 * @details A read write lock is a lock where multiple threads that only need
 * read access to the resource can access the lock. If a thread needs to write
 * to the resource, it can obtain a read lock. A writer thread has to wait
 * until all reader threads are done. Once a writer thread has the lock, no
 * reader threads can access the lock.
 *
 * @sa write_lock
 */
class read_write_lock
{
public:

	/// Default constructor.
	read_write_lock();

	/**
	 * @brief Obtain a read lock. Multiple threads can get a read lock.
	 */
	void obtain_read();

	/**
	 * @brief Release a read lock.
	 */
	void release_read();

	/**
	 * @brief Obtain a write lock.
	 */
	void obtain_write();

	/**
	 * @brief Release a write lock.
	 */
	void release_write();

private:
	std::mutex m_mut;
	std::condition_variable m_reader;
	std::condition_variable m_writer;
	int m_active_readers;
	int m_waiting_writers;
	int m_active_writers;
};

} // namespace gintonic

#endif