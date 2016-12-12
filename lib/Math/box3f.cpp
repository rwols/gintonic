#include "Math/box3f.hpp"

#include "Foundation/allocator.hpp"

namespace gintonic {

box3f::box3f()
: minCorner(0.0f, 0.0f, 0.0f)
, maxCorner(0.0f, 0.0f, 0.0f)
{
	GT_PROFILE_FUNCTION;
}

box3f::box3f(const vec3f& minCorner, const vec3f& maxCorner)
: minCorner(minCorner)
, maxCorner(maxCorner)
{
	GT_PROFILE_FUNCTION;
}

bool box3f::contains(const vec3f& point) const noexcept
{
	GT_PROFILE_FUNCTION;

	return minCorner <= point && point <= maxCorner;
}

#define GT_CONTAINS_VERSION 0

bool box3f::contains(const box3f& other) const noexcept
{
	GT_PROFILE_FUNCTION;

	#if GT_CONTAINS_VERSION == 2

	const auto zero = _mm_setzero_ps();
	auto test = _mm_cmple_ps(minCorner.data, other.minCorner.data);
	if (_mm_movemask_ps(_mm_cmpeq_ps(test, zero)) != 0)
	{
		return false;
	}
	test = _mm_cmple_ps(other.maxCorner.data, maxCorner.data);
	return (_mm_movemask_ps(_mm_cmpeq_ps(test, zero)) != 0) ? false : true;

	#elif GT_CONTAINS_VERSION == 1

	auto a = minCorner <= other.minCorner;
	auto b = other.maxCorner <= maxCorner;
	return a && b;

	#else // GT_CONTAINS_VERSION

	return minCorner <= other.minCorner && other.maxCorner <= maxCorner;

	#endif // GT_CONTAINS_VERSION
}

void box3f::addPoint(const vec3f& point) noexcept
{
	GT_PROFILE_FUNCTION;

	minCorner.data = _mm_min_ps(minCorner.data, point.data);
	maxCorner.data = _mm_max_ps(maxCorner.data, point.data);
}

void box3f::addPoint(const vec4f& point) noexcept
{
	GT_PROFILE_FUNCTION;

	minCorner.data = _mm_min_ps(minCorner.data, point.data);
	maxCorner.data = _mm_max_ps(maxCorner.data, point.data);
}

#define GT_INTERSECTS_VERSION 0

bool intersects(const box3f& a, const box3f& b) noexcept
{
	GT_PROFILE_FUNCTION;

	#if GT_INTERSECTS_VERSION == 1

	auto x = a.contains(b.minCorner);
	auto y = a.contains(b.maxCorner);
	auto z = b.contains(a.minCorner);
	auto w = b.contains(a.maxCorner);
	return x || y || z || w;

	#else // GT_INTERSECTS_VERSION

	std::array<vec3f, 8> lCorners;
	a.getCorners(lCorners.begin());
	for (int i = 0; i < 8; ++i)
	{
		if (b.contains(lCorners[i])) return true;
	}
	b.getCorners(lCorners.begin());
	for (int i = 0; i < 8; ++i)
	{
		if (a.contains(lCorners[i])) return true;
	}

	return false;

	#endif // GT_INTERSECTS_VERSION
}

std::ostream& operator << (std::ostream& os, const box3f& b)
{
	GT_PROFILE_FUNCTION;

	return os << b.minCorner << ' ' << b.maxCorner;
}
std::istream& operator >> (std::istream& is, box3f& b)
{
	GT_PROFILE_FUNCTION;
	
	is >> b.minCorner >> b.maxCorner;
	return is;
}

} // namespace gintonic