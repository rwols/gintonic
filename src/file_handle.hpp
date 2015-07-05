/*!
\file file_handle.hpp
\author Raoul Wols
\date 17/09/2014
\brief Various classes to handle files and I/O to files.
*/

#ifndef file_handle_hpp
#define file_handle_hpp

#include <string>
#include <fstream>
#include <stdexcept>

namespace gintonic {

/*!
\brief This class is meant to be inherited from.

To uniformize the notion of a class representing a filename, it is
recommended to simply inherit from this class to give your class a
member function called basic_filename_handle::filename(). The constructor
must take a string representing the filename.
*/
template <class StringClass> class basic_filename_handle
{
public:
	basic_filename_handle(const StringClass& filename) : m_filename(filename) {}
	basic_filename_handle(StringClass&& filename) : m_filename(std::move(filename)) {}
	virtual ~basic_filename_handle() {}
	template <class T> basic_filename_handle(basic_filename_handle<T>&& other)
	: m_filename(std::forward<T>(other.m_filename)) {}
	template <class T> basic_filename_handle& operator = (basic_filename_handle<T>&& other)
	{
		m_filename = std::forward<T>(other.m_filename);
		return *this;
	}
	inline const StringClass& filename() const BOOST_NOEXCEPT_OR_NOTHROW { return m_filename; }
private:
	StringClass m_filename;
};

typedef basic_filename_handle<std::string> filename_handle;
typedef basic_filename_handle<std::wstring> wfilename_handle;

/*!
\brief A file handle to open streams.

\deprecated Do not use this class. Simply use std::ifstream and std::ofstream.
*/
template <class Stream> class basic_file_handle
{
private:
	Stream m_stream;
public:

	/*!
	\brief Basic exception class for basic_file_handle.

	\deprecated Do not use this class. Simply use std::ifstream and std::ofstream.
	*/
	template <class StringClass> class error : public std::exception
	{
	private:
		StringClass m_filename;
		std::ios::openmode m_mode;
	public:
		template <class T> error(T&& filename, const std::ios_base::openmode mode) 
		: m_filename(std::forward<T>(filename))
		, m_mode(mode)
		{
			// nothing
		}
		virtual ~error() BOOST_NOEXCEPT_OR_NOTHROW {}
		inline virtual const char* what() const BOOST_NOEXCEPT_OR_NOTHROW
		{
			std::string message("Could not open file \"");
			message.append(m_filename);
			message.append("\".");
			return message.c_str();
		}
		inline StringClass filename() const BOOST_NOEXCEPT_OR_NOTHROW { return m_filename.c_str(); }
		inline bool mode() const BOOST_NOEXCEPT_OR_NOTHROW { return m_mode; }
	};

	basic_file_handle(const char* filename, const std::ios_base::openmode mode = 0)
	: m_stream(filename, mode)
	{
		if (!m_stream) throw error<std::string>(std::string(filename), mode);
	}

	template <class StringClass>
	basic_file_handle(const StringClass& filename, const std::ios_base::openmode mode = 0)
	: m_stream(filename, mode)
	{
		if (!m_stream) throw error<StringClass>(filename, mode);
	}

	template <class T> basic_file_handle(basic_file_handle<T>&& other)
	: m_stream(std::forward<T>(other.m_stream))
	{
		// nothing
	}

	template <class T> basic_file_handle& operator = (basic_file_handle<T>&& other)
	{
		m_stream = std::forward(other.m_stream);
		return *this;
	}

	~basic_file_handle() BOOST_NOEXCEPT_OR_NOTHROW { m_stream.close(); }

	inline Stream& get_stream() BOOST_NOEXCEPT_OR_NOTHROW { return m_stream; }
	inline operator Stream() BOOST_NOEXCEPT_OR_NOTHROW { return get_stream(); }
};

typedef basic_file_handle<std::ifstream> input_file_handle;
typedef basic_file_handle<std::ofstream> output_file_handle;
typedef basic_file_handle<std::fstream>  file_handle;

} // end of namespace gintonic

#endif