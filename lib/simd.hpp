/**
 * @file simd.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_simd_hpp
#define gintonic_simd_hpp

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

#define _mm_replicate_x_ps(v) _mm_shuffle_ps((v), (v), _MM_SHUFFLE(0,0,0,0))
#define _mm_replicate_y_ps(v) _mm_shuffle_ps((v), (v), _MM_SHUFFLE(1,1,1,1))
#define _mm_replicate_z_ps(v) _mm_shuffle_ps((v), (v), _MM_SHUFFLE(2,2,2,2))
#define _mm_replicate_w_ps(v) _mm_shuffle_ps((v), (v), _MM_SHUFFLE(3,3,3,3))

#endif