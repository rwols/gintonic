#include "mat3f.hpp"
#include "vec3f.hpp"

namespace gintonic {

mat3f::mat3f(const vec3f& column0, const vec3f& column1, const vec3f& column2)
: data{column0.x, column0.y, column0.z, 
	column1.x, column1.y, column1.z, 
	column2.x, column2.y, column2.z}
{
	/* Empty on purpose. */
}

vec3f mat3f::operator * (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return vec3f(
		m00 * v.x + m01 * v.y + m02 * v.z,
		m10 * v.x + m11 * v.y + m12 * v.z,
		m20 * v.x + m21 * v.y + m22 * v.z
	);
	// const auto temp0 = _mm_load_ps(data);
	// const auto temp1 = _mm_set_ps(data[3], data[4], data[5], 0.0f);
	// const auto temp2 = _mm_set_ps(data[6], data[7], data[8], 0.0f);

	// const auto m0 = _mm_mul_ps(temp0, _mm_replicate_x_ps(v.data));
	// const auto m1 = _mm_mul_ps(temp1, _mm_replicate_y_ps(v.data));
	// const auto m2 = _mm_mul_ps(temp2, _mm_replicate_z_ps(v.data));
	// return _mm_add_ps(_mm_add_ps(m0, m1), m2);
}

mat3f mat3f::operator * (const mat3f& b) const BOOST_NOEXCEPT_OR_NOTHROW
{
	mat3f r;
	// __m128 m0, m1, m2;

	r.m00 = m00 * b.m00 + m01 * b.m10 + m02 * b.m20;
	r.m10 = m10 * b.m00 + m11 * b.m10 + m12 * b.m20;
	r.m20 = m20 * b.m00 + m21 * b.m10 + m22 * b.m20;

	r.m00 = m00 * b.m01 + m01 * b.m11 + m02 * b.m21;
	r.m10 = m10 * b.m01 + m11 * b.m11 + m12 * b.m21;
	r.m20 = m20 * b.m01 + m21 * b.m11 + m22 * b.m21;

	r.m00 = m00 * b.m02 + m01 * b.m12 + m02 * b.m22;
	r.m10 = m10 * b.m02 + m11 * b.m12 + m12 * b.m22;
	r.m20 = m20 * b.m02 + m21 * b.m12 + m22 * b.m22;

	// m0 = _mm_mul_ps(_mm_load_ps(data+0), _mm_replicate_x_ps(b.data[0]));
	// m1 = _mm_mul_ps(_mm_load_ps(data+3), _mm_replicate_y_ps(b.data[0]));
	// m2 = _mm_mul_ps(_mm_load_ps(data+6), _mm_replicate_z_ps(b.data[0]));
	// r.data[0] = _mm_add_ps(_mm_add_ps(m0, m1), m2);

	// m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(b.data[1]));
	// m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(b.data[1]));
	// m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(b.data[1]));
	// r.data[1] = _mm_add_ps(_mm_add_ps(m0, m1), m2);

	// m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(b.data[2]));
	// m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(b.data[2]));
	// m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(b.data[2]));
	// r.data[2] = _mm_add_ps(_mm_add_ps(m0, m1), m2);

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

