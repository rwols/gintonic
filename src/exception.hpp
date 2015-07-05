/*!
\file exception.hpp
\author Raoul Wols
\date 17/09/2014
\brief We employ boost::exception to make our life easier.
*/

#ifndef exception_hpp
#define exception_hpp

#include <stdexcept>
#include <boost/exception/all.hpp>

namespace gintonic {

struct exception : virtual boost::exception, virtual std::exception {};

using boost::diagnostic_information;

} // end of namespace gintonic

#endif