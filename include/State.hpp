#pragma once

#include <boost/statechart/state.hpp>

namespace gintonic
{

template <class MostDerived, class Context>
using State = boost::statechart::state<MostDerived, Context>;

} // gintonic
