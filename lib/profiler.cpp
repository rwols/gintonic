#include "profiler.hpp"
#include <fstream>
#include <thread>

namespace gintonic {
namespace detail {

profiler::~profiler() noexcept
{
	try
	{
		const auto t2 = std::chrono::high_resolution_clock::now();
		s_log[m_scope_name].push_front(t2 - m_t1);
	}
	catch (...)
	{
		// do nothing
	}
}
void profiler::write_log(const char* log_file)
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using std::ofstream;
	if (s_log.empty()) return;
	ofstream output(log_file);
	nanoseconds average;
	for (const auto& p : s_log)
	{
		if (p.second.empty()) continue;
		average = nanoseconds::zero();
		std::size_t size = 0;
		for (const auto& d : p.second)
		{
			average += d;
			++size;
		}
		average /= size;
		output 
			<< '\"' << p.first << "\"," << size 
			<< "," << average.count() << '\n';
	}
}

std::map<std::string, std::forward_list<std::chrono::high_resolution_clock::duration>> 
profiler::s_log = 
std::map<std::string, std::forward_list<std::chrono::high_resolution_clock::duration>>();

} // end of namespace detail
} // end of namespace gintonic
