/**
 * @file tuple.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_tuple_hpp
#define gintonic_tuple_hpp

#include <tuple>
#include <utility>
#include <boost/functional/hash.hpp>

namespace std {

/*****************************************************************************
 * hash tuples (somewhat hacky).                                             *
 * I'm not documenting this because hashing tuples is used implicitly.       *
 ****************************************************************************/

//!@cond

template<typename... T> struct hash<tuple<T...>> 
{
	inline size_t operator()(tuple<T...> const& arg) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return boost::hash_value(arg);
	}
};

} // namespace std

// pretty-print a tuple (from http://stackoverflow.com/a/6245777/273767)

namespace gintonic {
namespace detail {

template <std::size_t...> struct sequence {};

template <std::size_t N, std::size_t... Is>
struct generate_sequence : generate_sequence<N-1, N-1, Is...> {};

template <std::size_t... Is>
struct generate_sequence<0, Is...> : sequence<Is...> {};

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple(
	std::basic_ostream<Ch,Tr>& os, 
	const Tuple& t, 
	sequence<Is...>)
{
	using swallow = int[];
	(void)swallow
	{
		0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...
	};
}

} // namespace detail
} // namespace gintonic

/*****************************************************************************
 * operator << for tuples                                                    *
 ****************************************************************************/
template <class Ch, class Tr, class... Args>
std::basic_ostream<Ch, Tr>& operator << (
	std::basic_ostream<Ch, Tr>& os, 
	std::tuple<Args...> const& t)
{
	os << "(";
	gintonic::detail::print_tuple(os, t, 
		gintonic::detail::generate_sequence<sizeof...(Args)>());
	return os << ")";
}

/*****************************************************************************
 * boost::serialization for tuples                                           *
 ****************************************************************************/
namespace boost {
namespace serialization {

template <std::size_t N> struct tuple_serializer
{
	template<class Archive, typename... Args>
	static void serialize(Archive & ar, std::tuple<Args...> & t, 
		const unsigned int version)
	{
		ar & std::get<N - 1>(t);
		tuple_serializer<N - 1>::serialize(ar, t, version);
	}
};

template<> struct tuple_serializer<0>
{
	template<class Archive, typename... Args>
	static void serialize(Archive & ar, std::tuple<Args...> & t, 
		const unsigned int /*version*/)
	{
		/* Empty on purpose */
	}
};

template<class Archive, typename... Args>
void serialize(Archive & ar, std::tuple<Args...> & t, 
	const unsigned int version)
{
	tuple_serializer<sizeof...(Args)>::serialize(ar, t, version);
}

//!@endcond

} // namespace serialization
} // namespace boost

#endif