#ifndef profiler_hpp
#define profiler_hpp

#include <chrono>
#include <map>
#include <string>
#include <list>
#include <memory>
#include <iostream>
#include <boost/config.hpp>

// Uncomment the next line to generate benchmark 
// profiles of various math functions.

// #define WITH_PROFILING

// #include "config.hpp"

#define PROFILER_CPU_LOG_FILE "cpu_profile.csv"
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
	
	template <class T> ___profiler___(T&& scope_name)
	: m_scope_name(std::forward<T>(scope_name))
	, m_t1(std::chrono::high_resolution_clock::now())
	{}
	~___profiler___();
	static void write_log(const char* log_file);
};

} // end of namespace gintonic

#endif
