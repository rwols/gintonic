#pragma once

#include <boost/signals2/signal.hpp>
#include <functional>
#include <mutex>

namespace gintonic
{

template <
    typename Signature,
    typename Combiner = boost::signals2::optional_last_value<
        typename boost::function_traits<Signature>::result_type>,
    typename Group = int, typename GroupCompare = std::less<Group>,
    typename SlotFunction = std::function<Signature>,
    typename ExtendedSlotFunction =
        typename boost::signals2::detail::extended_signature<
            boost::function_traits<Signature>::arity, Signature>::function_type,
    typename Mutex = std::mutex>
using Signal =
    boost::signals2::signal<Signature, Combiner, Group, GroupCompare,
                            SlotFunction, ExtendedSlotFunction, Mutex>;

} // gintonic
