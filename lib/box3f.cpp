#include "box3f.hpp"

namespace gintonic {

box3f::box3f(const vec3f& min_corner, const vec3f& max_corner)
: min_corner(min_corner), max_corner(max_corner)
{
	/* Empty on purpose. */
}

bool box3f::contains(const vec3f& point) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return min_corner < point && point < max_corner;
}

bool box3f::contains(const box3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return min_corner < other.min_corner && other.max_corner < max_corner;
}

bool box3f::intersects(const box3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return contains(other.min_corner) || contains(other.max_corner);
}

} // namespace gintonic