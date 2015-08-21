#ifndef gintonic_vec2f_hpp
#define gintonic_vec2f_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace GINTONIC_NAMESPACE_FBX
{
	class FbxVector2; // Forward declaration.	
}

namespace gintonic
{

/*****************************************************************************
* gintonic::vec2f                                                            *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union vec2f
{
private:
	__m128 data;
	friend union vec3f;
	friend union vec4f;
public:

	struct
	{
		float x;
		float y;
		float dummy0;
		float dummy1;
	};

	inline vec2f() BOOST_NOEXCEPT_OR_NOTHROW
	: dummy0(0.0f), dummy1(0.0f)
	{
		/* Empty on purpose. */
	}

	inline vec2f(const float s) BOOST_NOEXCEPT_OR_NOTHROW
	: data(_mm_set1_ps(s))
	{
		/* Empty on purpose. */
	}

	inline vec2f(const float a, const float b) BOOST_NOEXCEPT_OR_NOTHROW
	: x(a), y(b), dummy0(0.0f), dummy1(0.0f)
	{

	}

	inline vec2f(__m128 values) BOOST_NOEXCEPT_OR_NOTHROW : data(values)
	{
		/* Empty on purpose. */
	}

	vec2f(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec2f(const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW : data(v.data)
	{
		/* Empty on purpose. */
	}

	inline vec2f(vec2f&& v) BOOST_NOEXCEPT_OR_NOTHROW : data(v.data)
	{
		/* Empty on purpose. */
	}

	vec2f(const GINTONIC_NAMESPACE_FBX::FbxVector2& v) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec2f& operator = (const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = v.data;
		return *this;
	}

	inline vec2f& operator = (vec2f&& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = v.data;
		return *this;
	}

	vec2f& operator=(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW;

	vec2f& operator=(const GINTONIC_NAMESPACE_FBX::FbxVector2& v) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec2f& operator += (const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_add_ps(data, v.data);
		return *this;
	}

	inline vec2f& operator += (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_add_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec2f& operator -= (const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_sub_ps(data, v.data);
		return *this;
	}

	inline vec2f& operator -= (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_sub_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec2f& operator *= (const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_mul_ps(data, v.data);
		return *this;
	}

	inline vec2f& operator *= (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_mul_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec2f& operator /= (float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		s = 1.0f / s;
		return operator*=(s);
	}

	inline bool operator == (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == v.x && y == v.y;
	}

	inline bool operator != (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(v);
	}

	inline bool operator < (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x < v.x && y < v.y;
	}

	inline bool operator >= (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator<(v);
	}

	inline bool operator > (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x > v.x && y > v.y;
	}

	inline bool operator <= (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator>(v);
	}

	inline vec2f operator + (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(data, v.data);
	}

	inline vec2f operator + (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(data, _mm_set1_ps(s));
	}

	inline friend vec2f operator + (const float s, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(_mm_set1_ps(s), v.data);
	}

	inline vec2f operator - (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(data, v.data);
	}

	inline vec2f operator - (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(data, _mm_set1_ps(s));
	}

	inline friend vec2f operator - (const float s, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(_mm_set1_ps(s), v.data);
	}

	inline vec2f operator - () const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(_mm_set1_ps(-1.0f), data);
	}

	inline vec2f operator * (const vec2f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(data, v.data);
	}

	inline vec2f operator * (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(data, _mm_load1_ps(&s));
	}

	friend vec2f operator * (const float lhs, const vec2f& rhs) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec2f operator / (float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		s = 1.0f / s;
		return operator*(s);
	}

	inline friend float dot(const vec2f& lhs, const vec2f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto product = _mm_mul_ps(lhs.data, rhs.data);
		return _mm_cvtss_f32(_mm_hadd_ps(product, product));
	}

	inline float length2() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return dot(*this, *this);
	}

	inline float length() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return std::sqrt(length2());
	}

	inline vec2f& normalize() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return operator/=(length());
	}

	friend vec2f deg2rad(const vec2f& degrees) BOOST_NOEXCEPT_OR_NOTHROW;

	friend vec2f rad2deg(const vec2f& radians) BOOST_NOEXCEPT_OR_NOTHROW;

	inline friend __m128 pack(const vec2f& lhs, const vec2f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_shuffle_ps(lhs.data, rhs.data, 0x44);
	}

	operator GINTONIC_NAMESPACE_FBX::FbxVector2() const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend class boost::serialization::access;

	template <class Archive>
	inline void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline vec2f operator * (const float lhs, const vec2f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
	return _mm_mul_ps(_mm_load1_ps(&lhs), rhs.data);
}

inline vec2f deg2rad(const vec2f& degrees) BOOST_NOEXCEPT_OR_NOTHROW
{
	return _mm_mul_ps(degrees.data, _mm_set1_ps(0.0174532925f));
}

inline vec2f rad2deg(const vec2f& radians) BOOST_NOEXCEPT_OR_NOTHROW
{
	return _mm_mul_ps(radians.data, _mm_set1_ps(57.2957795f));
}

inline float distance2(const vec2f& u, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (u-v).length2();
}

inline float distance(const vec2f& u, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (u-v).length();
}

inline vec2f mix(const vec2f& u, const vec2f& v, const float a) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (1.0f - a) * u + a * v;
}

// WARNING: The vector v needs to be of unit length.
inline vec2f project(const vec2f& u, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return dot(u,v) * v;
}

// WARNING: The vector v needs to be of unit length.
inline vec2f reject(const vec2f& u, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return u - project(u, v);
}

// WARNING: The vector v needs to be of unit length.
inline vec2f reflect(const vec2f& u, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return 2.0f * project(u,v) - v;
}

inline std::ostream& operator << (std::ostream& os, const vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << v.x << ' ' << v.y;
}

inline std::istream& operator >> (std::istream& is, vec2f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	is >> v.x >> v.y;
	return is;
}

inline bool almost_equal(const vec2f& u, const vec2f& v, 
	const int units_in_last_place)
{
	return almost_equal(0.0f, distance(u,v), units_in_last_place);
}

} // namespace gintonic

BOOST_CLASS_IMPLEMENTATION(gintonic::vec2f, boost::serialization::object_serializable);

#endif