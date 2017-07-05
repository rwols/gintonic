#pragma once

#include <boost/intrusive_ptr.hpp>

namespace gintonic {

template <class T> using IntrusivePtr = boost::intrusive_ptr<T>;

} // gintonic
