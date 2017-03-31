/**
 * @file Profiler.hpp
 * @brief Defines a profiler class and useful macros to use the profiler
 * functionality.
 * @author Raoul Wols
 */

#pragma once

#include "config.hpp"
#include <boost/config.hpp>
#include <chrono>
#include <forward_list>
#include <iostream>
#include <map>
#include <memory>
#include <string>

/**
 * @brief The file to save the CPU profiling log to.
 */
#ifndef GT_PROFILING_LOG_FILE
#define GT_PROFILING_LOG_FILE "ProfilingResults.csv"
#endif

/**
 * @brief The file to save the memory profiling log to.
 */
#ifndef GT_PROFILING_MEM_LOG_FILE
#define GT_PROFILING_MEM_LOG_FILE "MemProfilingResults.csv"
#endif

#ifdef WITH_PROFILING
/**
 * @brief Put this macro at the beginning of a function to profile it.
 *
 * @param scope_name A string literal that should be equal to the name
 * of the function or method.
 */
#define GT_PROFILE_FUNCTION                                                    \
    ::gintonic::detail::Profiler __dont_touch_me__(GINTONIC_FUNC_SIGNATURE);
#else
#define GT_PROFILE_FUNCTION
#endif

#ifdef WITH_PROFILING
/**
 * @brief Put this macro at the end of the program.
 * @details This macro will write the profiling results of all recorded
 * functions and methods to a file.
 */
#define GT_FINALIZE_PROFILING                                                  \
    ::gintonic::detail::Profiler::writeLogToFile(GT_PROFILING_LOG_FILE);
#else
#define GT_FINALIZE_PROFILING
#endif

#include <gintonic/nsbegin.hpp>

namespace detail
{

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
 * use the Profiler class directly but instead use the macros.
 */
class Profiler
{

  private:
    static std::map<
        std::string,
        std::forward_list<std::chrono::high_resolution_clock::duration>>
        sGlobalLogMap;

    const std::string mScopeName;

    const std::chrono::high_resolution_clock::time_point mStartTime;

  public:
    /**
     * @brief Constructor that takes the name of a scope.
     * @tparam T The string type, usually just a const char*.
     * @param scopeName The name of the scope.
     */
    template <class T>
    Profiler(T&& scopeName)
        : mScopeName(std::forward<T>(scopeName)),
          mStartTime(std::chrono::high_resolution_clock::now())
    {
    }

    /**
     * @brief The destructor saves the timing of the scope to a global map.
     */
    ~Profiler() noexcept;

    /**
     * @brief Write the timing results as a CSV file.
     * @param logFile The filename to write to.
     */
    static void writeLogToFile(const char* logFile);
};

} // end of namespace detail

#include <gintonic/nsend.hpp>
