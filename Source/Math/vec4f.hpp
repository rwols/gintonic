/**
 * @file vec4f.hpp
 * @brief Defines a four-dimensional vector class.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <fbxsdk/fbxsdk_nsbegin.h>
class FbxVector4; // Forward declaration.	
#include <fbxsdk/fbxsdk_nsend.h>

#include <array>

namespace gintonic {

union vec2f; // Forward declaration.
union vec3f; // Forward declaration.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

/**
 * @brief Four-dimensional vector class that uses SSE.
 * 
 * @details The fact that this class carries an SSE type (namely, __m128) has
 * some consequences for classes that want to use this class as a datamember.
 * The SSE types need to be aligned to a 16-byte memory boundary. This is
 * done automatically by the compiler when you instantiate an object carrying
 * an SSE class on the program stack. It is **not done automatically** by the
 * compiler when you instantiate such a class **on the heap**. For this
 * reason, you **must** declare the macro
 * `GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();`
 * inside the definition of your class. This will take care of all memory
 * boundary problems. If you don't do this, your program will sporadically
 * crash every once in a while because just by chance objects *are* allocated
 * correctly on a 16-nyte memory boundary, while on some occassions they are
 * not. In fact, Linux and OSX allocate memory on a 16-byte boundary by
 * default, so we wouldn't need to worry about it if `gintonic` should only
 * work on those platforms. However Windows allocates on 8-byte boundaries.
 * 
 * If for some reason you need to override `operator new` and
 * `operator delete` for your class, then you cannot use the above mentioned
 * macro because the macro itself already overrides those operators. In this
 * case you will have to make sure by hand that objects are allocated on a
 * 16-byte boundary.
 */
union vec4f
{
private:
	
public:

	/// The raw SSE type.
	__m128 data;

	struct
	{
		/// The X-coordinate.
		float x;
		/// The Y-coordinate.
		float y;
		/// The Z-coordinate.
		float z;
		/// The W-coordinate.
		float w;
	};

	/// Default constructor.
	inline vec4f() noexcept
	{
		/* Empty on purpose. */
	}

	/// Constructor that sets every coordinate to the given value.
	inline vec4f(const float s) noexcept
	: data(_mm_set1_ps(s))
	{
		/* Empty on purpose. */
	}

	/// Constructor.
	inline vec4f(const float a, const float b, 
		const float c, const float d) noexcept
	: x(a), y(b), z(c), w(d)
	{
		/* Empty on purpose. */
	}

	/// Constructor for a raw SSE value.
	inline vec4f(__m128 values) noexcept : data(values)
	{
		/* Empty on purpose. */
	}

	/// Initializer list constructor.
	vec4f(std::initializer_list<float> init) noexcept;

	/// Copy constructor.
	inline vec4f(const vec4f& v) noexcept : data(v.data)
	{
		/* Empty on purpose. */
	}

	/// Move constructor.
	inline vec4f(vec4f&& v) noexcept : data(v.data)
	{
		/* Empty on purpose. */
	}

	/// Constructor.
	vec4f(const vec2f& v, const float z, const float w) 
		noexcept;

	/// Constructor.
	vec4f(const vec2f& a, const vec2f& b) noexcept;

	/// Constructor.
	vec4f(const vec3f& v, const float w) noexcept;

	/// Copy assignment operator.
	inline vec4f& operator = (const vec4f& v) noexcept
	{
		data = v.data;
		return *this;
	}

	/// Move assignment operator.
	inline vec4f& operator = (vec4f&& v) noexcept
	{
		data = v.data;
		return *this;
	}

	/// Initializer list assignment operator.
	vec4f& operator=(std::initializer_list<float> init) 
		noexcept;

	/// Constructor that takes an FbxVector4.
	vec4f(const FBXSDK_NAMESPACE::FbxVector4& v) noexcept;

	/// Assignment operator that takes an FbxVector4.
	vec4f& operator=(const FBXSDK_NAMESPACE::FbxVector4& v) noexcept;

	/// Add-and-assign operator.
	inline vec4f& operator += (const vec4f& v) noexcept
	{
		data = _mm_add_ps(data, v.data);
		return *this;
	}

	/// Add-and-assign operator that first builds a vec4f.
	inline vec4f& operator += (const float s) noexcept
	{
		data = _mm_add_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Subtract-and-assign operator.
	inline vec4f& operator -= (const vec4f& v) noexcept
	{
		data = _mm_sub_ps(data, v.data);
		return *this;
	}

	/// Subtract-and-assign operator that first builds a vec4f.
	inline vec4f& operator -= (const float s) noexcept
	{
		data = _mm_sub_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Pointwise-multiply-and-assign operator.
	inline vec4f& operator *= (const vec4f& v) noexcept
	{
		data = _mm_mul_ps(data, v.data);
		return *this;
	}

	/// Pointwise-multiply-and-assign operator.
	inline vec4f& operator *= (const float s) noexcept
	{
		data = _mm_mul_ps(data, _mm_load1_ps(&s));
		return *this;
	}

	/// Pointwise-divide-and-assign operator.
	inline vec4f& operator /= (float s) noexcept
	{
		s = 1.0f / s;
		return operator*=(s);
	}

	/// Equality comparison operator.
	inline bool operator == (const vec4f& v) const noexcept
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	/// Inequality comparison operator.
	inline bool operator != (const vec4f& v) const noexcept
	{
		return !operator==(v);
	}

	/// Less-than comparison operator.
	inline bool operator < (const vec4f& v) const noexcept
	{
		return x < v.x && y < v.y && z < v.z && w < v.w;
	}

	/// Greater-than-or-equal-to comparison operator.
	inline bool operator >= (const vec4f& v) const noexcept
	{
		return x >= v.x && y >= v.y && z >= v.z && w >= v.w;
	}

	/// Greater-than comparison operator.
	inline bool operator > (const vec4f& v) const noexcept
	{
		return x > v.x && y > v.y && z > v.z && w > v.w;
	}

	/// Less-than-or-equal-to comparison operator.
	inline bool operator <= (const vec4f& v) const noexcept
	{
		return x <= v.x && y <= v.y && z <= v.z && w <= v.w;
	}

	/// Addition operator.
	inline vec4f operator + (const vec4f& v) const noexcept
	{
		return _mm_add_ps(data, v.data);
	}

	/// Addition operator that first builds a vec4f.
	inline vec4f operator + (const float s) const noexcept
	{
		return _mm_add_ps(data, _mm_set1_ps(s));
	}

	/// Addition operator that first builds a vec4f (from the left).
	inline friend vec4f operator + (const float s, const vec4f& v) 
		noexcept
	{
		return _mm_add_ps(_mm_set1_ps(s), v.data);
	}

	/// "Take the negative" operator.
	inline vec4f operator -() const noexcept
	{
		return _mm_mul_ps(_mm_set1_ps(-1.0f), data);
	}

	/// Subtraction operator.
	inline vec4f operator - (const vec4f& v) const noexcept
	{
		return _mm_sub_ps(data, v.data);
	}

	/// Subtraction operator that first builds a vec4f.
	inline vec4f operator - (const float s) const noexcept
	{
		return _mm_sub_ps(data, _mm_set1_ps(s));
	}

	/// Subtraction operator that first builds a vec4f (from the left).
	inline friend vec4f operator - (const float s, const vec4f& v) 
		noexcept
	{
		return _mm_sub_ps(_mm_set1_ps(s), v.data);
	}

	/// Pointwise-multiplication operator.
	inline vec4f operator * (const vec4f& v) const noexcept
	{
		return _mm_mul_ps(data, v.data);
	}

	/// Pointwise-multiplication operator.
	inline vec4f operator * (const float s) const noexcept
	{
		return _mm_mul_ps(data, _mm_load1_ps(&s));
	}

	/// Pointwise-multiplication operator (from the left).
	friend vec4f operator * (const float lhs, const vec4f& rhs) 
		noexcept;

	/// Division by scalar operator.
	inline vec4f operator / (float s) const noexcept
	{
		s = 1.0f / s;
		return operator*(s);
	}

	/// The dot product (also called inner product).
	inline friend float dot(const vec4f& lhs, const vec4f& rhs) 
		noexcept
	{
		#if GINTONIC_SSE_VERSION >= 41

			return _mm_cvtss_f32(_mm_dp_ps(lhs.data, rhs.data, 0xffffffff));

		#elif GINTONIC_SSE_VERSION >= 30

			const auto product = _mm_mul_ps(lhs.data, rhs.data);
			const auto halfsum = _mm_hadd_ps(product, product);
			return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));

		#endif
	}

	/// Get the squared length.
	inline float length2() const noexcept
	{
		return dot(*this, *this);
	}

	/// Get the length.
	inline float length() const noexcept
	{
		return std::sqrt(length2());
	}

	/// Normalize this vec4f to unit length.
	inline vec4f& normalize() noexcept
	{
		return operator/=(length());
	}

	/// Pointwise convert radians to degrees.
	inline friend vec4f deg2rad(const vec4f& degrees) 
		noexcept
	{
		return _mm_mul_ps(degrees.data, _mm_set1_ps(0.0174532925f));
	}

	/// Pointwise convert degrees to radians.
	inline friend vec4f rad2deg(const vec4f& radians) 
		noexcept
	{
		return _mm_mul_ps(radians.data, _mm_set1_ps(57.2957795f));
	}

	/// Convert a vec4f to an FbxVector4 with a static_cast.
	operator FBXSDK_NAMESPACE::FbxVector4() const noexcept;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

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

//!@cond
// Implementation needs to be outside the class for MSVC.
inline vec4f operator * (const float lhs, const vec4f& rhs) 
	noexcept
{
	return _mm_mul_ps(_mm_load1_ps(&lhs), rhs.data);
}
//!@endcond

/// Get the squared distance between two vectors.
inline float distance2(const vec4f& u, const vec4f& v) 
	noexcept
{
	return (u-v).length2();
}

/// Get the distance between two vectors.
inline float distance(const vec4f& u, const vec4f& v) 
	noexcept
{
	return (u-v).length();
}

/// Mix two vectors.
inline vec4f mix(const vec4f& u, const vec4f& v, const float a)
	noexcept
{
	return (1.0f - a) * u + a * v;
}

/**
 * @brief Project a vector onto another vector.
 * @details The vector v needs to be of unit length.
 * 
 * @param u The vector to project.
 * @param v The vector to project onto. v needs to be of unit length.
 * 
 * @return `dot(u,v) * v`
 */
inline vec4f project(const vec4f& u, const vec4f& v) noexcept
{
	return dot(u,v) * v;
}

/**
 * @brief Reject a vector off another vector.
 * @details The vector v needs to be of unit length.
 * 
 * @param u The vector to reject.
 * @param v The vector to reject off. v needs to be of unit length.
 * 
 * @return `u - project(u, v)`
 */
inline vec4f reject(const vec4f& u, const vec4f& v) noexcept
{
	return u - project(u, v);
}

/**
 * @brief reflect a vector around another vector.
 * @details The vector v needs to be of unit length.
 * 
 * @param u The vector to reflect.
 * @param v The vector to reflect around. v needs to be of unit length.
 * 
 * @return `2.0f * project(u,v) - v`
 */
inline vec4f reflect(const vec4f& u, const vec4f& v) noexcept
{
	return 2.0f * project(u,v) - v;
}

/// Output stream support for vectors.
inline std::ostream& operator << (std::ostream& os, const vec4f& v) 
	noexcept
{
	return os << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
}

/// Input stream support for vectors.
inline std::istream& operator >> (std::istream& is, vec4f& v) 
	noexcept
{
	is >> v.x >> v.y >> v.z >> v.w;
	return is;
}

/**
 * @brief Check wether two vectors are almost equal.
 * 
 * @param u Some vector.
 * @param v Another vector.
 * @param units_in_last_place The number of units in the last place. This
 * determines the precision of the measurement.
 * @return True if the two vectors are almost equal, false otherwise.
 */
inline bool almost_equal(const vec4f& u, const vec4f& v, 
	const int units_in_last_place)
{
	return almost_equal(0.0f, distance(u,v), units_in_last_place);
}

} // namespace gintonic

// This macro is needed for boost::serialization because boost::serialization
// does not automatically assume unions are serializable.
BOOST_CLASS_IMPLEMENTATION(gintonic::vec4f, 
	boost::serialization::object_serializable);
