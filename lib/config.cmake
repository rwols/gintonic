#ifndef gintonic_config_hpp
#define gintonic_config_hpp

#include <boost/config.hpp>

/*****************************************************************************
* DEBUG_PRINT and PRINT_VAR                                                  *
*                                                                            *
* Purpose: Convenience macro to print the current file and line number.      *
* The macro PRINT_VAR will print the variable and its name.                  *
*                                                                            *
* Example: int my_var; PRINT_VAR(myvar); DEBUG_PRINT;                        *
*****************************************************************************/

#ifndef NDEBUG
#ifndef DEBUG_PRINT
#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif
#else
#define DEBUG_PRINT // as nothing
#define PRINT_VAR(var) // as nothing
#endif

/*****************************************************************************
* GINTONIC_SSE_VERSION                                                       *
*                                                                            *
* Purpose: Defines the SSE version number. This way, we can select at        *
* compile-time which SSE instructions we will use.                           *
*****************************************************************************/

#define GINTONIC_SSE_VERSION ${SSE_VERSION}

/*****************************************************************************
* GINTONIC_NAMESPACE_FBX                                                     *
*                                                                            *
* Purpose: Defines the fbx sdk namespace, to enable us to forward-declare    *
* fbx types.                                                                 *
*****************************************************************************/

#ifdef BOOST_MSVC
#define GINTONIC_NAMESPACE_FBX fbxsdk
#else
#define GINTONIC_NAMESPACE_FBX ${FBX_NAMESPACE}
#endif

#cmakedefine ENABLE_DEBUG_TRACE
#cmakedefine WITH_PROFILING
#cmakedefine WITH_MEMORY_PROFILING
#cmakedefine HIDE_CONSOLE
#cmakedefine REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE

#endif