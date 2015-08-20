#ifndef gintonic_quatf_hpp
#define gintonic_quatf_hpp

#include "vec3f.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

/*****************************************************************************
* gintonic::quatf                                                            *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union quatf
{
public:
	__m128 data;

	struct
	{
		float x;
		float y;
		float z;
		float w;
	};

	inline quatf() = default;

	inline quatf(const __m128& data) : data(data)
	{
		/* Empty on purpose. */
	}

	inline quatf(const float w, const float x, const float y, const float z)
	: x(x), y(y), z(z), w(w)  
	{
		/* Empty on purpose. */
	}

	inline quatf(const float realpart, const vec3f& imaginarypart)
	{
		data = imaginarypart.data;
		w = realpart;
	}

	inline quatf operator * (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW
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

	inline quatf& operator *= (const quatf& q) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return *this = *this * q;
	}

	inline quatf conjugate() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return quatf(w, -x, -y, -z);
	}

	vec3f apply_to(const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto temp = (*this) * quatf(0.0f, v) * conjugate();
		return vec3f(temp.x, temp.y, temp.z);
	}

	inline vec3f direction() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return apply_to(vec3f(0.0f, 0.0f, -1.0f));
	}

	float length2() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		auto l = _mm_mul_ps(data, data);
		l = _mm_hadd_ps(l, l);
		l = _mm_hadd_ps(l, l);
		return _mm_cvtss_f32(l);
	}

	inline float length() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return std::sqrt(length2());
	}

	inline bool operator == (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	inline bool operator != (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	static quatf axis_angle(const vec3f& rotation_axis, const float rotation_angle);
	static quatf look_at(vec3f eye_position, vec3f subject_position, const vec3f& up_direction);

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(w) 
			& BOOST_SERIALIZATION_NVP(x) 
			& BOOST_SERIALIZATION_NVP(y) 
			& BOOST_SERIALIZATION_NVP(z);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline std::ostream& operator<<(std::ostream& os, const quatf& q)
{
	return os << q.w << ' ' << q.x << ' ' << q.y << ' ' << q.z;
}

inline std::istream& operator>>(std::istream& is, quatf& q)
{
	is >> q.w >> q.x >> q.y >> q.z;
	return is;
}

} // namespace gintonic

#endif