/**
 * @file profiler.hpp
 * @author Raoul Wols
 */

#ifndef profiler_hpp
#define profiler_hpp

#include <chrono>
#include <map>
#include <string>
#include <list>
#include <memory>
#include <iostream>
#include <boost/config.hpp>
#include "config.hpp"

/**
 * @brief The file to save the CPU profiling log to.
 */
#define PROFILER_CPU_LOG_FILE "cpu_profile.csv"

/**
 * @brief The file to save the memory profiling log to.
 */
#define PROFILER_MEM_LOG_FILE "mem_profile.csv"

#ifdef WITH_PROFILING
#define PROFILE(scope_name) ___profiler___ ___dont_touch_me___(scope_name);
#else
#define PROFILE(scope_name)
#endif

#ifdef WITH_PROFILING
#define END_PROFILE \
___profiler___::write_log(PROFILER_CPU_LOG_FILE);
#else
#define END_PROFILE
#endif

namespace gintonic {

/**
 * @brief A profiler class.
 * @details The idea is as follows. At the start of a function, you write the
 * macro PROFILE("the_name_of_your_method_or_function"). This creates an
 * instance of this class. The destructor then saves the timing results of
 * the function into a global map. At the end of the program, you write the
 * macro END_PROFILE. This will write the global map to the predefined
 * filenames (PROFILER_CPU_LOG_FILE and PROFILER_MEM_LOG_FILE). If the macro
 * WITH_PROFILING is not defined (in the config.cmake file), then the macros
 * PROFILE(scope_name) and END_PROFILE will do nothing at all. You should not
 * use the profiler class directly but instead use the macros.
 */
class ___profiler___
{

private:
	
	static std::map
	<
		std::string, 
		std::list<std::chrono::high_resolution_clock::duration>
	> s_log;

	const std::string m_scope_name;

	const std::chrono::high_resolution_clock::time_point m_t1;

public:
	
	/**
	 * @brief Constructor that takes the name of a scope.
	 * @tparam T The string type, usually just a const char*.
	 * @param scope_name The name of the scope.
	 */
	template <class T> ___profiler___(T&& scope_name)
	: m_scope_name(std::forward<T>(scope_name))
	, m_t1(std::chrono::high_resolution_clock::now())
	{}

	/**
	 * @brief The destructor saves the timing of the scope to a global map.
	 */
	~___profiler___();

	/**
	 * @brief Write the timing results as a CSV file.
	 * @param log_file The filename to write to.
	 */
	static void write_log(const char* log_file);
};

} // end of namespace gintonic

#endif
