/**
 * @file exception.hpp
 * @brief Defines an exception class that houses an std::string.
 * @author Raoul Wols
 */

#ifndef exception_hpp
#define exception_hpp

#include <stdexcept>
#include <string>
#include "config.hpp"

namespace gintonic {

/**
 * @brief Simple exception class.
 * @details Derives from std::exception. It has
 * simple append and prepend functionality for
 * strings.
 */
class exception : public std::exception
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param message The exception message.
	 */
	exception(const std::string& message);

	/// Move constructor.
	exception(std::string&& message);

	/**
	 * @brief Constructor.
	 * 
	 * @param message The exception message.
	 */
	exception(const char* message);

	/// Copy constructor.
	exception(const exception& other) = default;

	/// Move constructor.
	exception(exception&&) noexcept;

	/// Copy assignment operator.
	exception& operator=(const exception&) = default;

	/// Move assignment operator.
	exception& operator=(exception&&);

	/// Destructor.
	virtual ~exception() noexcept;

	/**
	 * @brief Get the exception message.
	 * @return The exception message.
	 */
	virtual const char* what() const noexcept;

	/**
	 * @brief Append a string to the exception message.
	 * 
	 * @param message The string to append.
	 * @return *this.
	 */
	exception& append(std::string message);

	/**
	 * @brief Prepend a string to the exception message.
	 * 
	 * @param message The string to prepend.
	 * @return *this.
	 */
	exception& prepend(std::string message);
	
private:
	std::string m_message;
};

} // end of namespace gintonic

#endif