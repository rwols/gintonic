#include "filesystem.hpp"

#ifdef BOOST_MSVC
// INCLUDE RELEVANT HEADER HERE
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif __linux__
// INCLUDE RELEVANT HEADER HERE
#else

#error "Platform not supported."

#endif

namespace gintonic {

	boost::filesystem::path get_executable_path()
	{
		#ifdef BOOST_MSVC

		// TODO

		#elif defined(__APPLE__)
		
		uint32_t bufsize = 0;
		char* buf = nullptr;
		_NSGetExecutablePath(nullptr, &bufsize);
		buf = new char[bufsize];
		auto result = _NSGetExecutablePath(buf, &bufsize);
		if (result == 0)
		{
			boost::filesystem::path exec_path(buf);
			delete [] buf;
			return exec_path.parent_path();
		}
		else
		{
			delete [] buf;
			return boost::filesystem::path(); // empty, silent failure
		}
		
		#elif __linux__

		// TODO

		#else

		#error "Platform not supported."
		
		#endif
	}

} // namespace gintonic