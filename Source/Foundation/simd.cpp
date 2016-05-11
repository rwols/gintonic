#include "simd.hpp"

#include <iostream>

namespace gintonic {

std::ostream& operator << (std::ostream& os, const __m128& v)
{
	alignas(16) float temp[4];
	_mm_store_ps(temp, v);
	return os << '[' << temp[0] << ", " << temp[1] << ", " << temp[2] << ", " << temp[3] << ']';
}

} // namespace gintonic

