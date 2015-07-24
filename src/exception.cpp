#include "exception.hpp"

namespace gintonic {

exception::exception(const std::string& message) : m_message(message) {}
exception::exception(std::string&& message) : m_message(std::move(message)) {}
exception::exception(const char* message) : m_message(message) {}
const char* exception::what() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return m_message.c_str();
}
exception& exception::append(std::string message)
{
	m_message.append(std::move(message));
	return *this;
}
exception& exception::prepend(std::string message)
{
	m_message.insert(m_message.begin(), message.begin(), message.end());
	return *this;
}

} // namespace gintonic