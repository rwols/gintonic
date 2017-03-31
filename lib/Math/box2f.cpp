#include <gintonic/Math/box2f.hpp>

using namespace gintonic;

box2f::box2f() : minCorner(0.0f, 0.0f), maxCorner(0.0f, 0.0f)
{
    GT_PROFILE_FUNCTION;
}

box2f::box2f(const vec2f& minCorner, const vec2f& maxCorner)
    : minCorner(minCorner), maxCorner(maxCorner)
{
    GT_PROFILE_FUNCTION;
}

bool box2f::contains(const vec2f& point) const noexcept
{
    GT_PROFILE_FUNCTION;

    return minCorner <= point && point <= maxCorner;
}

bool box2f::contains(const box2f& other) const noexcept
{
    GT_PROFILE_FUNCTION;

    return minCorner <= other.minCorner && other.maxCorner <= maxCorner;
}

bool intersects(const box2f& a, const box2f& b) noexcept
{
    GT_PROFILE_FUNCTION;

    return a.contains(b.minCorner) || a.contains(b.maxCorner) ||
           b.contains(a.minCorner) || b.contains(a.maxCorner);
}

std::ostream& operator<<(std::ostream& os, const box2f& b)
{
    GT_PROFILE_FUNCTION;

    return os << b.minCorner << ' ' << b.maxCorner;
}
std::istream& operator>>(std::istream& is, box2f& b)
{
    GT_PROFILE_FUNCTION;

    is >> b.minCorner >> b.maxCorner;
    return is;
}
