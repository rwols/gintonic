#include "box3f.hpp"

namespace gintonic {

box3f::box3f()
: min_corner(0.0f, 0.0f, 0.0f)
, max_corner(0.0f, 0.0f, 0.0f)
{
	/* Empty on purpose. */
}

box3f::box3f(const vec3f& min_corner, const vec3f& max_corner)
: min_corner(min_corner)
, max_corner(max_corner)
{
	/* Empty on purpose. */
}

bool box3f::contains(const vec3f& point) const noexcept
{
	return min_corner <= point && point <= max_corner;
}

bool box3f::contains(const box3f& other) const noexcept
{
	return min_corner <= other.min_corner && other.max_corner <= max_corner;
}

bool intersects(const box3f& a, const box3f& b) noexcept
{
	return a.contains(b.min_corner) || a.contains(b.max_corner)
		|| b.contains(a.min_corner) || b.contains(a.max_corner);
}

std::ostream& operator << (std::ostream& os, const box3f& b)
{
	return os << b.min_corner << ' ' << b.max_corner;
}
std::istream& operator >> (std::istream& is, box3f& b)
{
	is >> b.min_corner >> b.max_corner;
	return is;
}

} // namespace gintonic