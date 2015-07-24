#ifndef gintonic_locks_hpp
#define gintonic_locks_hpp

#include <mutex>
#include <condition_variable>

namespace gintonic {

// Unfortunately, std::condition_variable
// can throw all sorts of exceptions, so
// there's no no-exception guarantee of any sort.

class write_lock
{
public:
	write_lock();

	void obtain();

	void release();

private:
	std::mutex m_mut;
	std::condition_variable m_cond;
	int m_waiting_writers;
	bool m_is_locked;
};

class read_write_lock
{
public:

	read_write_lock();

	void obtain_read();

	void release_read();

	void obtain_write();

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