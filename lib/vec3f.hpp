#ifndef gintonic_vec3f_hpp
#define gintonic_vec3f_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace FBX
{
	class FbxVector4; // Forward declaration.	
}

namespace gintonic {

union vec2f; // Forward declaration.

/*****************************************************************************
* gintonic::vec3f                                                            *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union vec3f
{
private:
	__m128 data;
	friend union vec4f;
	friend union quatf;
	friend union mat3f;
	friend union mat4f;
	friend struct SQT;
public:

	struct
	{
		float x;
		float y;
		float z;
		float dummy;
	};

	inline vec3f() BOOST_NOEXCEPT_OR_NOTHROW
	: dummy(0.0f)
	{
		/* Empty on purpose. */
	}

	inline vec3f(const float s) BOOST_NOEXCEPT_OR_NOTHROW
	: data(_mm_set1_ps(s))
	{
		/* Empty on purpose. */
	}

	inline vec3f(const float a, const float b, const float c) BOOST_NOEXCEPT_OR_NOTHROW
	: x(a), y(b), z(c), dummy(0.0f)
	// : data(_mm_set_ps(x, y, z, 0.0f))
	{
		/* Empty on purpose. */
	}

	inline vec3f(__m128 values) BOOST_NOEXCEPT_OR_NOTHROW : data(values)
	{
		/* Empty on purpose. */
	}

	vec3f(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec3f(const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW : data(v.data)
	{
		/* Empty on purpose. */
	}

	inline vec3f(vec3f&& v) BOOST_NOEXCEPT_OR_NOTHROW : data(v.data)
	{
		/* Empty on purpose. */
	}

	vec3f(const vec2f& v, const float z) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec3f& operator = (const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = v.data;
		return *this;
	}

	inline vec3f& operator = (vec3f&& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = v.data;
		return *this;
	}

	vec3f& operator=(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f(const FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f& operator=(const FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec3f& operator += (const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_add_ps(data, v.data);
		return *this;
	}

	inline vec3f& operator += (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_add_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec3f& operator -= (const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_sub_ps(data, v.data);
		return *this;
	}

	inline vec3f& operator -= (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_sub_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec3f& operator *= (const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_mul_ps(data, v.data);
		return *this;
	}

	inline vec3f& operator *= (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_mul_ps(data, _mm_load1_ps(&s));
		return *this;
	}

	inline vec3f& operator /= (float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		s = 1.0f / s;
		return operator*=(s);
	}

	inline bool operator == (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == v.x && y == v.y && z == v.z;
	}

	inline bool operator != (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(v);
	}

	inline bool operator < (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x < v.x && y < v.y && z < v.z;
	}

	inline bool operator >= (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x >= v.x && y >= v.y && z >= v.z;
	}

	inline bool operator > (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x > v.x && y > v.y && z > v.z;
	}

	inline bool operator <= (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x <= v.x && y <= v.y && z <= v.z;
	}

	inline vec3f operator + (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(data, v.data);
	}

	inline vec3f operator + (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(data, _mm_set1_ps(s));
	}

	inline friend vec3f operator + (const float s, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(_mm_set1_ps(s), v.data);
	}

	inline vec3f operator - (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(data, v.data);
	}

	inline vec3f operator - (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(data, _mm_set1_ps(s));
	}

	inline friend vec3f operator - (const float s, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(_mm_set1_ps(s), v.data);
	}

	inline vec3f operator * (const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(data, v.data);
	}

	inline vec3f operator * (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(data, _mm_load1_ps(&s));
	}

	friend vec3f operator * (const float lhs, const vec3f& rhs) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec3f operator / (float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		s = 1.0f / s;
		return operator*(s);
	}

	inline friend float dot(const vec3f& lhs, const vec3f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
	{
		#if GINTONIC_SSE_VERSION >= 41

			return _mm_cvtss_f32(_mm_dp_ps(lhs.data, rhs.data, 0xffffffff));

		#elif GINTONIC_SSE_VERSION >= 30

			const auto product = _mm_mul_ps(lhs.data, rhs.data);
			const auto halfsum = _mm_hadd_ps(product, product);
			return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));

		#endif
	}

	inline friend vec3f cross(const vec3f& lhs, const vec3f& rhs)
	{
		auto result = _mm_sub_ps(
			_mm_mul_ps(
				lhs.data, 
				_mm_shuffle_ps(
					rhs.data, 
					rhs.data, 
					_MM_SHUFFLE(3, 0, 2, 1))),
			_mm_mul_ps(
				rhs.data, 
				_mm_shuffle_ps(
					lhs.data, 
					lhs.data,
					_MM_SHUFFLE(3, 0, 2, 1))));
		return _mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 0, 2, 1));
	}

	inline float length2() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return dot(*this, *this);
	}

	inline float length() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return std::sqrt(length2());
	}

	inline vec3f& normalize() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return operator/=(length());
	}

	inline friend vec3f deg2rad(const vec3f& degrees) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(degrees.data, _mm_set1_ps(0.0174532925f));
	}

	inline friend vec3f rad2deg(const vec3f& radians) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(radians.data, _mm_set1_ps(57.2957795f));
	}

	operator FBX::FbxVector4() const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend class boost::serialization::access;

	template <class Archive> 
	inline void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) & BOOST_SERIALIZATION_NVP(z);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline vec3f operator * (const float lhs, const vec3f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
	return _mm_mul_ps(_mm_load1_ps(&lhs), rhs.data);
}

inline float distance2(const vec3f& u, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (u-v).length2();
}

inline float distance(const vec3f& u, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (u-v).length();
}

inline vec3f mix(const vec3f& u, const vec3f& v, const float a) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (1.0f - a) * u + a * v;
}

// WARNING: v needs to be of unit length.
inline vec3f project(const vec3f& u, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return dot(u,v) * v;
}

// WARNING: v needs to be of unit length.
inline vec3f reject(const vec3f& u, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return u - project(u, v);
}

// WARNING: v needs to be of unit length.
inline vec3f reflect(const vec3f& u, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return 2.0f * project(u,v) - v;
}

inline std::ostream& operator << (std::ostream& os, const vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << v.x << ' ' << v.y << ' ' << v.z;
}

inline std::istream& operator >> (std::istream& is, vec3f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	is >> v.x >> v.y >> v.z;
	return is;
}

inline bool almost_equal(const vec3f& u, const vec3f& v, const int units_in_last_place)
{
	return almost_equal(0.0f, distance(u,v), units_in_last_place);
}

} // namespace gintonic

BOOST_CLASS_IMPLEMENTATION(gintonic::vec3f, boost::serialization::object_serializable);

#endif