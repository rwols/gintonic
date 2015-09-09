/**
 * @file lib/utilities.hpp
 * @brief Various utility functions and macros.
 * @author Raoul Wols
 */

#ifndef gintonic_utilities_hpp
#define gintonic_utilities_hpp

#include "simd.hpp"
#include <cmath>
#include <limits>
#include <memory>
#include <iostream>
#include "config.hpp"

#ifdef BOOST_MSVC

	/**
	 * @brief Macro to make a type aligned. Can be used for classes.
	 * 
	 * @param x The memory boundary alignment. Usual values are 16 or 128.
	 */
	#define GINTONIC_ALIGNED(x) __declspec(align(x))

#else

	/**
	 * @brief Macro to make a type aligned. Can be used for classes.
	 * 
	 * @param x The memory boundary alignment. Usual values are 16 or 128.
	 */
	#define GINTONIC_ALIGNED(x) __attribute__ ((aligned(x)))

#endif

/**
 * @brief Convenience macro to define custom operator new / operator delete
 * for your class to get your class aligned on a memory boundary.
 * 
 * @param alignment The memory boundary alignment. Usual values are 16 or 128.
 * @return [description]
 */
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

/**
 * @brief The memory boundary of an SSE type.
 */
#define GINTONIC_SSE_ALIGNMENT 16

/**
 * @brief Convenience macro to use if you have SSE types in your class.
 */
#define GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE() \
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(GINTONIC_SSE_ALIGNMENT)

/**
 * @brief The memory boundary of a cache line. This is experimental.
 */
#define GINTONIC_CACHE_LINE 128

/**
 * @brief Convenience macro to use if you need cache alignment.
 * Experimental and untested.
 */
#define GINTONIC_DEFINE_CACHE_LINE_OPERATOR_NEW_DELETE() \
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(GINTONIC_CACHE_LINE)

namespace gintonic {

/**
 * @brief Check if two arithmetic types are almost equal.
 * 
 * @tparam T Any arithmetic type. The type T must have support for
 * `std::numeric_limits` because we need the `epsilon` and the `min`.
 * @param x Some arithmetic type.
 * @param y Another arithmetic type.
 * @param units_in_last_place The number of units the in the last place
 * determines the precision of this function. The higher this number, the
 * more stringent this function is.
 * @return True if `x` and `y` are almost equal, false if not.
 */
template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
inline almost_equal(const T x, const T y, int units_in_last_place)
{
	return std::abs(x - y) < std::numeric_limits<T>::epsilon()
		* std::abs(x + y) * units_in_last_place
		|| std::abs(x - y) < std::numeric_limits<T>::min();
}

/**
 * @brief General purpose dot product of two SSE types.
 * 
 * @param lhs An SSE value.
 * @param rhs Another SSE value.
 * 
 * @return The dot product of `lhs` and `rhs`.
 */
inline float dot(const __m128& lhs, const __m128& rhs) 
	BOOST_NOEXCEPT_OR_NOTHROW
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

/**
 * @brief Convert a floating point number from radians to degrees.
 * 
 * @param t The floating point number in radians.
 * @return The floating point number in degrees.
 */
inline float deg2rad(const float t)
{
	return float(0.0174532925) * t;
}

/**
 * @brief Convert a floating point number from degrees to radians.
 * 
 * @param t The floating point number in degrees.
 * @return The floating point number in radians.
 */
inline float rad2deg(const float t)
{
	return float(57.2957795) * t;
}

} // namespace gintonic

#endif