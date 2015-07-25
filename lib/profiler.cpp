#include "profiler.hpp"
#include <fstream>
#include <thread>

namespace gintonic {

___profiler___::~___profiler___()
{
	const auto t2 = std::chrono::high_resolution_clock::now();
	s_log[m_scope_name].push_back(t2 - m_t1);
}
void ___profiler___::write_log(const char* log_file)
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
		for (const auto& d : p.second) average += d;
		average /= p.second.size();
		output 
			<< '\"' << p.first << "\"," << p.second.size() 
			<< "," << average.count() << '\n';
	}
}

std::map<std::string, std::list<std::chrono::high_resolution_clock::duration>> 
___profiler___::s_log = 
std::map<std::string, std::list<std::chrono::high_resolution_clock::duration>>();

} // end of namespace gintonic
