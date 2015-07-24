/*!
\file exception.hpp
\author Raoul Wols
\date 17/09/2014
\brief We employ boost::exception to make our life easier.
*/

#ifndef exception_hpp
#define exception_hpp

#include <stdexcept>
#include <boost/exception/all.hpp>

namespace gintonic {

// struct exception : virtual boost::exception, virtual std::exception {};

// using boost::diagnostic_information;

// Might throw if there is not enough memory.
// In memory-critical situations do not use this exception.
class exception : public std::exception
{
public:
	exception(const std::string& message);
	exception(std::string&& message);
	exception(const char* message);
	exception(const exception& other) = default;
	exception(exception&&) BOOST_NOEXCEPT_OR_NOTHROW = default;
	exception& operator=(const exception&) = default;
	exception& operator=(exception&&) = default;
	virtual ~exception() BOOST_NOEXCEPT_OR_NOTHROW = default;
	virtual const char* what() const BOOST_NOEXCEPT_OR_NOTHROW;
	exception& append(std::string message);
	exception& prepend(std::string message);
private:
	std::string m_message;
};

} // end of namespace gintonic

#endif