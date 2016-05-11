/**
 * @file vec3f.hpp
 * @brief Defines a three-dimensional vector class.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/utilities.hpp"
#include "../Foundation/Profiler.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <fbxsdk/fbxsdk_nsbegin.h>
class FbxVector4; // Forward declaration.	
#include <fbxsdk/fbxsdk_nsend.h>

namespace gintonic {

union vec2f; // Forward declaration.
union quatf; // Forward declaration.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

/**
 * @brief Three-dimensional vector class that uses SSE.
 * 
 * @details The fact that this class carries an SSE type (namely, __m128) has
 * some consequences for classes that want to use this class as a datamember.
 * The SSE types need to be aligned to a 16-byte memory boundary. This is
 * done automatically by the compiler when you instantiate an object carrying
 * an SSE class on the program stack. It is **not done automatically** by the
 * compiler when you instantiate such a class **on the heap**. For this
 * reason, you **must** declare the macro
 * `

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();`
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
union alignas(16) vec3f
{
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
		//!@cond
		float dummy;
		//!@endcond
	};

	/// Default constructor.
	inline vec3f() noexcept
	: dummy(0.0f)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Constructor that sets every coordinate to the given value.
	inline vec3f(const float s) noexcept
	: data(_mm_set1_ps(s))
	{
		GT_PROFILE_FUNCTION;
	}

	/// Constructor.
	inline vec3f(const float a, const float b, const float c) 
		noexcept
	: x(a), y(b), z(c), dummy(0.0f)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Constructor for a raw SSE value.
	inline vec3f(__m128 values) noexcept : data(values)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Initializer list constructor.
	vec3f(std::initializer_list<float> init) noexcept;

	/// Copy constructor.
	inline vec3f(const vec3f& v) noexcept : data(v.data)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Move constructor.
	inline vec3f(vec3f&& v) noexcept : data(v.data)
	{
		GT_PROFILE_FUNCTION;
	}

	/// Constructor.
	vec3f(const vec2f& v, const float z) noexcept;

	/// Constructor. The quaternion needs to be a pure quaternion.
	vec3f(const quatf&) noexcept;

	/// Copy assignment operator.
	inline vec3f& operator = (const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = v.data;
		return *this;
	}

	/// Move assignment operator.
	inline vec3f& operator = (vec3f&& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = v.data;
		return *this;
	}

	/// Initializer list assignment operator.
	vec3f& operator=(std::initializer_list<float> init) noexcept;

	/// Constructor that takes an FbxVector4.
	vec3f(const FBXSDK_NAMESPACE::FbxVector4& v) noexcept;

	/// Assignment operator that takes an FbxVector4.
	vec3f& operator=(const FBXSDK_NAMESPACE::FbxVector4& v) noexcept;

	/// Add-and-assign operator.
	inline vec3f& operator += (const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_add_ps(data, v.data);
		return *this;
	}

	/// Add-and-assign operator that first builds a vec3f.
	inline vec3f& operator += (const float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_add_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Subtract-and-assign operator.
	inline vec3f& operator -= (const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_sub_ps(data, v.data);
		return *this;
	}

	/// Subtract-and-assign operator that first builds a vec3f.
	inline vec3f& operator -= (const float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_sub_ps(data, _mm_set1_ps(s));
		return *this;
	}

	/// Pointwise-multiply-and-assign operator.
	inline vec3f& operator *= (const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_mul_ps(data, v.data);
		return *this;
	}

	/// Pointwise-multiply-and-assign operator.
	inline vec3f& operator *= (const float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_mul_ps(data, _mm_load1_ps(&s));
		return *this;
	}

	/// Pointwise-divide-and-assign operator.
	inline vec3f& operator /= (float s) noexcept
	{
		GT_PROFILE_FUNCTION;

		s = 1.0f / s;
		return operator*=(s);
	}

	/// Equality comparison operator.
	inline bool operator == (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return (_mm_movemask_ps(_mm_cmpeq_ps(data, v.data)) & 0x7) == 0x7;

		// return x == v.x && y == v.y && z == v.z;
	}

	/// Inequality comparison operator.
	inline bool operator != (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return !operator==(v);
	}

	/// Less-than comparison operator.
	inline bool operator < (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x < v.x && y < v.y && z < v.z;
	}

	/// Greater-than-or-equal-to comparison operator.
	inline bool operator >= (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x >= v.x && y >= v.y && z >= v.z;
	}

	/// Greater-than comparison operator.
	inline bool operator > (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return x > v.x && y > v.y && z > v.z;
	}

	/// Less-than-or-equal-to comparison operator.
	inline bool operator <= (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return (_mm_movemask_ps(_mm_cmple_ps(data, v.data)) & 0x7) == 0x7;

		// return x <= v.x && y <= v.y && z <= v.z;
	}

	/// Addition operator.
	inline vec3f operator + (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		#ifdef BOOST_MSVC

		auto m1 = v.data;
		auto m2 = this->data;
		return _mm_add_ps(m1, m2);

		#else

		auto m1 = v.data;
		auto m2 = this->data;

		// return _mm_add_ps(data, v.data);
		return _mm_add_ps(m1, m2);

		#endif
	}

	/// Addition operator that first builds a vec3f.
	inline vec3f operator + (const float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_add_ps(data, _mm_set1_ps(s));
	}

	/// Addition operator that first builds a vec3f (from the left).
	inline friend vec3f operator + (const float s, const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_add_ps(_mm_set1_ps(s), v.data);
	}

	/// "Take the negative" operator.
	inline vec3f operator -() const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_xor_ps(data, _mm_set1_ps(-0.0));

		// This is very slow!
		// return _mm_mul_ps(_mm_set1_ps(-1.0f), data);
	}

	/// Subtraction operator.
	inline vec3f operator - (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		#ifdef BOOST_MSVC

		const auto copy = v.data;
		return _mm_sub_ps(data, copy);

		#else 

		return _mm_sub_ps(data, v.data);

		#endif
	}

	/// Subtraction operator that first builds a vec3f.
	inline vec3f operator - (const float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_sub_ps(data, _mm_set1_ps(s));
	}

	/// Subtraction operator that first builds a vec3f (from the left).
	inline friend vec3f operator - (const float s, const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_sub_ps(_mm_set1_ps(s), v.data);
	}

	/// Pointwise-multiplication operator.
	inline vec3f operator * (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(data, v.data);
	}

	/// Pointwise-multiplication operator.
	inline vec3f operator * (const float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(data, _mm_set1_ps(s));
	}

	/// Pointwise-multiplication operator (from the left).
	friend vec3f operator * (const float lhs, const vec3f& rhs) noexcept;

	/// Pointwise division by another vector.
	inline vec3f operator / (const vec3f& v) const noexcept
	{
		GT_PROFILE_FUNCTION;

		vec3f lResult(_mm_div_ps(data, v.data));
		lResult.dummy = 0.0f;
		return lResult;
	}

	/// Pointwise divide-and-assign operator.
	inline vec3f operator /= (const vec3f& v) noexcept
	{
		GT_PROFILE_FUNCTION;

		data = _mm_div_ps(data, v.data);
		dummy = 0.0f;
		return *this;
	}

	/// Division by scalar operator.
	inline vec3f operator / (float s) const noexcept
	{
		GT_PROFILE_FUNCTION;

		s = 1.0f / s;
		return operator*(s);
	}

	/// The dot product (also called inner product).
	inline friend float dot(const vec3f& lhs, const vec3f& rhs) noexcept
	{
		GT_PROFILE_FUNCTION;

		#if GINTONIC_SSE_VERSION >= 41

			return _mm_cvtss_f32(_mm_dp_ps(lhs.data, rhs.data, 0xffffffff));

		#elif GINTONIC_SSE_VERSION >= 30

			const auto product = _mm_mul_ps(lhs.data, rhs.data);
			const auto halfsum = _mm_hadd_ps(product, product);
			return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));

		#endif
	}

	/// The cross product (also called outer product).
	inline friend vec3f cross(const vec3f& lhs, const vec3f& rhs)
	{
		GT_PROFILE_FUNCTION;

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

	/// Get the squared length.
	inline float length2() const noexcept
	{
		GT_PROFILE_FUNCTION;

		return dot(*this, *this);
	}

	/// Get the length.
	inline float length() const noexcept
	{
		GT_PROFILE_FUNCTION;

		return std::sqrt(length2());
	}

	/// Normalize this vec3f to unit length.
	inline vec3f& normalize() noexcept
	{
		GT_PROFILE_FUNCTION;

		return operator/=(length());
	}

	/// Pointwise convert radians to degrees.
	inline friend vec3f deg2rad(const vec3f& degrees) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(degrees.data, _mm_set1_ps(0.0174532925f));
	}

	/// Pointwise convert degrees to radians.
	inline friend vec3f rad2deg(const vec3f& radians) noexcept
	{
		GT_PROFILE_FUNCTION;

		return _mm_mul_ps(radians.data, _mm_set1_ps(57.2957795f));
	}

	/// Convert a vec3f to an FbxVector4 with a static_cast.
	operator FBXSDK_NAMESPACE::FbxVector4() const noexcept;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	friend class boost::serialization::access;

	template <class Archive> 
	inline void serialize(Archive& ar, const unsigned int /*version*/)
	{
		GT_PROFILE_FUNCTION;
		
		ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) 
			& BOOST_SERIALIZATION_NVP(z);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//!@cond
// Implementation needs to be outside the class for MSVC.
inline vec3f operator * (const float lhs, const vec3f& rhs) 
	noexcept
{
	return _mm_mul_ps(_mm_load1_ps(&lhs), rhs.data);
}
//!@endcond

/// Get the squared distance between two vectors.
inline float distance2(const vec3f& u, const vec3f& v) 
	noexcept
{
	return (u-v).length2();
}

/// Get the distance between two vectors.
inline float distance(const vec3f& u, const vec3f& v) 
	noexcept
{
	return (u-v).length();
}

/// Mix two vectors.
inline vec3f mix(const vec3f& u, const vec3f& v, const float a) 
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
inline vec3f project(const vec3f& u, const vec3f& v) noexcept
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
inline vec3f reject(const vec3f& u, const vec3f& v) noexcept
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
inline vec3f reflect(const vec3f& u, const vec3f& v) noexcept
{
	return 2.0f * project(u,v) - v;
}

/// Output stream support for vectors.
inline std::ostream& operator << (std::ostream& os, const vec3f& v) 
	noexcept
{
	return os << v.x << ' ' << v.y << ' ' << v.z;
}

/// Input stream support for vectors.
inline std::istream& operator >> (std::istream& is, vec3f& v) 
	noexcept
{
	is >> v.x >> v.y >> v.z;
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
inline bool almost_equal(const vec3f& u, const vec3f& v, 
	const int units_in_last_place)
{
	return almost_equal(0.0f, distance(u,v), units_in_last_place);
}

} // namespace gintonic

//!@cond
// This macro is needed for boost::serialization because boost::serialization
// does not automatically assume unions are serializable.
BOOST_CLASS_IMPLEMENTATION(gintonic::vec3f, 
	boost::serialization::object_serializable);
//!@endcond
