#include "box2f.hpp"

namespace gintonic {

box2f::box2f()
: min_corner(0.0f, 0.0f)
, max_corner(0.0f, 0.0f)
{
	/* Empty on purpose. */
}

box2f::box2f(const vec2f& min_corner, const vec2f& max_corner)
: min_corner(min_corner)
, max_corner(max_corner)
{
	/* Empty on purpose. */
}

bool box2f::contains(const vec2f& point) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return min_corner <= point && point <= max_corner;
}

bool box2f::contains(const box2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return min_corner <= other.min_corner && other.max_corner <= max_corner;
}

bool intersects(const box2f& a, const box2f& b) BOOST_NOEXCEPT_OR_NOTHROW
{
	return a.contains(b.min_corner) || a.contains(b.max_corner)
		|| b.contains(a.min_corner) || b.contains(a.max_corner);
}

std::ostream& operator << (std::ostream& os, const box2f& b)
{
	return os << b.min_corner << ' ' << b.max_corner;
}
std::istream& operator >> (std::istream& is, box2f& b)
{
	is >> b.min_corner >> b.max_corner;
	return is;
}

} // namespace gintonic