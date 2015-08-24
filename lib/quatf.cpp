#include "quatf.hpp"
#include "vec2f.hpp"
#include "vec3f.hpp"

namespace gintonic {

quatf::quatf(const float realpart, const vec3f& imaginarypart)
{
	data = imaginarypart.data;
	w = realpart;
}

vec3f quatf::apply_to(const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto temp = (*this) * quatf(0.0f, v) * conjugate();
	return vec3f(temp.x, temp.y, temp.z);
}

vec3f quatf::forward_direction() const BOOST_NOEXCEPT_OR_NOTHROW
{
	// return apply_to(vec3f(0.0f, 0.0f, -1.0f));
	// const auto temp = (*this) * quatf(-z, -y, x, -w);
	// return vec3f(temp.x, temp.y, temp.z);
	return (*this) * quatf(-z, -y, x, -w);
}

vec3f quatf::right_direction() const BOOST_NOEXCEPT_OR_NOTHROW
{
	// return apply_to(vec3f(1.0f, 0.0f, 0.0f));
	// const auto temp = (*this) * quatf(x, -w, z, -y);
	// return vec3f(temp.x, temp.y, temp.z);
	return (*this) * quatf(x, w, z, -y);
}

vec3f quatf::up_direction() const BOOST_NOEXCEPT_OR_NOTHROW
{
	// return apply_to(vec3f(0.0f, 1.0f, 0.0f));
	// const auto temp = (*this) * quatf(y, -z, w, x);
	// return vec3f(temp.x, temp.y, temp.z);
	return (*this) * quatf(y, -z, w, x);
}

vec3f quatf::direction() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return forward_direction();
}

quatf& quatf::add_mousedelta(const vec2f& mousedelta)
{
	// const float cx = std::cos(mousedelta.x * 0.5f);
	// const float sx = std::sin(mousedelta.x * 0.5f);
	// const float cy = std::cos(mousedelta.y * 0.5f);
	// const float sy = std::sin(mousedelta.y * 0.5f);

	// const quatf mouse(cx * sy, sx * sy, sx * cy, cx * cy);
	return operator *= (
		axis_angle(vec3f(0.0f, 1.0f, 0.0f), mousedelta.x) * 
		axis_angle(vec3f(1.0f, 0.0f, 0.0f), mousedelta.y));
}

quatf quatf::axis_angle(const vec3f& rotation_axis, const float rotation_angle)
{
	const auto s = std::sin(rotation_angle * 0.5f);
	const auto c = std::cos(rotation_angle * 0.5f);
	return quatf(c, s * rotation_axis);
}

float quatf::length2() const BOOST_NOEXCEPT_OR_NOTHROW
{
	__m128 l = _mm_mul_ps(data, data);
	__m128 k = _mm_hadd_ps(l, l);
	return _mm_cvtss_f32(_mm_hadd_ps(k, k));
}

quatf quatf::operator * (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	/* multiplication of two quaternions (x, y, z, w) x (a, b, c, d)      */
	/* The product of two quaternions is:                                 */
	/* (X,Y,Z,W) = (xd+yc-zb+wa, -xc+yd+za+wb, xb-ya+zd+wc, -xa-yb-zc+wd) */

	auto wzyx = _mm_shuffle_ps(data, data, _MM_SHUFFLE(0,1,2,3));
	auto baba = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(0,1,0,1));
	auto dcdc = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(2,3,2,3));

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

quatf quatf::look_at(const vec3f& eye_position, const vec3f& subject_position, const vec3f& up_direction)
{
	auto eye_copy = eye_position;
	auto up_copy = up_direction;
	const auto dotproduct = dot(eye_copy.normalize(), up_copy.normalize());
	if (almost_equal(dotproduct, 1.0f, 5))
	{
		return quatf(1.0f, 0.0f, 0.0f, 0.0f);
	}
	else if (almost_equal(dotproduct, -1.0f, 5))
	{
		return quatf(-1.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		const auto rotation_axis = cross(subject_position, up_copy).normalize();
		const auto rotation_angle = std::acos(dotproduct);
		return axis_angle(rotation_axis, rotation_angle);
	}
}

} // namespace gintonic