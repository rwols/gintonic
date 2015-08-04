#include "vec2.hpp"
#include <iostream>
#include <fbxsdk.h>

namespace gintonic {
	
std::ostream& operator << (std::ostream& os, const vec2& v)
{
	return os << v.x << ' ' << v.y;
}

std::istream& operator >> (std::istream& is, vec2& v)
{
	return is >> v.x >> v.y;
}

} // namespace gintonic