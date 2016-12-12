/**
 * @file simd.hpp
 * @brief Include this file if you need to use SSE types.
 * @author Raoul Wols
 */

#pragma once

#if defined(_MSC_VER)
	/* Microsoft C/C++-compatible compiler */
	#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
	/* GCC-compatible compiler, targeting x86/x86-64 */
	#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
	/* GCC-compatible compiler, targeting ARM with NEON */
	#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
	/* GCC-compatible compiler, targeting ARM with WMMX */
	#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) \
	&& (defined(__VEC__) || defined(__ALTIVEC__))
	/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
	#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
	/* GCC-compatible compiler, targeting PowerPC with SPE */
	#include <spe.h>
#endif

#include "config.hpp"
#include <iosfwd>
#include <cstdint>

#define _mm_shuffle(v,a,b,c,d) _mm_shuffle_ps((v), (v), _MM_SHUFFLE(a,b,c,d))

/**
 * @brief Broadcast the first coordinate to all four registers.
 * @param v The SSE register.
 * @return A new SSE type that contains only the first coordinate
 * in all of its four registers.
 */
#define _mm_replicate_x_ps(v) _mm_shuffle((v),0,0,0,0)

 /**
 * @brief Broadcast the second coordinate to all four registers.
 * @param v The SSE register.
 * @return A new SSE type that contains only the second coordinate
 * in all of its four registers.
 */
#define _mm_replicate_y_ps(v) _mm_shuffle((v),1,1,1,1)

  /**
 * @brief Broadcast the third coordinate to all four registers.
 * @param v The SSE register.
 * @return A new SSE type that contains only the third coordinate
 * in all of its four registers.
 */
#define _mm_replicate_z_ps(v) _mm_shuffle((v),2,2,2,2)

  /**
 * @brief Broadcast the fourth coordinate to all four registers.
 * @param v The SSE register.
 * @return A new SSE type that contains only the fourth coordinate
 * in all of its four registers.
 */
#define _mm_replicate_w_ps(v) _mm_shuffle((v),3,3,3,3)

#define _mm_shift_0_left(v) _mm_shuffle((v),3,2,1,0)
#define _mm_shift_1_left(v) _mm_shuffle((v),0,3,2,1)
#define _mm_shift_2_left(v) _mm_shuffle((v),1,0,3,2)
#define _mm_shift_3_left(v) _mm_shuffle((v),2,1,0,3)
#define _mm_shift_0_right(v) _mm_shift_0_left((v))
#define _mm_shift_1_right(v) _mm_shift_3_left((v))
#define _mm_shift_2_right(v) _mm_shift_2_left((v))
#define _mm_shift_3_right(v) _mm_shift_1_left((v))

#define _mm_negate(v) _mm_xor_ps((v), _mm_set1_ps(-0.0f))

// #ifdef BOOST_MSVC

#define _mm_x000_ps(v) _mm_and_ps((v),                   _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0)))
#define _mm_0x00_ps(v) _mm_and_ps(_mm_replicate_x_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0)))
#define _mm_00x0_ps(v) _mm_and_ps(_mm_replicate_x_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0)))
#define _mm_000x_ps(v) _mm_and_ps(_mm_replicate_x_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff)))

#define _mm_y000_ps(v) _mm_and_ps(_mm_replicate_y_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0)))
#define _mm_0y00_ps(v) _mm_and_ps((v),                   _mm_castsi128_ps(_mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0)))
#define _mm_00y0_ps(v) _mm_and_ps(_mm_replicate_y_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0)))
#define _mm_000y_ps(v) _mm_and_ps(_mm_replicate_y_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff)))

#define _mm_z000_ps(v) _mm_and_ps(_mm_replicate_z_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0)))
#define _mm_0z00_ps(v) _mm_and_ps(_mm_replicate_z_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0)))
#define _mm_00z0_ps(v) _mm_and_ps((v),                   _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0)))
#define _mm_000z_ps(v) _mm_and_ps(_mm_replicate_z_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff)))

#define _mm_w000_ps(v) _mm_and_ps(_mm_replicate_w_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0)))
#define _mm_0w00_ps(v) _mm_and_ps(_mm_replicate_w_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0)))
#define _mm_00w0_ps(v) _mm_and_ps(_mm_replicate_w_ps(v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0)))
#define _mm_000w_ps(v) _mm_and_ps((v),                   _mm_castsi128_ps(_mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff)))

#define _mm_0yz0_ps(v) _mm_and_ps((v), _mm_castsi128_ps(_mm_setr_epi32(0x0, 0xffffffff, 0xffffffff, 0x0)))
#define _mm_x0z0_ps(v) _mm_and_ps((v), _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0x0, 0xffffffff, 0x0)))
#define _mm_xy00_ps(v) _mm_and_ps((v), _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0xffffffff, 0x0, 0x0)))

// #else // BOOST_MSVC

// #define _mm_x000_ps(v) _mm_and_ps((v),                   _mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0))
// #define _mm_0x00_ps(v) _mm_and_ps(_mm_replicate_x_ps(v), _mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0))
// #define _mm_00x0_ps(v) _mm_and_ps(_mm_replicate_x_ps(v), _mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0))
// #define _mm_000x_ps(v) _mm_and_ps(_mm_replicate_x_ps(v), _mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff))

// #define _mm_y000_ps(v) _mm_and_ps(_mm_replicate_y_ps(v), _mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0))
// #define _mm_0y00_ps(v) _mm_and_ps((v),                   _mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0))
// #define _mm_00y0_ps(v) _mm_and_ps(_mm_replicate_y_ps(v), _mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0))
// #define _mm_000y_ps(v) _mm_and_ps(_mm_replicate_y_ps(v), _mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff))

// #define _mm_z000_ps(v) _mm_and_ps(_mm_replicate_z_ps(v), _mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0))
// #define _mm_0z00_ps(v) _mm_and_ps(_mm_replicate_z_ps(v), _mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0))
// #define _mm_00z0_ps(v) _mm_and_ps((v),                   _mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0))
// #define _mm_000z_ps(v) _mm_and_ps(_mm_replicate_z_ps(v), _mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff))

// #define _mm_w000_ps(v) _mm_and_ps(_mm_replicate_w_ps(v), _mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0))
// #define _mm_0w00_ps(v) _mm_and_ps(_mm_replicate_w_ps(v), _mm_setr_epi32(0x0, 0xffffffff, 0x0, 0x0))
// #define _mm_00w0_ps(v) _mm_and_ps(_mm_replicate_w_ps(v), _mm_setr_epi32(0x0, 0x0, 0xffffffff, 0x0))
// #define _mm_000w_ps(v) _mm_and_ps((v),                   _mm_setr_epi32(0x0, 0x0, 0x0, 0xffffffff))

// #define _mm_0yz0_ps(v) _mm_and_ps((v), _mm_setr_epi32(0x0, 0xffffffff, 0xffffffff, 0x0))
// #define _mm_x0z0_ps(v) _mm_and_ps((v), _mm_setr_epi32(0xffffffff, 0x0, 0xffffffff, 0x0))
// #define _mm_xy00_ps(v) _mm_and_ps((v), _mm_setr_epi32(0xffffffff, 0xffffffff, 0x0, 0x0))

// #endif // BOOST_MSVC

namespace gintonic {

template <class T>
inline bool isAligned(T const * const ptr, std::size_t alignment = alignof(T))
{
    return 0 == reinterpret_cast<std::uintptr_t>(ptr) % alignment;
}

std::ostream& operator << (std::ostream&, const __m128&);

} // namespace gintonic

