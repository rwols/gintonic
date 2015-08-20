#include "mat2f.hpp"
#include "vec2f.hpp"

namespace gintonic {

mat2f::mat2f(const vec2f& column0, const vec2f& column1)
: data(pack(column0, column1))
{
	/* Empty on purpose. */
}

vec2f operator * (const mat2f& m, const vec2f& v)
{
	const auto product = _mm_mul_ps(m.data, pack(v,v));
	return _mm_hadd_ps(product, product);
}

} // namespace gintonic