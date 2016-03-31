#include "box3f.hpp"

namespace gintonic {

box3f::box3f()
: minCorner(0.0f, 0.0f, 0.0f)
, maxCorner(0.0f, 0.0f, 0.0f)
{
	/* Empty on purpose. */
}

box3f::box3f(const vec3f& minCorner, const vec3f& maxCorner)
: minCorner(minCorner)
, maxCorner(maxCorner)
{
	/* Empty on purpose. */
}

bool box3f::contains(const vec3f& point) const noexcept
{
	return minCorner <= point && point <= maxCorner;
}

bool box3f::contains(const box3f& other) const noexcept
{
	return minCorner <= other.minCorner && other.maxCorner <= maxCorner;
}

void box3f::addPoint(const vec3f& point) noexcept
{
	minCorner.data = _mm_min_ps(minCorner.data, point.data);
	maxCorner.data = _mm_max_ps(maxCorner.data, point.data);
}

void box3f::addPoint(const vec4f& point) noexcept
{
	minCorner.data = _mm_min_ps(minCorner.data, point.data);
	maxCorner.data = _mm_max_ps(maxCorner.data, point.data);
}

bool intersects(const box3f& a, const box3f& b) noexcept
{
	return a.contains(b.minCorner) || a.contains(b.maxCorner)
		|| b.contains(a.minCorner) || b.contains(a.maxCorner);
}

std::ostream& operator << (std::ostream& os, const box3f& b)
{
	return os << b.minCorner << ' ' << b.maxCorner;
}
std::istream& operator >> (std::istream& is, box3f& b)
{
	is >> b.minCorner >> b.maxCorner;
	return is;
}

} // namespace gintonic