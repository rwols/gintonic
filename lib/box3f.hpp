#ifndef gintonic_box3f_hpp
#define gintonic_box3f_hpp

#include "vec3f.hpp"

namespace gintonic {

struct box3f
{
	vec3f min_corner;
	vec3f max_corner;

	box3f();
	box3f(const vec3f& min_corner, const vec3f& max_corner);

	bool contains(const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	bool contains(const box3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

bool intersects(const box3f& a, const box3f& b) BOOST_NOEXCEPT_OR_NOTHROW;

std::ostream& operator << (std::ostream&, const box3f&);
std::istream& operator >> (std::istream&, box3f&);

} // namespace gintonic

#endif