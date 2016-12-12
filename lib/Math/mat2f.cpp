#include "Math/mat2f.hpp"
#include "Math/vec2f.hpp"
#include <array>

namespace gintonic {

mat2f::mat2f(const vec2f& column0, const vec2f& column1)
: data(pack(column0, column1))
{
	GT_PROFILE_FUNCTION;
}
	
mat2f::mat2f(std::initializer_list<float> init)
{
	GT_PROFILE_FUNCTION;

	GINTONIC_ALIGNED(16) float temp[4];
	std::copy(init.begin(), init.end(), temp);
	m00 = temp[0];
	m01 = temp[1];
	m10 = temp[2];
	m11 = temp[3];
}

mat2f& mat2f::operator=(std::initializer_list<float> init)
{
	GT_PROFILE_FUNCTION;

	GINTONIC_ALIGNED(16) float temp[4];
	std::copy(init.begin(), init.end(), temp);
	m00 = temp[0];
	m01 = temp[1];
	m10 = temp[2];
	m11 = temp[3];
	return *this;
}

vec2f operator * (const mat2f& m, const vec2f& v)
{
	GT_PROFILE_FUNCTION;
	
	const auto product = _mm_mul_ps(m.data, pack(v,v));
	return _mm_hadd_ps(product, product);
}

} // namespace gintonic