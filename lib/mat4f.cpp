#include "mat4f.hpp"
#include "vec3f.hpp"
#include "vec4f.hpp"
#include "quatf.hpp"
#include "mat3f.hpp"
#include "SQT.hpp"

namespace gintonic {

#ifdef BOOST_MSVC // NOTE: Consider removing this when on MSVC2015

mat4f::mat4f(const vec3f& translation)
{
	data[0] = _mm_set1_ps(0.0f);
	data[1] = _mm_set1_ps(0.0f);
	data[2] = _mm_set1_ps(0.0f);
	data[3] = translation.data;
	m33 = 1.0f;
}

#else

mat4f::mat4f(const vec3f& translation)
: m00(1.0f), m10(0.0f), m20(0.0f), m30(0.0f)
, m01(0.0f), m11(1.0f), m21(0.0f), m31(0.0f)
, m02(0.0f), m12(0.0f), m22(1.0f), m32(0.0f)
, m03(translation.x), m13(translation.y), m23(translation.z), m33(1.0f)
{
	/* Empty on purpose. */
}

#endif



mat4f::mat4f(const quatf& rotation)
{
	// SIMDify this...
	const auto n = rotation.length2();
	const auto s = (n == 0.0f) ? 0.0f : 2.0f / n;
	const auto wx = s * rotation.w * rotation.x;
	const auto wy = s * rotation.w * rotation.y;
	const auto wz = s * rotation.w * rotation.z;
	const auto xx = s * rotation.x * rotation.x;
	const auto xy = s * rotation.x * rotation.y;
	const auto xz = s * rotation.x * rotation.z;
	const auto yy = s * rotation.y * rotation.y;
	const auto yz = s * rotation.y * rotation.z;
	const auto zz = s * rotation.z * rotation.z;
	
	m00 = 1.0f - (yy + zz);
	m10 = xy + wz;
	m20 = xz - wy;
	m30 = 0.0f;

	m01 = xy - wz;
	m11 = 1.0f - (xx + zz);
	m21 = yz + wx;
	m31 = 0.0f;
	
	m02 = xz + wy;
	m12 = yz - wx;
	m22 = 1.0f - (xx + yy);
	m32 = 0.0f;
	
	m03 = 0.0f;
	m13 = 0.0f;
	m23 = 0.0f;
	m33 = 1.0f;
}

mat4f::mat4f(const SQT& transform) : mat4f(transform.rotation)
{
	data[0] = _mm_mul_ps(data[0], transform.scale.data);
	data[1] = _mm_mul_ps(data[1], transform.scale.data);
	data[2] = _mm_mul_ps(data[2], transform.scale.data);
	data[3] = transform.translation.data;
	m33 = 1.0f;
}

mat4f::mat4f(const mat3f& rotation_part)
{
	data[0] = _mm_set_ps(rotation_part.data[0], rotation_part.data[1], rotation_part.data[2], 0.0f);
	data[1] = _mm_set_ps(rotation_part.data[3], rotation_part.data[4], rotation_part.data[5], 0.0f);
	data[2] = _mm_set_ps(rotation_part.data[6], rotation_part.data[7], rotation_part.data[8], 0.0f);

	m30 = 0.0f;
	m31 = 0.0f;
	m32 = 0.0f;

	m03 = 0.0f;
	m13 = 0.0f;
	m23 = 0.0f;
	m33 = 1.0f;
}

#ifdef BOOST_MSVC // NOTE: Consider removing this when on MSVC2015

mat4f::mat4f(const vec4f& column0, const vec4f& column1, const vec4f& column2, const vec4f& column3)
{
	data[0] = column0.data;
	data[1] = column1.data;
	data[2] = column2.data;
	data[3] = column3.data;
}

#else

mat4f::mat4f(const vec4f& column0, const vec4f& column1, const vec4f& column2, const vec4f& column3)
: data{column0.data, column1.data, column2.data, column3.data}
{
	/* Empty on purpose. */
}

#endif

vec4f mat4f::operator * (const vec4f& v) const
{
	const auto m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(v.data));
	const auto m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(v.data));
	const auto m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(v.data));
	const auto m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(v.data));
	return _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));
}

mat4f mat4f::operator * (const mat4f& other) const
{
	mat4f r;
	__m128 m0, m1, m2, m3;

	m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[0]));
	m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[0]));
	m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[0]));
	m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[0]));
	r.data[0] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

	m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[1]));
	m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[1]));
	m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[1]));
	m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[1]));
	r.data[1] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

	m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[2]));
	m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[2]));
	m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[2]));
	m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[2]));
	r.data[2] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

	m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[3]));
	m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[3]));
	m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[3]));
	m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[3]));
	r.data[3] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

	return r;
}

mat4f::mat4f(const vec3f& axis, const float rotation_angle)
{
	vec3f v;
	const float sin = std::sin(rotation_angle);
	const float cos = std::cos(rotation_angle);
	const float omcos = 1.0f - cos;

	v.x = cos + axis.x * axis.x * omcos;
	v.y = axis.x * axis.y * omcos - axis.z * sin;
	v.z = axis.x * axis.z * omcos + axis.y * sin;
	v.normalize();

	m00 = v.x; m01 = v.y; m02 = v.z; m03 = 0.0f;

	v.x = axis.x * axis.y * omcos + axis.z * sin;
	v.y = cos + axis.y * axis.y * omcos;
	v.z = axis.y * axis.z * omcos - axis.x * sin;
	v.normalize();

	m10 = v.x; m11 = v.y; m12 = v.z; m13 = 0.0f;

	v.x = axis.x * axis.z * omcos - axis.y * sin;
	v.y = axis.y * axis.z * omcos + axis.x * sin;
	v.z = cos + axis.z * axis.z * omcos;
	v.normalize();

	m20 = v.x; m21 = v.y; m22 = v.z; m23 = 0.0f;

	m30 = m31 = m32 = 0.0f; m33 = 1.0f;
	#undef m
}

mat4f& mat4f::set_orthographic(const float left, const float right, const float bottom, const float top, const float nearplane, const float farplane)
{
	const float rl = right - left;
	const float tp = top - bottom;
	const float fn = farplane - nearplane;

	data[0] = _mm_set1_ps(0.0f);
	data[1] = _mm_set1_ps(0.0f);
	data[2] = _mm_set1_ps(0.0f);
	data[3] = _mm_set1_ps(0.0f);

	m00 = 2.0f * nearplane / rl;
	m11 = 2.0f * nearplane / tp;
	m02 = (right + left) / rl;
	m12 = (top + bottom) / tp;
	m22 = -(farplane + nearplane) / fn;
	m32 = 0.0f;
	m23 = -(2.0f * farplane * nearplane) / fn;
	m33 = 1.0f;
	return *this;
}

mat4f& mat4f::set_orthographic(const float width, const float height, const float nearplane, const float farplane)
{
	data[0] = _mm_set1_ps(0.0f);
	data[1] = _mm_set1_ps(0.0f);
	data[2] = _mm_set1_ps(0.0f);
	data[3] = _mm_set1_ps(0.0f);

	m00 = 1.0f / width;
	m11 = 1.0f / height;
	m22 = -2.0f / (farplane - nearplane);
	m23 = -(farplane + nearplane) / (farplane - nearplane);
	m33 = 1.0f;
	return *this;
}

mat4f::mat4f(const vec3f& eye_location, const vec3f& subject_location, const vec3f& up_direction)
{
	const auto f = (subject_location - eye_location).normalize();
	const auto s = (cross(f, up_direction)).normalize();
	const auto u = cross(s, f).normalize();

	m00 =  s.x;
	m10 =  u.x;
	m20 = -f.x;
	m30 = 0.0f;

	m01 =  s.y;
	m11 =  u.y;
	m21 = -f.y;
	m31 = 0.0f;

	m02 =  s.z;
	m12 =  u.z;
	m22 = -f.z;
	m32 = 0.0f;

	m03 = -dot(eye_location, s);
	m13 = -dot(eye_location, u);
	m23 =  dot(eye_location, f);
	m33 = 1.0f;
}

mat4f& mat4f::set_perspective(const float fieldofview, const float aspectratio, const float nearplane, const float farplane)
{
	data[0] = _mm_set1_ps(0.0f);
	data[1] = _mm_set1_ps(0.0f);
	data[2] = _mm_set1_ps(0.0f);
	data[3] = _mm_set1_ps(0.0f);

	const float f = 1.0f / std::tan(fieldofview / 2.0f);

	m00 = f / aspectratio;
	m11 = f;
	m22 = (farplane + nearplane) / (nearplane - farplane);
	m32 = -1.0f;
	m23 = 2.0f * farplane * nearplane / (nearplane - farplane);

	return *this;
}

mat4f& mat4f::set_perspective_infinite(const float fieldofview, const float aspectratio, const float nearplane)
{
	data[0] = _mm_set1_ps(0.0f);
	data[1] = _mm_set1_ps(0.0f);
	data[2] = _mm_set1_ps(0.0f);
	data[3] = _mm_set1_ps(0.0f);

	m00 = std::tan(fieldofview / 2.0f) / aspectratio;

	return *this;
}

mat4f& mat4f::set_inverse_perspective(const float fieldofview, const float aspectratio, const float nearplane, const float farplane)
{
	const float b = 1.0f / std::tan(fieldofview / 2.0f);
	const float nf2 = 2.0f * nearplane * farplane;
	
	data[0] = _mm_set1_ps(0.0f);
	data[1] = _mm_set1_ps(0.0f);
	data[2] = _mm_set1_ps(0.0f);
	data[3] = _mm_set1_ps(0.0f);

	m00 = aspectratio / b;
	m11 = 1.0f / b;
	m32 = (nearplane - farplane) / nf2;
	m23 = 1.0f;
	m33 = (nearplane + farplane) / nf2;

	return *this;
}

void mat4f::unproject_perspective(float& fieldofview, float& aspectratio, float& nearplane, float& farplane)
{
	fieldofview = 2.0f * std::atan(1 / m11);
	aspectratio = m11 / m00;
	nearplane = m23 / (m22 - 1.0f);
	farplane = m23 / (m22 + 1.0f);

		// fov = T(2) * std::atan(1 / m(1,1));
		// aspectratio = m(1,1) / m(0,0);
		// nearplane = m(2,3) / (m(2,2) - T(1));
		// farplane = m(2,3) / (m(2,2) + T(1));
}

vec3f mat4f::apply_to_point(const vec3f& point) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto tmp = *this * vec4f(point, 1.0f);
	return vec3f(tmp.x, tmp.y, tmp.z);
}

vec3f mat4f::apply_to_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto tmp = *this * vec4f(direction, 0.0f);
	return vec3f(tmp.x, tmp.y, tmp.z);
}

mat3f mat4f::upper_left_33() const
{
	return mat3f(vec3f(data[0]), vec3f(data[1]), vec3f(data[2]));
}

} // namespace gintonic