#ifndef gintonic_vec4f_hpp
#define gintonic_vec4f_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace GINTONIC_NAMESPACE_FBX
{
	class FbxVector4; // Forward declaration.	
}

namespace gintonic {

union vec2f; // Forward declaration.
union vec3f; // Forward declaration.

/*****************************************************************************
* gintonic::vec4f                                                            *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union vec4f
{
private:
	
public:

	__m128 data;

	struct
	{
		float x;
		float y;
		float z;
		float w;
	};

	inline vec4f() BOOST_NOEXCEPT_OR_NOTHROW
	{
		/* Empty on purpose. */
	}

	inline vec4f(const float s) BOOST_NOEXCEPT_OR_NOTHROW
	: data(_mm_set1_ps(s))
	{
		/* Empty on purpose. */
	}

	inline vec4f(const float a, const float b, 
		const float c, const float d) BOOST_NOEXCEPT_OR_NOTHROW
	: x(a), y(b), z(c), w(d)
	{
		/* Empty on purpose. */
	}

	inline vec4f(__m128 values) BOOST_NOEXCEPT_OR_NOTHROW : data(values)
	{
		/* Empty on purpose. */
	}

	inline vec4f(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW
	{
		float temp alignas(16) [4];
		std::copy(init.begin(), init.end(), temp);
		data = _mm_load_ps(temp);
		std::cout << "Floats are: " << temp[0] << ' '
			<< temp[1] << ' ' << temp[2] << ' ' << temp[3] << '\n';
		// data = _mm_load_ps(temp);
	}

	inline vec4f(const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW : data(v.data)
	{
		/* Empty on purpose. */
	}

	inline vec4f(vec4f&& v) BOOST_NOEXCEPT_OR_NOTHROW : data(v.data)
	{
		/* Empty on purpose. */
	}

	vec4f(const vec2f& v, const float z, const float w) BOOST_NOEXCEPT_OR_NOTHROW;
	vec4f(const vec2f& a, const vec2f& b) BOOST_NOEXCEPT_OR_NOTHROW;
	vec4f(const vec3f& v, const float w) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec4f& operator = (const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = v.data;
		return *this;
	}

	inline vec4f& operator = (vec4f&& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = v.data;
		return *this;
	}

	inline vec4f& operator=(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW
	{
		alignas(16) float temp[4];
		std::copy(init.begin(), init.end(), temp);
		data = _mm_load_ps(temp);
		return *this;
	}

	vec4f(const GINTONIC_NAMESPACE_FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW;

	vec4f& operator=(const GINTONIC_NAMESPACE_FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW;

	inline vec4f& operator += (const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_add_ps(data, v.data);
		return *this;
	}

	inline vec4f& operator += (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_add_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec4f& operator -= (const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_sub_ps(data, v.data);
		return *this;
	}

	inline vec4f& operator -= (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_sub_ps(data, _mm_set1_ps(s));
		return *this;
	}

	inline vec4f& operator *= (const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_mul_ps(data, v.data);
		return *this;
	}

	inline vec4f& operator *= (const float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		data = _mm_mul_ps(data, _mm_load1_ps(&s));
		return *this;
	}

	inline vec4f& operator /= (float s) BOOST_NOEXCEPT_OR_NOTHROW
	{
		s = 1.0f / s;
		return operator*=(s);
	}

	inline bool operator == (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	inline bool operator != (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(v);
	}

	inline bool operator < (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x < v.x && y < v.y && z < v.z && w < v.w;
	}

	inline bool operator >= (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator<(v);
	}

	inline bool operator > (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x > v.x && y > v.y && z > v.z && w > v.w;
	}

	inline bool operator <= (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator>(v);
	}

	inline vec4f operator + (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(data, v.data);
	}

	inline vec4f operator + (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(data, _mm_set1_ps(s));
	}

	inline friend vec4f operator + (const float s, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_add_ps(_mm_set1_ps(s), v.data);
	}

	inline vec4f operator - (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(data, v.data);
	}

	inline vec4f operator - (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(data, _mm_set1_ps(s));
	}

	inline friend vec4f operator - (const float s, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_sub_ps(_mm_set1_ps(s), v.data);
	}

	inline vec4f operator * (const vec4f& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(data, v.data);
	}

	inline vec4f operator * (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(data, _mm_load1_ps(&s));
	}

	inline friend vec4f operator * (const float lhs, const vec4f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(_mm_load1_ps(&lhs), rhs.data);
	}

	inline vec4f operator / (float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		s = 1.0f / s;
		return operator*(s);
	}

	inline friend float dot(const vec4f& lhs, const vec4f& rhs) BOOST_NOEXCEPT_OR_NOTHROW
	{
		#if GINTONIC_SSE_VERSION >= 41

			return _mm_cvtss_f32(_mm_dp_ps(lhs.data, rhs.data, 0xffffffff));

		#elif GINTONIC_SSE_VERSION >= 30

			const auto product = _mm_mul_ps(lhs.data, rhs.data);
			const auto halfsum = _mm_hadd_ps(product, product);
			return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));

		#endif
	}

	inline float length2() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return dot(*this, *this);
	}

	inline float length() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return std::sqrt(length2());
	}

	inline vec4f& normalize() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return operator/=(length());
	}

	inline friend vec4f deg2rad(const vec4f& degrees) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(degrees.data, _mm_set1_ps(0.0174532925f));
	}

	inline friend vec4f rad2deg(const vec4f& radians) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return _mm_mul_ps(radians.data, _mm_set1_ps(57.2957795f));
	}

	operator GINTONIC_NAMESPACE_FBX::FbxVector4() const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend class boost::serialization::access;

	template <class Archive> 
	inline void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) 
			& BOOST_SERIALIZATION_NVP(z) & BOOST_SERIALIZATION_NVP(w);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline float distance2(const vec4f& u, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (u-v).length2();
}

inline float distance(const vec4f& u, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (u-v).length();
}

inline vec4f mix(const vec4f& u, const vec4f& v, const float a) BOOST_NOEXCEPT_OR_NOTHROW
{
	return (1.0f - a) * u + a * v;
}

// WARNING: v needs to be of unit length.
inline vec4f project(const vec4f& u, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return dot(u,v) * v;
}

// WARNING: v needs to be of unit length.
inline vec4f reject(const vec4f& u, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return u - project(u, v);
}

// WARNING: v needs to be of unit length.
inline vec4f reflect(const vec4f& u, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return 2.0f * project(u,v) - v;
}

inline std::ostream& operator << (std::ostream& os, const vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
}

inline std::istream& operator >> (std::istream& is, vec4f& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	is >> v.x >> v.y >> v.z >> v.w;
	return is;
}

inline bool almost_equal(const vec4f& u, const vec4f& v, const int units_in_last_place)
{
	return almost_equal(0.0f, distance(u,v), units_in_last_place);
}

} // namespace gintonic

BOOST_CLASS_IMPLEMENTATION(gintonic::vec4f, boost::serialization::object_serializable);

#endif