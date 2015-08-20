#ifndef gintonic_box2f_hpp
#define gintonic_box2f_hpp

#include "vec2f.hpp"

namespace gintonic {

struct box2f
{
	vec2f min_corner;
	vec2f max_corner;

	box2f() = default;
	box2f(const vec2f& min_corner, const vec2f& max_corner);

	bool contains(const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	bool contains(const box2f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	bool intersects(const box2f&) const BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif