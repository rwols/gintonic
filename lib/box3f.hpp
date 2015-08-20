#ifndef gintonic_box3f_hpp
#define gintonic_box3f_hpp

#include "vec3f.hpp"

namespace gintonic {

struct box3f
{
	vec3f min_corner;
	vec3f max_corner;

	box3f() = default;
	box3f(const vec3f& min_corner, const vec3f& max_corner);

	bool contains(const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	bool contains(const box3f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	bool intersects(const box3f&) const BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif