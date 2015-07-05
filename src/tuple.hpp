#ifndef tuple_hpp
#define tuple_hpp

#include <tuple>

namespace boost {
	namespace serialization {

		template <std::size_t N> struct tuple_serializer
		{
			template<class Archive, typename... Args>
			static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
			{
				ar & std::get<N - 1>(t);
				tuple_serializer<N - 1>::serialize(ar, t, version);
			}
		};

		template<> struct tuple_serializer<0>
		{
			template<class Archive, typename... Args>
			static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
			{
			}
		};

		template<class Archive, typename... Args>
		void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
		{
			tuple_serializer<sizeof...(Args)>::serialize(ar, t, version);
		}

	}
}

#endif