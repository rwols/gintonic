#ifndef gintonic_vec2_hpp
#define gintonic_vec2_hpp

#include <iosfwd>
#include "simd.hpp"

namespace gintonic {

union vec2
{
	struct
	{
		float x;
		float y;
	};
	__m128 data;

	inline vec2() = default;
	inline vec2(const float x, const float y) : x(x), y(y) {}
	inline vec2(const __m128& data) : data(data) {}
};

inline vec2 operator + (const vec2& lhs, const vec2& rhs)
BOOST_NOEXCEPT_OR_NOTHROW
{
	return _mm_add_ps(lhs.data, rhs.data);
}

inline vec2 operator - (const vec2& lhs, const vec2& rhs)
BOOST_NOEXCEPT_OR_NOTHROW
{
	return _mm_sub_ps(lhs.data, rhs.data);
}

std::ostream& operator << (std::ostream&, const vec2&);
std::istream& operator >> (std::istream&, vec2&);

} // namespace gintonic

#endif