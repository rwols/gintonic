#pragma once

#include <boost/statechart/event.hpp>

namespace gintonic
{

template <class MostDerived, class Allocator = std::allocator<void>>
using StateEvent = boost::statechart::event<MostDerived, Allocator>;

struct EvStartStop : StateEvent<EvStartStop>
{
};
struct EvReset : StateEvent<EvReset>
{
};
struct EvUpdate : StateEvent<EvUpdate>
{
};
struct EvContinue : StateEvent<EvContinue>
{
};
struct EvNewGame : StateEvent<EvNewGame>
{
};
struct EvOptions : StateEvent<EvOptions>
{
};

} // gintonic
