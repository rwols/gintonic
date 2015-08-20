#include "box2f.hpp"

namespace gintonic {

box2f::box2f(const vec2f& min_corner, const vec2f& max_corner)
: min_corner(min_corner), max_corner(max_corner)
{
	/* Empty on purpose. */
}

bool box2f::contains(const vec2f& point) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return min_corner < point && point < max_corner;
}

bool box2f::contains(const box2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return min_corner < other.min_corner && other.max_corner < max_corner;
}

bool box2f::intersects(const box2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return contains(other.min_corner) || contains(other.max_corner);
}

} // namespace gintonic