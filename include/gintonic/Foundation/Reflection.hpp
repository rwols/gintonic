#pragma once

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/preprocessor/library.hpp>
#include <type_traits>

#define REM(...) __VA_ARGS__
#define EAT(...)

// Retrieve the type
#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),

// Strip off the type
#define STRIP(x) EAT x

// Show the type without parenthesis
#define PAIR(x) REM x

#define GT_REFLECTABLE(...) \
static const uint8_t fieldsCount = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
friend struct Reflector; \
template<uint8_t N, class Self> \
struct FieldData {}; \
BOOST_PP_SEQ_FOR_EACH_I(GT_REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define GT_REFLECT_EACH(r, data, i, x) \
PAIR(x); \
template <class Self> \
struct FieldData<i, Self> \
{ \
    Self& self; \
    FieldData(Self& self) : self(self) {} \
    \
    typename MakeConst<Self, TYPEOF(x)>::type& value() \
    { \
        return self.STRIP(x); \
    }\
    typename std::add_const<TYPEOF(x)>::type& value() const \
    { \
        return self.STRIP(x); \
    }\
    const char* name() const \
    {\
        return BOOST_PP_STRINGIZE(STRIP(x)); \
    } \
}; \

// namespace gintonic {

// A helper metafunction for adding const to a type
template<class M, class T>
struct MakeConst
{
    typedef T type;
};

template<class M, class T>
struct MakeConst<const M, T>
{
    typedef typename std::add_const<T>::type type;
};

struct Reflector
{
    //Get field_data at index N
    template<uint8_t N, class T>
    static typename T::template FieldData<N, T> getFieldData(T& x)
    {
        return typename T::template FieldData<N, T>(x);
    }

    // Get the number of fields
    template<class T>
    struct Fields
    {
        static const uint8_t count = T::fieldsCount;
    };
};

struct FieldVisitor
{
    template<class C, class Visitor, class I>
    void operator()(C& c, Visitor v, I)
    {
        v(Reflector::getFieldData<I::value>(c));
    }
};


template<class C, class Visitor>
void visitEach(C& c, Visitor v)
{
    using Range = boost::mpl::range_c<uint8_t,0,Reflector::Fields<C>::count>;
    boost::mpl::for_each<Range>(boost::bind<void>(FieldVisitor(), boost::ref(c), v, _1));
}

// } // namespace gintonic