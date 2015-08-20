#include "mat3f.hpp"
#include "vec3f.hpp"

namespace gintonic {

vec3f mat3f::operator * (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto m0 = _mm_mul_ps(data[0], v.data);
	const auto m1 = _mm_mul_ps(data[1], v.data);
	const auto m2 = _mm_mul_ps(data[2], v.data);
	return _mm_add_ps(_mm_add_ps(m0, m1), m2);
}

mat3f mat3f::operator * (const mat3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	mat3f r;
	__m128 m0, m1, m2;

	m0 = _mm_mul_ps(data[0], other.data[0]);
	m1 = _mm_mul_ps(data[1], other.data[0]);
	m2 = _mm_mul_ps(data[2], other.data[0]);
	r.data[0] = _mm_add_ps(_mm_add_ps(m0, m1), m2);

	m0 = _mm_mul_ps(data[0], other.data[1]);
	m1 = _mm_mul_ps(data[1], other.data[1]);
	m2 = _mm_mul_ps(data[2], other.data[1]);
	r.data[1] = _mm_add_ps(_mm_add_ps(m0, m1), m2);

	m0 = _mm_mul_ps(data[0], other.data[2]);
	m1 = _mm_mul_ps(data[1], other.data[2]);
	m2 = _mm_mul_ps(data[2], other.data[2]);
	r.data[2] = _mm_add_ps(_mm_add_ps(m0, m1), m2);

	return r;
}

mat3f& mat3f::invert() BOOST_NOEXCEPT_OR_NOTHROW
{
	const float A =   m11 * m22 - m12 * m21 ;
	const float B = -(m10 * m22 - m12 * m20);
	const float C =   m10 * m21 - m11 * m20 ;
	const float D = -(m01 * m22 - m02 * m21);
	const float E =   m00 * m22 - m02 * m20 ;
	const float F = -(m00 * m21 - m01 * m20);
	const float G =   m01 * m12 - m02 * m11 ;
	const float H = -(m00 * m12 - m02 * m10);
	const float K =   m00 * m11 - m01 * m10 ;

	// d is the inverse of the determinant.
	const float d = 1.0f / (m00 * A + m01 * B + m02 * C);

	m00 = d * A;
	m01 = d * D;
	m02 = d * G;
	m10 = d * B;
	m11 = d * E;
	m12 = d * H;
	m20 = d * C;
	m21 = d * F;
	m22 = d * K;
	return *this;
}

mat3f& mat3f::transpose() BOOST_NOEXCEPT_OR_NOTHROW
{
	std::swap(m01, m10);
	std::swap(m02, m20);
	std::swap(m12, m21);
	return *this;
}

} // namespace gintonic

