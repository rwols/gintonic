#include "quatf.hpp"
#include "vec2f.hpp"
#include "vec3f.hpp"
#include "vec4f.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector4.h>

namespace gintonic {

quatf::quatf(const float realpart, const vec3f& imaginarypart)
{
	data = imaginarypart.data;
	w = realpart;
}

quatf::quatf(const FBXSDK_NAMESPACE::FbxVector4& v)
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
, z(static_cast<float>(v[2]))
, w(static_cast<float>(v[3]))
{
	/* Empty on purpose. */
}

quatf& quatf::operator = (const FBXSDK_NAMESPACE::FbxVector4& v)
{
	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	z = static_cast<float>(v[2]);
	w = static_cast<float>(v[3]);
	return *this;
}

vec3f quatf::apply_to(const vec3f& v) const noexcept
{
	return (*this) * quatf(0.0f, v) * conjugate();
	// return vec3f(temp.x, temp.y, temp.z);
}

vec3f quatf::forward_direction() const noexcept
{
	// return apply_to(vec3f(0.0f, 0.0f, -1.0f));
	// const auto temp = (*this) * quatf(-z, -y, x, -w);
	// return vec3f(temp.x, temp.y, temp.z);
	return (*this) * quatf(-z, -y, x, -w);
}

vec3f quatf::right_direction() const noexcept
{
	// return apply_to(vec3f(1.0f, 0.0f, 0.0f));
	// const auto temp = (*this) * quatf(x, -w, z, -y);
	// return vec3f(temp.x, temp.y, temp.z);
	return (*this) * quatf(x, w, z, -y);
}

vec3f quatf::up_direction() const noexcept
{
	// return apply_to(vec3f(0.0f, 1.0f, 0.0f));
	// const auto temp = (*this) * quatf(y, -z, w, x);
	// return vec3f(temp.x, temp.y, temp.z);
	return (*this) * quatf(y, -z, w, x);
}

vec3f quatf::direction() const noexcept
{
	return forward_direction();
}

quatf& quatf::set_mousedelta(const vec2f& angles) noexcept
{
	return *this = mouse(angles);
		// * axis_agle(vec3f(0.0f, 0.0f, -1.0f), roll);
}

quatf& quatf::add_mousedelta(const vec2f& mousedelta) noexcept
{
	// const float cx = std::cos(mousedelta.x * 0.5f);
	// const float sx = std::sin(mousedelta.x * 0.5f);
	// const float cy = std::cos(mousedelta.y * 0.5f);
	// const float sy = std::sin(mousedelta.y * 0.5f);

	// const quatf mouse(cx * sy, sx * sy, sx * cy, cx * cy);

	// return operator *= (mouse);

	*this = axis_angle(vec3f(1.0f, 0.0f, 0.0f), mousedelta.y) * (*this) * axis_angle(vec3f(0.0f, 1.0f, 0.0f), -mousedelta.x);
	return *this;

	// return operator *= (
	// 	axis_angle(vec3f(1.0f, 0.0f, 0.0f), mousedelta.y) *
	// 	axis_angle(vec3f(0.0f, 1.0f, 0.0f), mousedelta.x) 
	// 	);
}

quatf quatf::axis_angle(const vec3f& rotation_axis, const float rotation_angle)
{
	const auto s = std::sin(rotation_angle * 0.5f);
	const auto c = std::cos(rotation_angle * 0.5f);
	return quatf(c, s * rotation_axis);
}

float quatf::length2() const noexcept
{
	#ifdef BOOST_MSVC
	
	auto a = data;
	auto b = data;
	auto c = _mm_mul_ps(a, b);
	b = c;
	a = _mm_hadd_ps(b, c);
	b = a;
	return _mm_cvtss_f32(_mm_hadd_ps(a, b));
	
	#else
	
	__m128 l = _mm_mul_ps(data, data);
	__m128 k = _mm_hadd_ps(l, l);
	return _mm_cvtss_f32(_mm_hadd_ps(k, k));
	
	#endif
}

quatf quatf::operator * (const quatf& other) const noexcept
{
	/* multiplication of two quaternions (x, y, z, w) x (a, b, c, d)      */
	/* The product of two quaternions is:                                 */
	/* (X,Y,Z,W) = (xd+yc-zb+wa, -xc+yd+za+wb, xb-ya+zd+wc, -xa-yb-zc+wd) */

	#ifdef BOOST_MSVC

	auto copy = data;
	auto wzyx = _mm_shuffle_ps(copy, copy, _MM_SHUFFLE(0,1,2,3));
	copy = other.data;
	auto baba = _mm_shuffle_ps(copy, copy, _MM_SHUFFLE(0,1,0,1));
	auto dcdc = _mm_shuffle_ps(copy, copy, _MM_SHUFFLE(2,3,2,3));

	#else

	auto wzyx = _mm_shuffle_ps(data, data, _MM_SHUFFLE(0,1,2,3));
	auto baba = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(0,1,0,1));
	auto dcdc = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(2,3,2,3));

	#endif

	/* variable names below are for parts of componens of result (X,Y,Z,W) */
	/* nX stands for -X and similarly for the other components             */

	/* znxwy  = (xb - ya, zb - wa, wd - zc, yd - xc) */
	auto ZnXWY = _mm_hsub_ps(_mm_mul_ps(data, baba), _mm_mul_ps(wzyx, dcdc));

	/* xzynw  = (xd + yc, zd + wc, wb + za, yb + xa) */ 
	auto XZYnW = _mm_hadd_ps(_mm_mul_ps(data, dcdc), _mm_mul_ps(wzyx, baba));

	/* _mm_shuffle_ps(XZYnW, ZnXWY, _MM_SHUFFLE(3,2,1,0)) */
	/*      = (xd + yc, zd + wc, wd - zc, yd - xc)        */
	/* _mm_shuffle_ps(ZnXWY, XZYnW, _MM_SHUFFLE(2,3,0,1)) */
	/*      = (zb - wa, xb - ya, yb + xa, wb + za)        */

	/* _mm_addsub_ps adds elements 1 and 3 and subtracts elements 0 and 2, so we get: */
	/* _mm_addsub_ps(*, *) = (xd+yc-zb+wa, xb-ya+zd+wc, wd-zc+yb+xa, yd-xc+wb+za)     */

	auto XZWY = _mm_addsub_ps(_mm_shuffle_ps(XZYnW, ZnXWY, _MM_SHUFFLE(3,2,1,0)),
	_mm_shuffle_ps(ZnXWY, XZYnW, _MM_SHUFFLE(2,3,0,1)));

	/* now we only need to shuffle the components in place and return the result      */
	return _mm_shuffle_ps(XZWY, XZWY, _MM_SHUFFLE(2,1,3,0));

	/* operations: 6 shuffles, 4 multiplications, 3 compound additions/subtractions   */

	// This is the old way of multiplying two quaternions
	// return quatf(
	// 	w * b.w - x * b.x - y * b.y - z * b.z,
	// 	w * b.x + x * b.w + y * b.z - z * b.y,
	// 	w * b.y - x * b.z + y * b.w + z * b.x,
	// 	w * b.z + x * b.y - y * b.x + z * b.w);
}

quatf quatf::yaw_pitch_roll(
	const float yaw, 
	const float pitch, 
	const float roll)
{
	return
		axis_angle(vec3f(0.0f, 0.0f, -1.0f), roll)
		* axis_angle(vec3f(1.0f, 0.0f, 0.0f), pitch) 
		* axis_angle(vec3f(0.0f, 1.0f, 0.0f), yaw);
}

quatf quatf::look_at(const vec3f& eye_position, const vec3f& subject_position, const vec3f& up_direction)
{
	const auto direction = (subject_position - eye_position).normalize();

	if (almost_equal(direction.z, 1.0f, 5))
	{
		return axis_angle(up_direction, static_cast<float>(M_PI));
	}
	else if (almost_equal(direction.z, -1.0f, 5))
	{
		return quatf(1.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		const auto angle = std::acos(-direction.z);
		const auto axis = cross(vec3f(0.0f, 0.0f, -1.0f), direction).normalize();
		return axis_angle(axis, angle);
	}

	// return look_at((subject_position - eye_position).normalize(), up_direction);

	// const auto f = (subject_location - eye_location).normalize();
	// const auto s = (cross(f, up_direction)).normalize();
	// const auto u = cross(s, f).normalize();



	// auto direction = (subject_position - eye_position).normalize();
	// const auto dotproduct = dot(direction, up_direction);
	// direction = cross(direction, up_direction).normalize();
	// const auto rotation_angle = std::acos(dotproduct);
	// return axis_angle(direction, rotation_angle);

	// if (almost_equal(dotproduct, 1.0f, 5))
	// {
	// 	return quatf(1.0f, 0.0f, 0.0f, 0.0f);
	// }
	// else if (almost_equal(dotproduct, -1.0f, 5))
	// {
	// 	return quatf(-1.0f, 0.0f, 0.0f, 0.0f);
	// }
	// else
	// {
	// 	const auto right_direction = cross(look_direction, up_direction).normalize();
	// 	const auto rotation_angle = std::acos(dotproduct);
	// 	return axis_angle(right_direction, rotation_angle);
	// }
}

quatf quatf::look_at(const vec3f& f, const vec3f& up)
{

	const auto r = cross(f, up).normalize();
	const auto u = cross(r, f).normalize();

	// Now we have an orthonormal basis of unit vectors:
	// r (X), u (Y) and -f (Z).

	const auto tr = r.x + u.y - f.z;
	float S;

	#define m00 r.x
	#define m10 r.y
	#define m20 r.z
	#define m01 u.x
	#define m11 u.y
	#define m21 u.z
	#define m02 -f.x
	#define m12 -f.y
	#define m22 -f.z

	quatf q;

	if (tr > 0.0f)
	{ 
		S = std::sqrt(tr + 1.0f) * 2.0f; // S=4*q.w 
		q.x = (m21 - m12) / S;
		q.y = (m02 - m20) / S; 
		q.z = (m10 - m01) / S;
		q.w = 0.25f * S;
	}
	else if ((m00 > m11) & (m00 > m22))
	{ 
		S = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f; // S=4*q.x 
		q.x = 0.25f * S;
		q.y = (m01 + m10) / S; 
		q.z = (m02 + m20) / S;
		q.w = (m21 - m12) / S;
	}
	else if (m11 > m22)
	{ 
		S = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f; // S=4*q.y
		q.x = (m01 + m10) / S; 
		q.y = 0.25f * S;
		q.z = (m12 + m21) / S;
		q.w = (m02 - m20) / S;
	}
	else
	{ 
		S = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f; // S=4*q.z
		q.x = (m02 + m20) / S;
		q.y = (m12 + m21) / S;
		q.z = 0.25f * S;
		q.w = (m10 - m01) / S;
	}

	#undef m00
	#undef m10
	#undef m20
	#undef m01
	#undef m11
	#undef m21
	#undef m02
	#undef m12
	#undef m22

	return q;
}

void quatf::add_mouse(const vec2f& angles) noexcept
{
	const auto rot = axis_angle(vec3f(0.0f, 1.0f, 0.0f), angles.x);
	*this = axis_angle(rot.right_direction(), angles.y) * (*this) * rot;
}

quatf quatf::mouse(const vec2f& angles)
{
	const auto rot = axis_angle(vec3f(0.0f, 1.0f, 0.0f), angles.x);
	return axis_angle(rot.right_direction(), angles.y) * rot;
}

} // namespace gintonic