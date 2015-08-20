#ifndef gintonic_utilities_hpp
#define gintonic_utilities_hpp

#include "simd.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <memory>
#include <iostream>
#include <boost/config.hpp>

#define GINTONIC_SSE_VERSION 30

#define GINTONIC_NAMESPACE_FBX fbxsdk_2015_1

/*****************************************************************************
* GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE                                *
*                                                                            *
* Purpose: Convenience macro to define a class' new and delete operators     *
*          so that they are aligned on the given boundary. Every class that  *
*          has a vec*f as data member needs to define this macro in the      *
*          class definition with a value of 16.                              *
*****************************************************************************/
#define GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(alignment)               \
inline static void* operator new(std::size_t count)                          \
{                                                                            \
	return _mm_malloc(count, alignment);                                     \
}                                                                            \
inline static void operator delete(void* ptr)                                \
{                                                                            \
	_mm_free(ptr);                                                           \
}                                                                            \
inline static void* operator new[](std::size_t count)                        \
{                                                                            \
	return _mm_malloc(count, alignment);                                     \
}                                                                            \
inline static void operator delete[](void* ptr)                              \
{                                                                            \
	_mm_free(ptr);                                                           \
}                                                                            \
inline static void* operator new(std::size_t count, void* ptr)               \
{                                                                            \
	return ptr;                                                              \
}                                                                            \
inline static void operator delete(void* ptr, std::size_t count)             \
{                                                                            \
	_mm_free(ptr);                                                           \
}                                                                            \
inline static void operator delete[](void* ptr, std::size_t count)           \
{                                                                            \
	_mm_free(ptr);                                                           \
}

namespace gintonic {

/*****************************************************************************
* gintonic::almost_equal                                                     *
*                                                                            *
* Purpose: Check wether two floats are almost equal.                         *
*****************************************************************************/

template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
inline almost_equal(const T x, const T y, int units_in_last_place)
{
	return std::abs(x - y) < std::numeric_limits<T>::epsilon()
		* std::abs(x + y) * units_in_last_place
		|| std::abs(x - y) < std::numeric_limits<T>::min();
}

/*****************************************************************************
* gintonic::dot                                                              *
*                                                                            *
* Purpose: General dot product for SIMD types.                               *
*****************************************************************************/

inline float dot(const __m128& lhs, const __m128& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
	#if GINTONIC_SSE_VERSION >= 41

		return _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0xffffffff));

	#elif GINTONIC_SSE_VERSION >= 30

		const auto product = _mm_mul_ps(lhs, rhs);
		const auto halfsum = _mm_hadd_ps(product, product);
		return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));

	#else

		#error Make sure to define GINTONIC_SSE_VERSION

	#endif
}

/*****************************************************************************
* gintonic::deg2rad and gintonic::rad2deg                                    *
*                                                                            *
* Purpose: Convert between radians and degrees.                              *
*****************************************************************************/

template <class T> T deg2rad(const T t)
{
	return T(0.0174532925) * t;
}

template <class T> T rad2deg(const T t)
{
	return T(57.2957795) * t;
}

} // namespace gintonic

#endif