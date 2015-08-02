
/* This file is auto-generated from CMake
 * Please do not edit this by hand. */

#ifndef gintonic_config_hpp
#define gintonic_config_hpp

#ifndef DEBUG_PRINT
#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif

#cmakedefine ENABLE_DEBUG_TRACE
#cmakedefine WITH_PROFILING
#cmakedefine WITH_MEMORY_PROFILING
#cmakedefine HIDE_CONSOLE
#cmakedefine REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE

#endif