#pragma once

#include <gintonic/nsbegin.hpp>

template <class Type, class DurationType = float> struct Interpolator
{
    using type = Type;
    using duration_type = DurationType;

    Type begin;
    Type end;

    Interpolator() = default;

    template <class A, class B>
    Interpolator(A&& a, B&& b)
        : begin(std::forward<A>(a)), end(std::forward<B>(b))
    {
    }

    inline Type operator()(const DurationType& d)
    {
        return (DurationType(1) - d) * begin + d * end;
    }

    inline void operator()(const DurationType& d, Type& out)
    {
        out = operator()(d);
    }
};

#include <gintonic/nsend.hpp>
