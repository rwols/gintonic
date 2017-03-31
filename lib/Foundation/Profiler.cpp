#include <gintonic/Foundation/Profiler.hpp>
#include <fstream>
#include <thread>

using namespace gintonic;
using namespace gintonic::detail;

Profiler::~Profiler() noexcept
{
    try
    {
        const auto lEndTime = std::chrono::high_resolution_clock::now();
        sGlobalLogMap[mScopeName].push_front(lEndTime - mStartTime);
    }
    catch (...)
    {
        // do nothing
    }
}
void Profiler::writeLogToFile(const char* logFile)
{
    if (sGlobalLogMap.empty()) return;

    std::cerr << "Writing profiling information to " << logFile << " ...\n";
    std::cerr << "Please wait ... ";

    std::ofstream lOutput(logFile);
    std::chrono::nanoseconds lAverage;

    // Write the headers.
    lOutput << "Function signature,Number of Calls,Average Nanoseconds per "
               "Call,Average Total Seconds\n";

    // Write the data.
    for (const auto& p : sGlobalLogMap)
    {
        if (p.second.empty()) continue;
        lAverage = std::chrono::nanoseconds::zero();
        std::size_t lCount{0};
        for (const auto& lRecordedDuration : p.second)
        {
            lAverage += lRecordedDuration;
            ++lCount;
        }
        lAverage /= lCount;
        lOutput << '\"' << p.first << "\"," << lCount << "," << lAverage.count()
                << ", "
                << static_cast<double>(
                       lCount * static_cast<std::size_t>(lAverage.count())) /
                       1e9
                << '\n';
    }

    std::cerr << "Done!\n";
}

std::map<std::string,
         std::forward_list<std::chrono::high_resolution_clock::duration>>
    Profiler::sGlobalLogMap = std::map<
        std::string,
        std::forward_list<std::chrono::high_resolution_clock::duration>>();
