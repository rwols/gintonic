#include "Foundation/exception.hpp"
#include <string>

namespace gintonic {

exception::exception(const std::string& message) : m_message(message) {}
exception::exception(std::string&& message) : m_message(std::move(message)) {}
exception::exception(const char* message) : m_message(message) {}

exception::exception(exception&& other) noexcept
: m_message(std::move(other.m_message))
{
	// When MSVC supports defaulted move constructors / destructors,
	// remove me.
}


exception& exception::operator=(exception&& other)
{
	// When MSVC supports defaulted move constructors / destructors,
	// remove me.
	m_message = std::move(other.m_message);
	return *this;
}

exception::~exception() noexcept
{
	// When MSVC supports defaulted move constructors / destructors,
	// remove me.
}

const char* exception::what() const noexcept
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