#ifndef gintonic_box2f_hpp
#define gintonic_box2f_hpp

#include "vec2f.hpp"

namespace gintonic {

struct box2f
{
	vec2f min_corner;
	vec2f max_corner;

	box2f();
	box2f(const vec2f& min_corner, const vec2f& max_corner);

	bool contains(const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	bool contains(const box2f&) const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

bool intersects(const box2f& a, const box2f& b) BOOST_NOEXCEPT_OR_NOTHROW;

std::ostream& operator << (std::ostream&, const box2f&);
std::istream& operator >> (std::istream&, box2f&);

} // namespace gintonic

#endif