#include "locks.hpp"

namespace gintonic
{

write_lock::write_lock()
: m_mut()
, m_cond()
, m_waiting_writers(0)
, m_is_locked(false)
{
	/* Empty on purpose. */
}

void write_lock::obtain()
{
	std::unique_lock<std::mutex> lock(m_mut);
	++m_waiting_writers;
	while (m_is_locked) m_cond.wait(lock);
	m_is_locked = true;
	lock.unlock();
}

void write_lock::release()
{
	std::unique_lock<std::mutex> lock(m_mut);
	--m_waiting_writers;
	m_is_locked = false;
	if (m_waiting_writers > 0) m_cond.notify_one();
	lock.unlock();
}

read_write_lock::read_write_lock()
: m_mut()
, m_reader(), m_writer()
, m_active_readers(0), m_waiting_writers(0), m_active_writers(0)
{
	/* Empty on purpose. */
}

void read_write_lock::obtain_read() const
{
	std::unique_lock<std::mutex> lock(m_mut);
	while (m_waiting_writers != 0) m_reader.wait(lock);
	++m_active_readers;
	lock.unlock();
}

void read_write_lock::release_read() const
{
	std::unique_lock<std::mutex> lock(m_mut);
	--m_active_readers;
	lock.unlock();
	m_writer.notify_one();
}

void read_write_lock::obtain_write() 
{
	std::unique_lock<std::mutex> lock(m_mut);
	++m_waiting_writers;
	while (m_active_readers != 0 || m_active_writers != 0) m_writer.wait(lock);
	++m_active_writers;
	lock.unlock();
}

void read_write_lock::release_write() 
{
	std::unique_lock<std::mutex> lock(m_mut);
	--m_waiting_writers;
	--m_active_writers;
	if (m_waiting_writers > 0) m_writer.notify_one();
	else m_reader.notify_all();
	lock.unlock();
}

} // namespace gintonic 

