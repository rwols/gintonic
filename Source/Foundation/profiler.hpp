/**
 * @file profiler.hpp
 * @brief Defines a profiler class and useful macros to use the profiler
 * functionality.
 * @author Raoul Wols
 */

#ifndef profiler_hpp
#define profiler_hpp

#include <chrono>
#include <map>
#include <string>
#include <forward_list>
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
/**
 * @brief Put this macro at the beginning of a function to profile it.
 * 
 * @param scope_name A string literal that should be equal to the name
 * of the function or method.
 */
#define PROFILE_FUNCTION \
::gintonic::detail::profiler __dont_touch_me__(GINTONIC_FUNC_SIGNATURE);
#else
#define PROFILE_FUNCTION
#endif

#ifdef WITH_PROFILING

/**
 * @brief Put this macro at the end of the program.
 * @details This macro will write the profiling results of all recorded
 * functions and methods to a file.
 */
#define FINALIZE_PROFILING(filename) \
::gintonic::detail::profiler::write_log(filename);
#else
#define FINALIZE_PROFILING(filename)
#endif

namespace gintonic {
namespace detail {

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
class profiler
{

private:
	
	static std::map
	<
		std::string, 
		std::forward_list
		<
			std::chrono::high_resolution_clock::duration
		>
	> s_log;

	const std::string m_scope_name;

	const std::chrono::high_resolution_clock::time_point m_t1;

public:
	
	/**
	 * @brief Constructor that takes the name of a scope.
	 * @tparam T The string type, usually just a const char*.
	 * @param scope_name The name of the scope.
	 */
	template <class T> profiler(T&& scope_name)
	: m_scope_name(std::forward<T>(scope_name))
	, m_t1(std::chrono::high_resolution_clock::now())
	{}

	/**
	 * @brief The destructor saves the timing of the scope to a global map.
	 */
	~profiler() noexcept;

	/**
	 * @brief Write the timing results as a CSV file.
	 * @param log_file The filename to write to.
	 */
	static void write_log(const char* log_file);
};

} // end of namespace detail
} // end of namespace gintonic

#endif
