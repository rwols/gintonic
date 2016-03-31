#include "box2f.hpp"

namespace gintonic {

box2f::box2f()
: minCorner(0.0f, 0.0f)
, maxCorner(0.0f, 0.0f)
{
	/* Empty on purpose. */
}

box2f::box2f(const vec2f& minCorner, const vec2f& maxCorner)
: minCorner(minCorner)
, maxCorner(maxCorner)
{
	/* Empty on purpose. */
}

bool box2f::contains(const vec2f& point) const noexcept
{
	return minCorner <= point && point <= maxCorner;
}

bool box2f::contains(const box2f& other) const noexcept
{
	return minCorner <= other.minCorner && other.maxCorner <= maxCorner;
}

bool intersects(const box2f& a, const box2f& b) noexcept
{
	return a.contains(b.minCorner) || a.contains(b.maxCorner)
		|| b.contains(a.minCorner) || b.contains(a.maxCorner);
}

std::ostream& operator << (std::ostream& os, const box2f& b)
{
	return os << b.minCorner << ' ' << b.maxCorner;
}
std::istream& operator >> (std::istream& is, box2f& b)
{
	is >> b.minCorner >> b.maxCorner;
	return is;
}

} // namespace gintonic