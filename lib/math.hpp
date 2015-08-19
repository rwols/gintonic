/**
 * REMARKS
 *
 * This file has become waaay to big. Worse, I wrote this a long time ago and
 * now feel that most of the interface and design totally sucks. Everything
 * should be rewritten from the ground up with SIMD in mind. The FBX SDK
 * should not be included here. The basic vector class is templated, but I
 * don't think there's ever a use for a vec<long long, 6>, to name something
 * pointless.
 * The basic types should be limited to vec2, vec3, vec4, mat2, mat3, mat4,
 * quat and optionally perhaps integer variants if we're going to rewrite.
 */

#ifndef math_hpp
#define math_hpp

#define _USE_MATH_DEFINES
#include <cmath>
#include <array>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

#include <boost/config.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/geometry.hpp>

#ifdef BOOST_MSVC
	#include <intrin.h>
#else
	#include <x86intrin.h>
#endif

#include "profiler.hpp"

/*****************************************************************************
 * GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE                               *
 *                                                                           *
 * Purpose: Convenience macro to define a class' new and delete operators    *
 *          so that they are aligned on the given boundary. Every class that *
 *          has a vec4f as data member needs to define this macro in the     *
 *          class definition with a value of 16.                             *
 ****************************************************************************/
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

/*****************************************************************************
 * We include the FBX SDK at this point. The header file has a bunch of      *
 * warnings if we compile with clang, so we explicitly disable those here.   *
 ****************************************************************************/
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdocumentation"
	#pragma clang diagnostic ignored "-Wgnu"
	#pragma clang diagnostic ignored "-Wextra-semi"
	#pragma clang diagnostic ignored "-Wignored-qualifiers"
	#pragma clang diagnostic ignored "-Wunused-parameter"
	#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
#define FBXSDK_NEW_API
#include <fbxsdk.h>
#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wgnu"
	#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace gintonic {

/*****************************************************************************
 * gintonic::allocator                                                       *
 *                                                                           *
 * Purpose: To be able to put SIMD types in containers.                      *
 ****************************************************************************/
template <class T, std::size_t Alignment = 16> class allocator
{
public:
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef std::size_t size_type;
	typedef ptrdiff_t difference_type;

	inline T* address(T& t) const
	{
		return std::addressof(t);
	}

	inline const T * address(const T& t) const
	{
		return std::addressof(t);
	}

	inline std::size_t max_size() const
	{
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<std::size_t>(0) - static_cast<std::size_t>(1)) 
			/ sizeof(T);
	}

	// What follows is always the same for all allocators.
	template <typename U> struct rebind
	{
		typedef allocator<U, Alignment> other;
	};

	inline bool operator!=(const allocator& other) const
	{
		return !(*this == other);
	}

	void construct(T* const p, const T& t) const
	{
		void* const pv = static_cast<void*>(p);
		new (pv) T(t);
	}

	inline void destroy(T * const p) const
	{
		p->~T();
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	inline bool operator==(const allocator& other) const
	{
		return true;
	}


	// Default constructor, copy constructor, rebinding constructor, and
	// destructor. Empty for stateless allocators.
	inline allocator() = default;

	inline allocator(const allocator&) = default;

	template <typename U> inline allocator(const allocator<U, Alignment>&) {}

	inline ~allocator() {}


	// The following will be different for each allocator.
	T* allocate(const std::size_t n) const
	{
		// The return value of allocate(0) is unspecified.
		// Mallocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		if (n == 0) 
		{
			return nullptr;
		}

		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		if (n > max_size())
		{
			throw std::length_error(
				"gintonic::allocator<T>::allocate() - Integer overflow.");
		}

		// Mallocator wraps malloc().
		void* const pv = _mm_malloc(n * sizeof(T), Alignment);

		// Allocators should throw std::bad_alloc in the case of memory 
		// allocation failure.
		if (pv == nullptr)
		{
			throw std::bad_alloc();
		}

		return static_cast<T*>(pv);
	}

	inline void deallocate(T* const p, const std::size_t n) const
	{
		_mm_free(p);
	}


	// The following will be the same for all allocators that ignore hints.
	template <typename U>
	inline T* allocate(const std::size_t n, const U* /* const hint */) const
	{
		return allocate(n);
	}

	allocator& operator=(const allocator&) = delete;
};

/*!
\brief Computes the sine.
\tparam T The arithmetic type.
\param[in] angle The angle to compute the sine of.
\return The sine of the given angle.
\sa cos(const T angle)
\sa sincos(T in_angle, T& out_sin, T& out_cos)
*/
template <class T> inline T sin(const T angle) BOOST_NOEXCEPT_OR_NOTHROW
{
	return ::std::sin(angle);
}

/*!
\brief Computes the cosine.
\tparam T The arithmetic type.
\param[in] angle The angle to compute the cosine of.
\return The cosine of the given angle.
\sa sin(const T angle)
\sa sincos(T in_angle, T& out_sin, T& out_cos)
*/
template <class T> inline T cos(const T angle) BOOST_NOEXCEPT_OR_NOTHROW
{
	return ::std::cos(angle);
}

/*!
\brief Computes the sine and the cosine of the given angle.
\tparam T The arithmetic type.
\param[in] in_angle The angle to compute the sine and the cosine of.
\param[out] out_sin This variable will hold the computed sine value.
\param[out] out_cos This variable will hold the computed cosine value.
\sa sin(const T angle)
\sa cos(const T angle)
*/
template <class T> inline void sincos(const T in_angle, T& out_sin, T& out_cos) BOOST_NOEXCEPT_OR_NOTHROW
{
	PROFILE("sincos")
	out_sin = sin(in_angle);
	out_cos = cos(in_angle);
}

/*!
\brief Computes the square root of the given value.
\tparam T The arithmetic type.
\param[in] value The value to compute the square root of.
\return The square root of the given value.
*/
template <class T> inline T sqrt(const T value) BOOST_NOEXCEPT_OR_NOTHROW
{
	PROFILE("sqrt")
	return ::std::sqrt(value);
}

/*!
\brief Computes the radians, given the degrees.
\tparam T The arithmetic type.
\param[in] degrees The angle in degrees.
\return The angle in radians.
\sa rad_to_deg(const T radians)
*/
template <class T> inline T deg_to_rad(const T degrees) BOOST_NOEXCEPT_OR_NOTHROW
{
	PROFILE("deg_to_rad")
	return T(0.0174532925) * degrees;
}

/*!
\brief Computes the degrees, given the radians.
\tparam T The arithmetic type.
\param[in] radians The angle in radians.
\return The angle in degrees.
\sa deg_to_rad(const T degrees)
*/
template <class T> inline T rad_to_deg(const T radians) BOOST_NOEXCEPT_OR_NOTHROW
{
	PROFILE("rad_to_deg")
	return 57.2957795 * radians;
}

/*!
\brief Computes spherical coordinates, given cartesian coordinates.
\tparam T The arithmetic type.
\param[in] x The x-coordinate of the cartesian coordinate.
\param[in] y The y-coordinate of the cartesian coordinate.
\param[in] z The z-coordinate of the cartesian coordinate.
\param[out] r The radius of the spherical coordinate.
\param[out] theta The azimuth of the spherical coordinate.
\param[out] phi The altitude of the spherical coordinate.
\sa spherical_to_cartesian(const T r, const T theta, const T phi, T& x, T& y, T& z)
*/
template <class T> inline void cartesian_to_spherical(const T x, const T y, const T z, T& r, T& theta, T& phi) BOOST_NOEXCEPT_OR_NOTHROW
{
	PROFILE("cartesian_to_spherical")
	r = sqrt(x*x + y*y + z*z);
	theta = std::acos(z/r);
	phi = std::atan(y/x);
}

/*!
\brief spherical_to_cartesian
\tparam T The arithmetic type.
\param[in] r The radius of the spherical coordinate.
\param[in] theta The azimuth of the spherical coordinate.
\param[in] phi The altitude of the spherical coordinate.
\param[out] x The x-coordinate of the cartesian coordinate.
\param[out] y The y-coordinate of the cartesian coordinate.
\param[out] z The z-coordinate of the cartesian coordinate.
\sa cartesian_to_spherical(const T x, const T y, const T z, T& r, T& theta, T& phi)
*/
template <class T> inline void spherical_to_cartesian(const T r, const T theta, const T phi, T& x, T& y, T& z) BOOST_NOEXCEPT_OR_NOTHROW
{
	PROFILE("spherical_to_cartesian")
	x = r * sin(theta) * cos(phi);
	y = r * sin(theta) * sin(phi);
	z = r * cos(phi);
}

/*!
\brief Output stream support for `FbxVector2`.
\param[in,out] os The output stream.
\param[in] v The vector to print.
\return The output stream.
\sa operator << (std::ostream& os, const FbxVector4& v)
\sa operator << (std::ostream& os, const FbxColor& c)
*/
std::ostream& operator << (std::ostream& os, const FbxVector2& v);

/*!
\brief Output stream support for `FbxVector4`.
\param[in,out] os The output stream.
\param[in] v The vector to print.
\return The output stream.
\sa operator << (std::ostream& os, const FbxVector2& v)
\sa operator << (std::ostream& os, const FbxColor& c)
*/
std::ostream& operator << (std::ostream& os, const FbxVector4& v);

/*!
\brief Output stream support for `FbxColor`.
\param[in,out] os The output stream.
\param[in] c The color to print.
\return The output stream.
\sa operator << (std::ostream& os, const FbxVector2& v)
\sa operator << (std::ostream& os, const FbxVector4& c)
*/
std::ostream& operator << (std::ostream& os, const FbxColor& c);

/*!
\brief A structure to represent an arbitrary dimensional vector of arbitrary type.

vec inherits from std::array. This is usually discouraged, since
std::array does not have a virtual destructor. However, vec has
no new data members defined.

\tparam T The arithmetic type.
\tparam N The dimension of the vector.
 */
template <class T, ::std::size_t N> struct vec : public ::std::array<T,N>
{

	/*!
	\brief Default constructor.
	*/
	vec() = default;

	/*!
	\brief Constructs a vec from an `std::array`.
	\param a The array.
	*/
	vec(const ::std::array<T,N>& a) : ::std::array<T,N>(a) {}

	/*!
	\brief Constructs a vec from an `std::array`.

	This is the move-constructor.

	\param a The array.
	*/
	vec(::std::array<T,N>&& a) : ::std::array<T,N>(std::move(a)) {}

	/*!
	\brief Constructor that takes two arbitrary input iterators.
	\tparam ForwardIter An iterator that supports incrementing and deferencing.
	\param first An iterator pointing to the first element.
	\param last An iterator pointing to the last element.
	*/
	template <class ForwardIter> vec(ForwardIter first,
									ForwardIter last)
	{
		::std::copy(first, last, this->begin());
	}

	/*!
	\brief Add-and-assign operator for vectors.
	\param v The vector to add-and-assign to this vector.
	\return This vector.
	*/
	template <class S>
	inline vec& operator += (const vec<S,N>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x + y; });
		return *this;
	}

	//! Substract-and-assign operator for vectors.
	template <class S>
	inline vec& operator -= (const vec<S,N>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x - y; });
		return *this;
	}

	//! Multiply-and-assign operator for vectors. Only works for scalars, of course.
	template <class S>
	inline vec& operator *= (const S s)
	{
		::std::transform(this->begin(), this->end(), this->begin(), [&s](T x) -> T { return s * x; });
		return *this;
	}

	//! Divide-and-assign operator for vectors. Only works for scalars, of course.
	template <class S>
	inline vec& operator /= (const S s)
	{
		return operator*=(S(1) / s);
	}

	//! Obtain a standard unit axis.
	/*
	 * Obtains a standard unit axis. For example, calling `vec<float,6>::Axis<2>`
	 * will get the third unit axis of six-dimensional Eucledean space.
	 */
	template <::std::size_t A> static inline vec<T,N> Axis()
	{
		vec<T,N> r;
		::std::fill(r.begin(), r.end(), T(0));
		r[A] = T(1);
		return r;
	}

	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version)
	{
		ar & *this;
	}
};

/*!
\brief Copies the contents of a vector into a raw array.
\tparam T The value type.
\tparam N The dimension of the vector (and the array).
\param array The raw array to copy the values into. Make sure that it is large enough to hold all `N` values.
\param the_vector The vector to copy the values from.
*/
template <class T, ::std::size_t N> inline void assign(T* array, const vec<T,N>& the_vector)
{
	::std::copy(the_vector.begin(), the_vector.end(), array);
}

/*!
\brief Check wether two values are almost equal.

This is used for floating point types. If the difference between `a` and `b` is less or
equal to the machine epsilon, then they are considered equal.

\tparam T The value type. Only makes sense if `T` is a floating point type.
\param a One value.
\param b Another value.
\return True if the difference between `a` and `b` is less than or equal to the machine epsilon,
false otherwise.
*/
template <class T> inline bool almost_equal(const T a, const T b)
{
	return b - a <= ::std::numeric_limits<T>::epsilon();
}

/*!
\brief Check wether two vectors are almost equal.
\tparam T The value type. Only makes sense if `T` is a floating point type.
\param a One vector.
\param b Another vector.
\return True if the difference between `a` and `b` is less than or equal to the machine epsilon,
false otherwise.
*/
template <class T, ::std::size_t N> inline bool almost_equal(const vec<T,N>& a, const vec<T,N>& b)
{
	for (::std::size_t i = 0; i < N; ++i)
	{
		if (!almost_equal(a[i], b[i])) return false;
	}
	return true;
}

/*!
\brief Partial template specialization of a vector with N = 2.

An additional convenience constructor is added, as well as a way
to obtain standard unit axes.

\tparam T The value type.

*/
template <class T> struct vec<T,2> : public ::std::array<T,2>
{
	typedef T float_type;

	inline BOOST_CONSTEXPR vec() = default;
	template <class S>
	inline BOOST_CONSTEXPR vec(const ::std::array<S,2>& a) : ::std::array<T,2>(a) {}
	template <class S>
	inline BOOST_CONSTEXPR vec(::std::array<S,2>&& a) : ::std::array<T,2>(std::move(a)) {}
	template <class S> inline BOOST_CONSTEXPR vec(const vec<S,2>& other) : std::array<T,2>({other[0], other[1]}) {}

	BOOST_CONSTEXPR vec(const T x, const T y) : ::std::array<T,2>({{x,y}}) {} // Don't ask......

	// inline vec(std::initializer_list<T> l) : std::array<T,2>(l) {}

	// inline vec(const FbxVector2& v) BOOST_NOEXCEPT_OR_NOTHROW
	// {
	//  (*this)[0] = static_cast<T>(v[0]);
	//  (*this)[1] = static_cast<T>(v[1]);
	// }

	// inline vec& operator=(const FbxVector2& v) BOOST_NOEXCEPT_OR_NOTHROW
	// {
	//  (*this)[0] = static_cast<T>(v[0]);
	//  (*this)[1] = static_cast<T>(v[1]);
	//  return *this;
	// }

	vec(const FbxVector2& v)
	{
		(*this)[0] = v[0];
		(*this)[1] = v[1];
	}

	inline operator FbxVector2() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return FbxVector2((*this)[0], (*this)[1]);
	}

	//template <class InputIt> vec(InputIt first) { ::std::copy(first, ::std::advance(first, N), this->begin()); }
	//template <class InputIt> vec(InputIt first, InputIt last) { ::std::copy(first, last, this->begin()); }

	template <class S>
	inline vec& operator += (const vec<S,2>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x + y; });
		return *this;
	}

	template <class S>
	inline vec& operator -= (const vec<S,2>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x - y; });
		return *this;
	}

	template <class S>
	inline vec& operator *= (const S s)
	{
		::std::transform(this->begin(), this->end(), this->begin(), [&s](T x) -> T { return s * x; });
		return *this;
	}

	template <class S>
	inline vec& operator /= (const S s)
	{
		return this->operator*=((S(1) / s));
	}

	template <::std::size_t A> static inline vec<T,2> Axis() {
		vec<T,2> r(0,0,0);
		r[A] = 1;
		return r;
	}

	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version)
	{
		ar & *this;
	}

	static const vec<T,2> NaN;
	static const vec<T,2> zero;
};

template <class T> vec<T,2> const vec<T,2>::NaN = 
vec<T,2>(std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN());

template <class T> vec<T,2> const vec<T,2>::zero = 
vec<T,2>(T(0), T(0));

/*!
\brief Partial template specialization of a vector with `N = 3`.

An additional convenience constructor is added, as well as a way
to obtain standard unit axes, and `operator %` is added which denotes
the cross product (or outer product).

\tparam T The value type.

*/
template <class T> struct vec<T,3> : public ::std::array<T,3>
{
	typedef T float_type;
	BOOST_CONSTEXPR vec() = default;
	BOOST_CONSTEXPR vec(const ::std::array<T,3>& a) : ::std::array<T,3>(a) {}
	BOOST_CONSTEXPR vec(::std::array<T,3>&& a) : ::std::array<T,3>(std::move(a)) {}

	BOOST_CONSTEXPR vec(const T x, const T y, const T z) : ::std::array<T,3>({{x,y,z}}) {} // Don't ask......

	//template <class InputIt> vec(InputIt first) { ::std::copy(first, ::std::advance(first, N), this->begin()); }
	template <class InputIt> vec(InputIt first, InputIt last) { ::std::copy(first, last, this->begin()); }

	// inline vec(std::initializer_list<T> l) : std::array<T,3>(l) {}

	// inline vec& operator = (std::initializer_list<T> l)
	// {
	//  std::copy(l.begin(), l.end(), this->begin());
	//  return *this;
	// }

	vec(const FbxDouble3& fbx_vector)
	: ::std::array<T,3>
	({{
		static_cast<float_type>(fbx_vector[0]),
		static_cast<float_type>(fbx_vector[1]),
		static_cast<float_type>(fbx_vector[2])
	}})
	{

	}

	vec(const FbxVector4& v)
	{
		(*this)[0] = static_cast<T>(v[0]);
		(*this)[1] = static_cast<T>(v[1]);
		(*this)[2] = static_cast<T>(v[2]);
	}


	// inline vec(const FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
	// {
	//  (*this)[0] = static_cast<T>(v[0]);
	//  (*this)[1] = static_cast<T>(v[1]);
	//  (*this)[2] = static_cast<T>(v[2]);
	// }

	// inline vec& operator = (const FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
	// {
	//  (*this)[0] = static_cast<T>(v[0]);
	//  (*this)[1] = static_cast<T>(v[1]);
	//  (*this)[2] = static_cast<T>(v[2]);
	//  return *this;
	// }

	template <class S>
	inline vec& operator += (const vec<S,3>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x + y; });
		return *this;
	}

	template <class S>
	inline vec& operator -= (const vec<S,3>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x - y; });
		return *this;
	}

	template <class S>
	inline vec& operator *= (const S s)
	{
		::std::transform(this->begin(), this->end(), this->begin(), [&s](T x) -> T { return s * x; });
		return *this;
	}

	template <class S>
	inline vec& operator /= (const S s)
	{
		return this->operator*=((S(1) / s));
	}

	template <::std::size_t A> static inline vec<T,3> Axis() {
		vec<T,3> r(0,0,0);
		r[A] = 1;
		return r;
	}

	template <class S>
	inline vec operator % (const vec<S,3>& v) const
	{
		vec r;
		#define u (*this)
		r[0] = u[1] * v[2] - u[2] * v[1];
		r[1] = u[2] * v[0] - u[0] * v[2];
		r[2] = u[0] * v[1] - u[1] * v[0];
		#undef u
		return r;
	}

	template <class S>
	inline vec& operator %= (const vec<S,3>& v)
	{
		#define w (*this)
		vec u = w;
		w[0] = u[1] * v[2] - u[2] * v[1];
		w[1] = u[2] * v[0] - u[0] * v[2];
		w[2] = u[0] * v[1] - u[1] * v[0];
		return w;
		#undef w
	}

	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version)
	{
		ar & *this;
	}

	static const vec<T,3> NaN;
	static const vec<T,3> zero;
};

template <class T> const vec<T,3> vec<T,3>::NaN = 
vec<T,3>(std::numeric_limits<T>::quiet_NaN(), 
	std::numeric_limits<T>::quiet_NaN(),
	std::numeric_limits<T>::quiet_NaN());

template <class T> const vec<T,3> vec<T,3>::zero = 
vec<T,3>(T(0), T(0), T(0));

template <class T> inline vec<T,3> normalOfTriangle(const vec<T,3>& p1, const vec<T,3>& p2, const vec<T,3>& p3)
{
	return normalize((p3 - p1) % (p3 - p2));
}

#ifdef BOOST_MSVC
template <class T> struct __declspec(align(16)) vec<T,4> : public ::std::array<T,4> {
#else
template <class T> struct alignas(16) vec<T,4> : public ::std::array<T,4> {
#endif
	typedef T float_type;
	BOOST_CONSTEXPR vec() = default;
	BOOST_CONSTEXPR vec(const ::std::array<T,4>& a) : ::std::array<T,4>(a) {}
	BOOST_CONSTEXPR vec(::std::array<T,4>&& a) : ::std::array<T,4>(std::move(a)) {}
	// vec(const Fbxvec4& fbxvec)
	// {
	//  static_assert(::std::is_same<T,float>::value || ::std::is_same<T,double>::value,
	//      "This constructor can only be used when the type T is either float or double.");
	//  (*this)[0] = ((const double*)fbxvec)[0];
	//  (*this)[1] = ((const double*)fbxvec)[1];
	//  (*this)[2] = ((const double*)fbxvec)[2];
	//  (*this)[3] = ((const double*)fbxvec)[3];
	// }

	BOOST_CONSTEXPR vec(const T x, const T y, const T z, const T w) : ::std::array<T,4>({{x,y,z,w}}) {}

	vec(const FbxVector4& v)
	{
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		(*this)[2] = v[2];
		(*this)[3] = v[3];
	}

	template <class InputIt> vec(InputIt first, InputIt last) { ::std::copy(first, last, this->begin()); }

	template <class S>
	inline vec& operator += (const vec<S,4>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x + y; });
		return *this;
	}

	template <class S>
	inline vec& operator -= (const vec<S,4>& v)
	{
		::std::transform(this->begin(), this->end(), v.begin(), this->begin(), [](T x, T y) -> T { return x - y; });
		return *this;
	}

	template <class S>
	inline vec& operator *= (const S s)
	{
		::std::transform(this->begin(), this->end(), this->begin(), [&s](T x) -> T { return s * x; });
		return *this;
	}

	template <class S>
	inline vec& operator /= (const S s)
	{
		return this->operator*=((S(1) / s));
	}

	template <::std::size_t A> static inline vec<T,4> Axis() {
		vec<T,3> r(0,0,0,0);
		r[A] = 1;
		return r;
	}

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

	// friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned /*int version*/)
	{
		ar & this->operator[](0) & this->operator[](1) & this->operator[](2) & this->operator[](3);
	}

	// static const vec<T,4> NaN;
	// static const vec<T,4> zero;

};

// template <class T> vec<T,4> const vec<T,4>::NaN = 
// vec<T,4>(std::numeric_limits<T>::quiet_NaN(), 
// 	std::numeric_limits<T>::quiet_NaN(),
// 	std::numeric_limits<T>::quiet_NaN(),
// 	std::numeric_limits<T>::quiet_NaN());

// template <class T> vec<T,4> const vec<T,4>::zero = 
// vec<T,4>(T(0), T(0), T(0), T(0));

template <class T> inline T inverse(const T t) { return T(1) / t; }

template <class S, class T, ::std::size_t N> inline 
auto operator + (const vec<S,N>& lhs, const vec<T,N>& rhs) -> vec<typename std::common_type<S,T>::type, N>
{
	typedef typename std::common_type<S,T>::type R;
	vec<R,N> r;
	::std::transform(lhs.begin(), lhs.end(), rhs.begin(), r.begin(), [](R x, R y) -> T { return x + y; });
	return r;
}

template <class S, class T, ::std::size_t N> inline
auto operator - (const vec<S,N>& lhs, const vec<T,N>& rhs) -> vec<typename std::common_type<S,T>::type, N>
{
	typedef typename std::common_type<S,T>::type R;
	vec<R,N> r;
	::std::transform(lhs.begin(), lhs.end(), rhs.begin(), r.begin(), [](R x, R y) -> T { return x - y; });
	return r;
}

template <class T, ::std::size_t N> inline vec<T,N> operator - (const vec<T,N>& v)
{
	vec<T,N> r;
	::std::transform(v.begin(), v.end(), r.begin(), [](T x) -> T { return -x; });
	return r;
}

template <class S, class T, ::std::size_t N> inline
auto dot (const vec<S,N>& lhs, const vec<T,N>& rhs) -> typename std::common_type<S,T>::type
{
	return ::std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), typename std::common_type<S,T>::type(0));
}

// Template specialization for T == float, N == 4, uses SIMD.
template <> inline float dot (const vec<float,4>& lhs, const vec<float,4>& rhs)
{
	PROFILE("operator * (const vec<float,4>&, const vec<float,4>&)")
	__m128 product = _mm_mul_ps(_mm_load_ps(lhs.data()), _mm_load_ps(rhs.data()));
	__m128 halfsum = _mm_hadd_ps(product, product);
	return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));
}

template <class S, class T, ::std::size_t N> inline
auto operator * (const vec<T,N>& v, const S s) -> vec<typename std::common_type<S,T>::type, N>
{
	typedef typename std::common_type<S,T>::type R;
	vec<R,N> r;
	::std::transform(v.begin(), v.end(), r.begin(), [&s](T x) -> T { return s * x; });
	return r;
}

template <class S, class T, ::std::size_t N> inline vec<T,N> operator * (const S s, const vec<T,N>& v)
{
	return v * s;
}

template <class T, ::std::size_t N> inline vec<T,N> operator / (const vec<T,N>& v, const T s)
{
	return v * (T(1) / s);
}

template <class T, ::std::size_t N> inline T norm2(const vec<T,N>& v) { return dot(v,v); }
template <class T, ::std::size_t N> inline T norm(const vec<T,N>& v) { return sqrt(norm2(v)); }
template <class T, ::std::size_t N> inline T distance2(const vec<T,N>& u, const vec<T,N>& v) { return norm2(u-v); }
template <class T, ::std::size_t N> inline T distance(const vec<T,N>& u, const vec<T,N>& v) { return sqrt(distance2(u,v)); }
template <class T, ::std::size_t N> inline vec<T,N> max(const vec<T,N>& u, const vec<T,N>& v)
{
	vec<T,N> r;
	::std::transform(u.begin(), u.end(), v.begin(), r.begin(), [](T x, T y) -> T { return ::std::max(x,y); });
	return r;
}
template <class T, ::std::size_t N> inline vec<T,N> min(const vec<T,N>& u, const vec<T,N>& v)
{
	vec<T,N> r;
	::std::transform(u.begin(), u.end(), v.begin(), r.begin(), [](T x, T y) -> T { return ::std::min(x,y); });
	return r;
}
template <class T, ::std::size_t N> inline vec<T,N> mix(const vec<T,N>& u, const vec<T,N>& v, const T a) { return (T(1) - a) * u + a * v; }
template <class T, ::std::size_t N> inline vec<T,N> normalize(const vec<T,N>& u)
{
	PROFILE("normalize")
	return u / norm(u);
}
template <class T, ::std::size_t N> inline vec<T,N> project(const vec<T,N>& u, const vec<T,N>& v) { return ((u * v) / (v * v)) * v; }
template <class T, ::std::size_t N> inline vec<T,N> reject(const vec<T,N>& u, const vec<T,N>& v) { return u - project(u, v); }
template <class T, ::std::size_t N> inline vec<T,N> reflect(const vec<T,N>& u, const vec<T,N>& v) { return T(2) * project(u,v) - v; }

template <class T, ::std::size_t N>
::std::ostream& operator << (::std::ostream& os, const vec<T,N>& v)
{
	for (std::size_t i = 0; i < N - 1; ++i) os << v[i] << ' ';
	os << v[N-1];
	return os;
}

template <class T, ::std::size_t N>
::std::istream& operator >> (::std::istream& is, vec<T,N>& v)
{
	for (std::size_t i = 0; i < N; ++i) is >> v[i];
	return is;
}

//
// mat<T,N>
//

template <class T, ::std::size_t N> struct mat : ::std::array<vec<T,N>,N>
{
	typedef T float_type;
	mat() = default;

	inline T& operator()(const ::std::size_t row, const ::std::size_t col) { return (*this)[row][col]; }
	inline const T& operator()(const ::std::size_t row, const ::std::size_t col) const { return (*this)[row][col]; }

	inline static mat identity()
	{
		std::size_t i,j;
		mat r;
		for (i = 0; i < N; ++i)
			for (j = 0; j < N; ++j)
				r(i,j) = (i == j) ? T(1) : T(0);
		return r;
	}

	inline static mat zero()
	{
		mat r;
		::std::fill(r.begin(), r.end(), T(0));
		return r;
	}

	inline mat& operator *= (const mat& m)
	{
		*this = *this * m;
		return *this;
	}
};

template <class FloatType, ::std::size_t Size> class to_hex
{
public:
	to_hex(FloatType const*const v) : m_v(v) {}
	friend ::std::ostream& operator << (::std::ostream& os, const to_hex& th)
	{
		return th(os);
	}
private:
	::std::ostream& operator () (::std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		os << '#';
		for (std::size_t i = 0; i < Size; ++i)
		{
			os << ::std::hex << ::std::noshowbase << ::std::setw(2) << ::std::setfill('0')
				<< static_cast<int>(static_cast<unsigned char>(m_v[i] * 255));
		}
		return os;
	}
	FloatType const*const m_v;
};

template <class IntegralType, ::std::size_t Size> class to_float
{
public:
	to_float(IntegralType const*const v) : m_v(v) {}
	friend ::std::ostream& operator << (::std::ostream& os, const to_float& tf)
	{
		return tf(os);
	}
private:
	::std::ostream& operator () (::std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		os << '[';
		for (::std::size_t i = 0; i < Size; ++i)
		{
			os << ::std::dec << ::std::noshowbase << ::std::fixed
				<< static_cast<float>(m_v[i]) / 255.0f;
			if (i != Size - 1) os << ", ";
		}
		os << ']';
		return os;
	}
	IntegralType const*const m_v;
};

template <class T, ::std::size_t N> inline mat<T,N> operator + (const mat<T,N>& a, const mat<T,N>& b)
{
	mat<T,N> r;
	::std::transform(a.begin(), a.end(), b.begin(), r.begin(), [](const T& x, const T& y) -> T { return x + y; });
	return r;
}

template <class T, ::std::size_t N> inline mat<T,N> operator - (const mat<T,N>& a, const mat<T,N>& b)
{
	mat<T,N> r;
	::std::transform(a.begin(), a.end(), b.begin(), r.begin(), [](const T& x, const T& y) -> T { return x - y; });
	return r;
}

template <class T, ::std::size_t N> inline vec<T,N> operator * (const mat<T,N>& m, const vec<T,N>& v)
{
	PROFILE("operator * (const mat<T,N>&, const vec<T,N>&)")
	vec<T,N> r;
	for (::std::size_t i = 0; i < N; ++i) r[i] = dot(m[i], v);
	return r;
}

template <class T, ::std::size_t N> inline mat<T,N> operator * (const mat<T,N>& a, const mat<T,N>& b)
{
	PROFILE("operator * (const mat<T,N>&, const mat<T,N>&)")
	mat<T,N> r;
	std::size_t i,j,k;
	for (i = 0; i < N; ++i)
	{
		for (j = 0; j < N; ++j)
		{
			r(i,j) = T(0);
			for (k = 0; k < N; ++k)
			{
				r(i,j) += a(i,k) * b(k,j);
			}
		}

	}
	return r;
}

template <class T> struct mat<T,2> : ::std::array<vec<T,2>,2>
{
	typedef typename vec<T,2>::float_type float_type;
	mat() = default;

	inline T& operator()(const ::std::size_t row, const ::std::size_t col) { return (*this)[row][col]; }
	inline const T& operator()(const ::std::size_t row, const ::std::size_t col) const { return (*this)[row][col]; }

	mat(const T m00, const T m01,
		const T m10, const T m11)
	{
		#define m(i,j) (*this)(i,j)
		m(0,0) = m00; m(0,1) = m01;
		m(1,0) = m10; m(1,1) = m11;
		#undef m
	}

	mat(const vec<T,2>& row1, const vec<T,2>& row2)
	{
		#define m(i,j) (*this)(i,j)
		m(0,0) = row1[0]; m(0,1) = row1[1];
		m(1,0) = row2[0]; m(1,1) = row2[1];
		#undef m
	}

	inline mat& operator *= (const mat& m)
	{
		*this = *this * m;
		return *this;
	}

	inline static mat identity()
	{
		std::size_t i,j;
		mat r;
		for (i = 0; i < 2; ++i)
			for (j = 0; j < 2; ++j)
				r(i,j) = (i == j) ? T(1) : T(0);
		return r;
	}

	inline static mat zero()
	{
		mat r;
		::std::fill(r.begin(), r.end(), vec<T,3>(0,0,0));
		return r;
	}

	inline T det() const
	{
		return (*this)(0,0) * (*this)(1,1) - (*this)(0,1) * (*this)(1,0);
	}

	inline mat& invert()
	{
		::std::swap((*this)(1,0), (*this)(0,1));
		(*this)(0,0) = -(*this)(0,0);
		(*this)(1,1) = -(*this)(1,1);
		return *this;
	}
};

//
// Template specialization mat<T,3>
//

template <class T> struct quat;

template <class T> struct mat<T,3> : ::std::array<vec<T,3>,3>
{
	typedef typename vec<T,3>::float_type float_type;
	mat() = default;

	inline T& operator()(const ::std::size_t row, const ::std::size_t col) { return (*this)[row][col]; }
	inline const T& operator()(const ::std::size_t row, const ::std::size_t col) const { return (*this)[row][col]; }

	// #ifdef __math_row_major_storage__
	// inline T& operator()(const std::size_t row, const std::size_t col) { return (*this)[row * 3 + col]; }
	// i3line const T& operator()(const std::size_t row, const std::size_t col) const { return (*this)[row * 3 + col]; }
	// #else // column major storage
	// inline T& operator()(const std::size_t row, const std::size_t col) { return (*this)[row * 3 + col]; }
	// inline const T& operator()(const std::size_t row, const std::size_t col) const { return (*this)[row * 3 + col]; }
	// #endif

	mat(const T m00, const T m01, const T m02,
		const T m10, const T m11, const T m12,
		const T m20, const T m21, const T m22)
	{
		#define m(i,j) (*this)(i,j)
		m(0,0) = m00; m(0,1) = m01; m(0,2) = m02;
		m(1,0) = m10; m(1,1) = m11; m(1,2) = m12;
		m(2,0) = m20; m(2,1) = m21; m(2,2) = m22;
		#undef m
	}

	mat(const vec<T,3>& row1, const vec<T,3>& row2, const vec<T,3>& row3)
	{
		#define m(i,j) (*this)(i,j)
		m(0,0) = row1[0]; m(0,1) = row1[1]; m(0,2) = row1[2];
		m(1,0) = row2[0]; m(1,1) = row2[1]; m(1,2) = row2[2];
		m(2,0) = row3[0]; m(2,1) = row3[1]; m(2,2) = row3[2];
		#undef m
	}

	// rotation constructor
	mat(const T angle, const vec<T,3>& axis)
	{
		PROFILE("mat(const T angle, const vec<T,3>& axis)")
		#define m(i,j) (*this)(i,j)
		vec<T,3> theVec;
		T sin, cos;
		sincos(angle, sin, cos);
		// const T sin = ::std::sin(angle);
		// const T cos = ::std::cos(angle);
		const T omcos = T(1) - cos;

		theVec[0] = cos + axis[0] * axis[0] * omcos;
		theVec[1] = axis[0] * axis[1] * omcos - axis[2] * sin;
		theVec[2] = axis[0] * axis[2] * omcos + axis[1] * sin;
		theVec = normalize(theVec);

		m(0,0) = theVec[0]; m(0,1) = theVec[1]; m(0,2) = theVec[2];

		theVec[0] = axis[0] * axis[1] * omcos + axis[2] * sin;
		theVec[1] = cos + axis[1] * axis[1] * omcos;
		theVec[2] = axis[1] * axis[2] * omcos - axis[0] * sin;
		theVec = normalize(theVec);

		m(1,0) = theVec[0]; m(1,1) = theVec[1]; m(1,2) = theVec[2];

		theVec[0] = axis[1] * axis[0] * omcos - axis[1] * sin;
		theVec[1] = axis[1] * axis[1] * omcos + axis[0] * sin;
		theVec[2] = cos + axis[2] * axis[2] * omcos;
		theVec = normalize(theVec);

		m(2,0) = theVec[0]; m(2,1) = theVec[1]; m(2,2) = theVec[2];
		#undef m
	}

	inline mat& operator *= (const mat& m)
	{
		*this = *this * m;
		return *this;
	}

	inline static mat identity()
	{
		std::size_t i,j;
		mat r;
		for (i = 0; i < 3; ++i)
			for (j = 0; j < 3; ++j)
				r(i,j) = (i == j) ? T(1) : T(0);
		return r;
	}

	inline static mat zero()
	{
		mat r;
		::std::fill(r.begin(), r.end(), vec<T,3>(0,0,0));
		return r;
	}

	inline T det() const
	{
		#define m(i,j) (*this)(i,j)
		return
		  m(0,0)*m(1,1)*m(2,2)
		+ m(0,1)*m(1,2)*m(2,0)
		+ m(0,2)*m(1,0)*m(2,1)
		- m(0,2)*m(1,1)*m(2,0)
		- m(0,1)*m(1,0)*m(2,2)
		- m(0,0)*m(1,2)*m(2,1);
		#undef m
	}

	inline mat& invert()
	{
		#define m(i,j) (*this)(i,j)
		const T A =   m(1,1)*m(2,2) - m(1,2)*m(2,1) ;
		const T B = -(m(1,0)*m(2,2) - m(1,2)*m(2,0));
		const T C =   m(1,0)*m(2,1) - m(1,1)*m(2,0) ;
		const T D = -(m(0,1)*m(2,2) - m(0,2)*m(2,1));
		const T E =   m(0,0)*m(2,2) - m(0,2)*m(2,0) ;
		const T F = -(m(0,0)*m(2,1) - m(0,1)*m(2,0));
		const T G =   m(0,1)*m(1,2) - m(0,2)*m(1,1) ;
		const T H = -(m(0,0)*m(1,2) - m(0,2)*m(1,0));
		const T K =   m(0,0)*m(1,1) - m(0,1)*m(1,0) ;

		// d is the inverse of the determinant.
		const T d = T(1) / (m(0,0)*A + m(0,1)*B + m(0,2)*C);

		m(0,0) = d * A;
		m(0,1) = d * D;
		m(0,2) = d * G;
		m(1,0) = d * B;
		m(1,1) = d * E;
		m(1,2) = d * H;
		m(2,0) = d * C;
		m(2,1) = d * F;
		m(2,2) = d * K;
		#undef m
		return *this;
	}

	inline mat& transpose()
	{
		::std::swap((*this)(0,1), (*this)(1,0));
		::std::swap((*this)(0,2), (*this)(2,0));
		::std::swap((*this)(1,2), (*this)(2,1));
		return *this;
	}

	quat<T> to_quaternion() const;
};

//
// Template specialization mat<T,4>
//

template <class T> struct mat<T,4> : ::std::array<vec<T,4>,4>
{
	typedef typename vec<T,4>::float_type float_type;
	mat() = default;

	inline T& operator()(const ::std::size_t row, const ::std::size_t col) { return (*this)[row][col]; }
	inline const T& operator()(const ::std::size_t row, const ::std::size_t col) const { return (*this)[row][col]; }

	// #ifdef __math_row_major_storage__
	// inline T& operator()(const std::size_t row, const std::size_t col) { return (*this)[row * 4 + col]; }
	// inline const T& operator()(const std::size_t row, const std::size_t col) const { return (*this)[row * 4 + col]; }
	// #else // column major storage
	// inline T& operator()(const std::size_t row, const std::size_t col) { return (*this)[row + 4 * col]; }
	// inline const T& operator()(const std::size_t row, const std::size_t col) const { return (*this)[row + 4 * col]; }
	// #endif


	mat(const T m00, const T m01, const T m02, const T m03,
		const T m10, const T m11, const T m12, const T m13,
		const T m20, const T m21, const T m22, const T m23,
		const T m30, const T m31, const T m32, const T m33)
	{
		(*this)[0] = {m00, m01, m02, m03};
		(*this)[1] = {m10, m11, m12, m13};
		(*this)[2] = {m20, m21, m22, m23};
		(*this)[3] = {m30, m31, m32, m33};
	}

	// Uniform scale constructor
	mat(const T scale)
	{
		PROFILE("mat(const T scale)")
		(*this)[0] = {scale, T(0), T(0), T(0)};
		(*this)[1] = {T(0), scale, T(0), T(0)};
		(*this)[2] = {T(0), T(0), scale, T(0)};
		(*this)[3] = {T(0), T(0),  T(0), T(1)};
	}

	// Non-Uniform scale constructor
	mat(const T scaleX, const T scaleY, const T scaleZ)
	{
		PROFILE("mat(const T scaleX, const T scaleY, const T scaleZ)")
		(*this)[0] = {scaleX, 0, 0, 0};
		(*this)[1] = {0, scaleY, 0, 0};
		(*this)[2] = {0, 0, scaleZ, 0};
		(*this)[3] = {0, 0,      0, 1};
	}

	// rotation constructor
	mat(const T angle, const vec<T,3>& axis)
	{
		PROFILE("mat(const T angle, const vec<T,3>& axis)")
		#define m(i,j) (*this)(i,j)
		vec<T,3> theVec;
		T sin, cos;
		sincos(angle, sin, cos);
		const T omcos = T(1) - cos;

		theVec[0] = cos + axis[0] * axis[0] * omcos;
		theVec[1] = axis[0] * axis[1] * omcos - axis[2] * sin;
		theVec[2] = axis[0] * axis[2] * omcos + axis[1] * sin;
		theVec = normalize(theVec);

		m(0,0) = theVec[0]; m(0,1) = theVec[1]; m(0,2) = theVec[2]; m(0,3) = 0;

		theVec[0] = axis[0] * axis[1] * omcos + axis[2] * sin;
		theVec[1] = cos + axis[1] * axis[1] * omcos;
		theVec[2] = axis[1] * axis[2] * omcos - axis[0] * sin;
		theVec = normalize(theVec);

		m(1,0) = theVec[0]; m(1,1) = theVec[1]; m(1,2) = theVec[2]; m(1,3) = 0;

		theVec[0] = axis[0] * axis[2] * omcos - axis[1] * sin;
		theVec[1] = axis[1] * axis[2] * omcos + axis[0] * sin;
		theVec[2] = cos + axis[2] * axis[2] * omcos;
		theVec = normalize(theVec);

		m(2,0) = theVec[0]; m(2,1) = theVec[1]; m(2,2) = theVec[2]; m(2,3) = 0;

		m(3,0) = m(3,1) = m(3,2) = 0; m(3,3) = 1;
		#undef m
	}

	// translation constructor
	mat(const vec<T,3>& translation)
	{
		PROFILE("mat(const vec<T,3>& translation)")
		#define m(i,j) (*this)(i,j)
		m(0,0) = 1;
		m(1,0) = 0;
		m(2,0) = 0;
		m(3,0) = 0;

		m(0,1) = 0;
		m(1,1) = 1;
		m(2,1) = 0;
		m(3,1) = 0;

		m(0,2) = 0;
		m(1,2) = 0;
		m(2,2) = 1;
		m(3,2) = 0;

		m(0,3) = translation[0];
		m(1,3) = translation[1];
		m(2,3) = translation[2];
		m(3,3) = 1;
		#undef m
	}

	// Perspective projection constructor
	mat(const T fovy, const T aspect, const T zNear, const T zFar)
	{
		PROFILE("mat(const T fovy, const T aspect, const T zNear, const T zFar)")
		const T f = T(1) / std::tan(fovy / T(2));
		const T m22 = (zFar + zNear) / (zNear - zFar);
		const T m23 = (T(2) * zFar * zNear) / (zNear - zFar);
		#define m(i,j) (*this)(i,j)
		m(0,0) = f/aspect;
		m(1,0) = 0;
		m(2,0) = 0;
		m(3,0) = 0;

		m(0,1) = 0;
		m(1,1) = f;
		m(2,1) = 0;
		m(3,1) = 0;

		m(0,2) = 0;
		m(1,2) = 0;
		m(2,2) = m22;
		m(3,2) = -1;

		m(0,3) = 0;
		m(1,3) = 0;
		m(2,3) = m23;
		m(3,3) = 0;
		#undef m
	}

	void unproject_perspective(T& fov, T& aspectratio, T& nearplane, T& farplane)
	const BOOST_NOEXCEPT_OR_NOTHROW
	{
		#define m(i,j) (*this)(i,j)
		fov = T(2) * std::atan(1 / m(1,1));
		aspectratio = m(1,1) / m(0,0);
		nearplane = m(2,3) / (m(2,2) - T(1));
		farplane = m(2,3) / (m(2,2) + T(1));
		#undef m
	}

	// Orthographic projection constructor
	mat(const T left, const T right, const T bottom, const T top, const T nearVal, const T farVal)
	{
		PROFILE("mat(const T left, const T right, const T bottom, const T top, const T nearVal, const T farVal)")
		#define m(i,j) (*this)(i,j)

		const auto rl = right - left;
		const auto tp = top - bottom;
		const auto fn = farVal - nearVal;

		m(0,0) = T(2) * nearVal / rl;
		m(1,0) = 0;
		m(2,0) = 0;
		m(3,0) = 0;

		m(0,1) = 0;
		m(1,1) = T(2) * nearVal / tp;
		m(2,1) = 0;
		m(3,1) = 0;

		m(0,2) = (right + left) / rl;
		m(1,2) = (top + bottom) / tp;
		m(2,2) = -(farVal + nearVal) / fn;
		m(3,2) = 0;

		m(0,3) = 0;
		m(1,3) = 0;
		m(2,3) = -(T(2) * farVal * nearVal) / fn;
		m(3,3) = 1;
		#undef m
	}

	static mat orthographic(const T width, const T height, const T near_val, const T far_val)
	{
		return mat
		(
			T(1) / width, 0, 0, 0,
			0, T(1) / height, 0, 0,
			0, 0, T(-2) / (far_val - near_val), -(far_val + near_val) / (far_val - near_val),
			0, 0, 0, 1
		);
	}

	static mat frustum(const T fieldofview, const T aspectratio, const T nearplane, const T farplane)
	{
		const T b = T(1) / std::tan(fieldofview / T(2));
		const T nf2 = T(2) * nearplane * farplane;
		return mat
		(
			aspectratio / b, 0, 0, 0,
			0, 1.0f / b, 0, 0,
			0, 0, 0, 1,
			0, 0, (nearplane - farplane) / nf2, (nearplane + farplane) / nf2
		);
	}

	// "Look at" constructor
	mat(const vec<T,3>& eye, const vec<T,3>& center, const vec<T,3> up)
	{
		PROFILE("mat(const vec<T,3>& eye, const vec<T,3>& center, const vec<T,3> up)")
		const vec<T,3> f = normalize(center - eye);

		// We have to normalize the result of f % normalize(up) AGAIN,
		// because of floating point errors. You might see the
		// sides of the view become "stretched" if we do not normalize.
		const vec<T,3> s = normalize(f % normalize(up));

		// Just to be safe, normalize it once more.
		const vec<T,3> u = normalize(s % f);

		#define m(i,j) (*this)(i,j)
		m(0,0) =  s[0];
		m(1,0) =  u[0];
		m(2,0) = -f[0];
		m(3,0) =     0;

		m(0,1) =  s[1];
		m(1,1) =  u[1];
		m(2,1) = -f[1];
		m(3,1) = 0;

		m(0,2) =  s[2];
		m(1,2) =  u[2];
		m(2,2) = -f[2];
		m(3,2) = 0;

		m(0,3) = -dot(eye, s);
		m(1,3) = -dot(eye, u);
		m(2,3) =  dot(eye, f);
		m(3,3) = 1;
		#undef m
	}

	// mat<T,3> constructor.
	mat(const mat<T,3>& rotation_matrix)
	{
		std::copy(rotation_matrix[0].begin(), rotation_matrix[0].end(), this->operator[](0).begin());
		std::copy(rotation_matrix[1].begin(), rotation_matrix[1].end(), this->operator[](1).begin());
		std::copy(rotation_matrix[2].begin(), rotation_matrix[2].end(), this->operator[](2).begin());
		#define m(i,j) (*this)(i,j)
		m(3,0) = 0;
		m(3,1) = 0;
		m(3,2) = 0;
		m(3,3) = 1;
		m(0,3) = 0;
		m(1,3) = 0;
		m(2,3) = 0;
		#undef m
	}

	inline mat& operator *= (const mat& m)
	{
		*this = *this * m;
		return *this;
	}

	inline vec<T,3> apply_to_point(const vec<T,3>& point) const
	{
		vec<T,4> tmp(point[0], point[1], point[2], T(1));
		tmp = (*this * tmp);
		return vec<T,3>(tmp[0], tmp[1], tmp[2]);
	}

	inline vec<T,3> apply_to_direction(const vec<T,3>& direction) const
	{
		vec<T,4> tmp(direction[0], direction[1], direction[2], T(0));
		tmp = (*this * tmp);
		return vec<T,3>(tmp[0], tmp[1], tmp[2]);
	}

	inline mat<T,3> upper_left_33() const
	{
		mat<T,3> r;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				r(i,j) = (*this)(i,j);
			}
		}
		return r;
	}

	inline mat<T,4>& transpose()
	{
		#define m(i,j) (*this)(i,j)
		std::swap(m(0,1), m(1,0));
		std::swap(m(0,2), m(2,0));
		std::swap(m(0,3), m(3,0));
		std::swap(m(1,2), m(2,1));
		std::swap(m(1,3), m(3,1));
		std::swap(m(2,3), m(3,2));
		#undef m
		return *this;
	}

	inline static mat identity()
	{
		std::size_t i,j;
		mat r;
		for (i = 0; i < 4; ++i)
			for (j = 0; j < 4; ++j)
				r(i,j) = (i == j) ? T(1) : T(0);
		return r;
	}

	inline static mat zero()
	{
		mat r;
		::std::fill(r.begin(), r.end(), T(0));
		return r;
	}
};

template <class T, ::std::size_t N> std::ostream& operator << (std::ostream& os, const mat<T,N>& m)
{
	std::stringstream ss("");
	std::size_t maxPadding = 0, padding = 0;
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			ss << m(i,j);
			if (ss.str().length() + 1 > maxPadding)
			maxPadding = ss.str().length() + 1;
			ss.str("");
		}
	}
	for (int i = 0; i < N; ++i) {
		os << '[';
		for (int j = 0; j < N; ++j) {
			ss << m(i,j);
			padding = maxPadding - ss.str().length();
			while (padding != 0) { os << ' '; --padding; }
			os << m(i,j);
			ss.str("");
			if (j != N - 1) os << ',';
		}
		os << ']';
		if (i != N - 1) os << '\n';
	}
	return os;
}
//
// euler_xyz
// 
template <class T> struct euler_xyz
{
	typedef T float_type;
	T alpha;
	T beta;
	T gamma;
	euler_xyz(const T alpha, const T beta, const T gamma) : alpha(alpha), beta(beta), gamma(gamma) {}
	mat<T,3> to_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		T a_, a__, b_, b__, c_, c__;
		sincos(alpha, a_, a__);
		sincos(beta, b_, b__);
		sincos(gamma, c_, c__);
		return mat<T,3>
		(
							 b__ * c__,                 -b__ * c_,      -b_ ,
			a__ * c_ - a_  * b_  * c__, a_ * c__ + a_  * b_  * c_, -a_ * b__,
			a_  * c_ + a__ * b_  * c__, a_ * c__ - a__ * b_  * c_, a__ * b__
		);
	}
};

//
// euler_zxz
//

template <class T> struct euler_zxz
{
	T phi;
	T theta;
	T psi;
};

template <class T> struct axis_angle;

//
// quat
//

template <class T> struct quat
{
	typedef T float_type;
	T w;
	T x;
	T y;
	T z;

	quat() {}
	quat(const T w, const T x, const T y, const T z) : w(w), x(x), y(y), z(z) {}
	quat(const T real, const vec<T,3>& imaginary) : w(real), x(imaginary[0]), y(imaginary[1]), z(imaginary[2]) {}
	quat(const euler_xyz<T>& e)
	{
		T halfsin;
		T halfcos;
		const T halfsum = (e.alpha + e.beta) / T(2);
		const T halfdiff = (e.alpha - e.beta) / T(2);
		sincos(e.gamma / T(2), halfsin, halfcos);
		w = cos(halfdiff) * halfsin;
		x = sin(halfdiff) * halfsin;
		y = sin(halfsum) * halfcos;
		z = cos(halfsum) * halfcos; 
	}

	static quat from_angle_axis(const T angle, const vec<T,3>& axis)
	{
		quat result;
		T sin, cos;
		sincos(angle / T(2), sin, cos);
		result.w = cos;
		result.x = sin * axis[0];
		result.y = sin * axis[1];
		result.z = sin * axis[2];
		return result;
	}
	static quat from_euler_zxz(const T phi, const T theta, const T psi)
	{
		quat result;
		T halfsin;
		T halfcos;
		const T halfsum = (phi + psi) / T(2);
		const T halfdiff = (phi - psi) / T(2);
		sincos(theta / T(2), halfsin, halfcos);
		result.w = cos(halfdiff) * halfsin;
		result.x = sin(halfdiff) * halfsin;
		result.y = sin(halfsum) * halfcos;
		result.z = cos(halfsum) * halfcos;
		return result;
	}

	static quat look_at(const vec<T,3>& eye, const vec<T,3>& center, const vec<T,3>& up)
	{
		const T dotproduct = dot(normalize(eye), normalize(center));

		if (almost_equal(dotproduct, T(-1), 5))
		{
			// The eye and the subject point in opposite directions.
			return from_angle_axis(deg_to_rad(T(180)), up);
		}
		else if (almost_equal(dotproduct, T(1), 5))
		{
			// The eye already looks at the subject.
			return quat(1, 0, 0, 0);
		}
		else
		{
			// Do calculations as usual.
			const T angle = std::acos(dotproduct);
			const vec<T,3> rotation_axis = normalize(center % up);
			return from_angle_axis(angle, rotation_axis);
		}
	}

	quat& operator = (const vec<T,3>& v)
	{
		w = T(0);
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	quat& operator += (const quat& q)
	{
		w += q.w;
		x += q.x;
		y += q.y;
		z += q.z;
		return *this;
	}

	quat& operator -= (const quat& q)
	{
		w -= q.w;
		x -= q.x;
		y -= q.y;
		z -= q.z;
		return *this;
	}

	quat& operator *= (const T s)
	{
		w *= s;
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	quat& operator /= (const T s)
	{
		w /= s;
		x /= s;
		y /= s;
		z /= s;
	}

	quat& operator *= (const quat& q)
	{
		return *this = *this * q;
	}

	quat conjugate() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return quat(w, -x, -y, -z);
	}

	vec<T,3> apply_to(const vec<T,3>& v) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto temp = (*this) * quat(0, v[0], v[1], v[2]) * conjugate();
		return vec<T,3>(temp.x, temp.y, temp.z);
	}

	mat<T,4> to_rotation_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const T n = norm2(*this);
		const T s = n == T(0) ? T(0) : T(2) / n;
		const T wx = s * w * x;
		const T wy = s * w * y;
		const T wz = s * w * z;
		const T xx = s * x * x;
		const T xy = s * x * y;
		const T xz = s * x * z;
		const T yy = s * y * y;
		const T yz = s * y * z;
		const T zz = s * z * z;
		return mat<T,4>
		(
		 1 - (yy + zz)  ,      xy - wz   ,       xz + wy , 0,
			  xy + wz   , 1 - (xx + zz)  ,      yz - wx  , 0,
			  xz - wy   ,      yz + wx   , 1 - (xx + yy) , 0,
					 0  ,              0 ,             0 , 1
		);
	}

	mat<T,3> to_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const T n = norm2(*this);
		const T s = n == T(0) ? T(0) : T(2) / n;
		const T wx = s * w * x;
		const T wy = s * w * y;
		const T wz = s * w * z;
		const T xx = s * x * x;
		const T xy = s * x * y;
		const T xz = s * x * z;
		const T yy = s * y * y;
		const T yz = s * y * z;
		const T zz = s * z * z;
		return mat<T,3>
		(
			T(1) - (yy + zz),         xy - wz ,         xz + wy ,
					xy + wz , T(1) - (xx + zz),         yz - wx ,
					xz - wy ,         yz + wx , T(1) - (xx + yy)
		);
	}

	axis_angle<T> to_angle_axis() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const T alpha_over_2 = std::acos(w);
		const T sin_alpha_over_2 = sin(alpha_over_2);
		const T cos_beta_x = x / sin_alpha_over_2;
		const T cos_beta_y = y / sin_alpha_over_2;
		const T cos_beta_z = z / sin_alpha_over_2;
		return axis_angle<T>(vec<T, 3>(cos_beta_x, cos_beta_y, cos_beta_z), T(2) * alpha_over_2);
	}

	vec<T,3> direction() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return apply_to(vec<T,3>(0,0,-1));
	}
};

template <class T> quat<T> mat<T,3>::to_quaternion() const
{
	#define m(i,j) (*this)(i,j)
	const T r = std::sqrt(m(0,0) + m(1,1) + m(2,2) + T(1));
	const T s = T(0.5) / r;
	return quat<T>(T(0.5) * r, s * (m(2,1) - m(1,2)), s * (m(0,2) - m(2,0)), s * (m(1,0) - m(0,1)));
	#undef m
}

template <class T> ::std::ostream& operator << (std::ostream& os, const quat<T>& q)
{
	return os << q.w << " + " << q.x  << "i + " << q.y << "j + " << q.z << "k";
}

template <class T> quat<T> operator + (const quat<T>& a, const quat<T>& b)
{
	return quat<T>(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
}

template <class T> quat<T> operator - (const quat<T>& a, const quat<T>& b)
{
	return quat<T>(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
}

template <class T> quat<T> operator * (const quat<T>& a, const quat<T>& b)
{
	return quat<T>(
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
		a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w);
}

template <class T> quat<T> operator * (const quat<T>& q, const T s)
{
	return quat<T>(q.w * s, q.x * s, q.y * s, q.z * s);
}

template <class T> quat<T> operator * (const T s, const quat<T>& q)
{
	return quat<T>(s * q.w, s * q.x, s * q.y, s * q.z);
}

template <class T> quat<T> operator / (const quat<T>& q, const T s)
{
	return quat<T>(q.w / s, q.x / s, q.y / s, q.z / s);
}

template <class T> quat<T> Conjugate(const quat<T>& q)
{
	return quat<T>(q.w, -q.x, -q.y, -q.z);
}

template <class T> vec<T,3> ConjugateWithvec(const quat<T>& q, const vec<T,3>& v)
{
	const quat<T> qConj = Conjugate(q);
	const quat<T> vAsQuat(0, v);
	const quat<T> res = q * vAsQuat * qConj;
	return vec<T,3>(res.x, res.y, res.z);
}

template <class T> T norm2(const quat<T>& q) { return (q * Conjugate(q)).w; }

template <class T> T norm(const quat<T>& q) { return sqrt(norm2(q)); }

template <class T> quat<T> normalize(const quat<T>& q)
{
	return q / norm(q);
}

template <class T> T distance2(const quat<T>& a, const quat<T>& b)
{
	return norm2(b-a);
}

template <class T> T distance(const quat<T>& a, const quat<T>& b)
{
	return sqrt(distance(a,b));
}

template <class T> quat<T> mix(const quat<T>& a, const quat<T>& b, const T t)
{
	return a * (T(1) - t) + t * b;
}

template <class T> struct axis_angle
{
	typedef T float_type;
	vec<T,3> axis;
	T angle;
	axis_angle() = default;
	axis_angle(const vec<T,3>& axis, const T angle) : axis(axis), angle(angle) {}
	mat<T,3> to_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		T sin, cos;
		sincos(angle, sin, cos);
		const mat<T,3> e
		(
				   0, -axis[2],  axis[1], 
			 axis[2],        0, -axis[0], 
			-axis[1],  axis[0],        0
		);
		const mat<T,3> ee
		(
			axis[0] * axis[0], axis[1] * axis[0], axis[0] * axis[2],
			axis[0] * axis[1], axis[1] * axis[1], axis[1] * axis[2],
			axis[0] * axis[2], axis[1] * axis[2], axis[2] * axis[2]
		);
		return mat<T,3>(1) * cos + (T(1) - cos) * ee + e * sin;
	}
	quat<T> to_quaternion() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		T sin, cos;
		sincos(angle / T(2), sin, cos);
		return quat<T>(cos, axis[0] * sin, axis[1] * sin, axis[2] * sin);
	}

};

//
// line
//

template <class T, ::std::size_t N> struct line
{
	typedef typename vec<T,N>::float_type float_type;
	const vec<T,N> origin;
	const vec<T,N> direction;

	line(const vec<T,N>& origin, const vec<T,N>& direction) : origin(origin), direction(normalize(direction)) {}
	line(vec<T,N>&& origin, vec<T,N>&& direction) : origin(std::move(origin)), direction(std::move(normalize(direction))) {}

	vec<T,N> operator() (const T t) const { return origin + t * direction; }
};

template <class T, ::std::size_t N> std::ostream& operator << (std::ostream& os, const line<T,N>& line)
{
	return os << line.origin << " + t * " << line.direction;
}

template <class T, ::std::size_t N> inline T distance2(const line<T,N>& line, const vec<T,N>& point)
{
	const T dotproduct = (point - line.origin) * line.direction;
	return norm2(point - line.origin) - dotproduct * dotproduct;
}

template <class T, ::std::size_t N> inline T distance(const line<T,N>& line, const vec<T,N>& point)
{
	return sqrt(distance2(line, point));
}

template <class T, ::std::size_t N> inline T distance2(const line<T,N>& line1, const line<T,N>& line2)
{
	const T dotdirs = line1.direction * line2.direction;
	if (::std::abs(dotdirs) == T(1)) return distance2(line1.origin, line2.origin);
	else
	{
		const vec<T,N> diff = line2.origin - line1.origin;
		const T one_over_dots = inverse(dotdirs * dotdirs - T(1));
		const T t1 = one_over_dots * (dotdirs * (diff * line2.direction) - (diff * line1.direction));
		const T t2 = one_over_dots * ((diff * line2.direction) - dotdirs * (diff * line1.direction));
		return distance2(line1(t1), line2(t2));
	}
}

template <class T, ::std::size_t N> inline T distance(const line<T,N>& line1, const line<T,N>& line2)
{
	return sqrt(distance2(line1, line2));
}

template <class T, ::std::size_t N> vec<T,N> inline closest_point(const line<T,N>& line, const vec<T,N>& point)
{
	return line.origin - (line.direction * (line.origin - point)) * line.direction;
}

//
// plane
//

template <class T> struct plane : public vec<T,4>
{
	typedef typename vec<T,4>::float_type float_type;
	plane(const vec<T,3>& normal, const T signeddistance_from_origin)
	{
		(*this)[0] = normal[0]; (*this)[1] = normal[1]; (*this)[2] = normal[2];
		(*this)[3] = signeddistance_from_origin;
	}
	plane(const vec<T,3>& q1, const vec<T,3>& q2, const vec<T,3>& q3)
	{
		mat<T,3> qmat(q1, q2, q3);
		qmat.invert();
		const vec<T,3> N(normalize(qmat * vec<T,3>(T(1), T(1), T(1))));
		(*this)[0] = N[0];
		(*this)[1] = N[1];
		(*this)[2] = N[2];
		(*this)[3] = - N * q1;
	}
	vec<T,3> normal() const { return vec<T,3>((*this)[0], (*this)[1], (*this)[2]); }
	T distance_from_origin() const { return ::std::abs((*this)[3]); }
};

template <class T> std::ostream& operator << (std::ostream& os, const plane<T>& plane)
{
	return os << '(' << plane.normal() << ',' << plane[4] << ')';
}

template <class T> vec<T,3> intersection_point(const plane<T>& plane, const line<T,3>& line)
{
	BOOST_CONSTEXPR auto inf = std::numeric_limits<T>::infinity();
	if (line.normal() * plane.normal() == 0) return vec3f(inf, inf, inf);
}

template <typename T> using vec2 = vec<T,2>;
template <typename T> using vec3 = vec<T,3>;
template <typename T> using vec4 = vec<T,4>;
template <typename T> using mat2 = mat<T,2>;
template <typename T> using mat3 = mat<T,3>;
template <typename T> using mat4 = mat<T,4>;
template <typename T, std::size_t N> using box = boost::geometry::model::box<vec<T,N>>;
template <typename T> using box2 = box<T,2>;
template <typename T> using box3 = box<T,3>;
template <typename T> using box4 = box<T,4>;
template <typename T, std::size_t N> using linestring = boost::geometry::model::linestring<vec<T,N>>;
template <typename T> using linestring2 = linestring<T,2>;
template <typename T> using linestring3 = linestring<T,3>;
template <typename T> using linestring4 = linestring<T,4>;
// template <typename T, std::size_t N> using polygon = boost::geometry::model::polygon<vec<T,N>>;
// template <typename T> using polygon2 = polygon<T,2>;
// template <typename T> using polygon3 = polygon<T,3>;
// template <typename T> using polygon4 = polygon<T,4>;
// template <typename T, std::size_t N> using multi_polygon = boost::geometry::model::multi_polygon<vec<T,N>>;
// template <typename T> using multi_polygon2 = multi_polygon<T,2>;
// template <typename T> using multi_polygon3 = multi_polygon<T,3>;
// template <typename T> using multi_polygon4 = multi_polygon<T,4>;

using vec2f = vec2<float>;
using vec3f = vec3<float>;
using vec4f = vec4<float>;
using vec2d = vec2<double>;
using vec3d = vec3<double>;
using vec4d = vec4<double>;
using vec2i = vec2<int>;
using vec3i = vec3<int>;
using vec4i = vec4<int>;
using vec2st = vec2<std::size_t>;
using vec3st = vec3<std::size_t>;
using vec4st = vec4<std::size_t>;

using mat2f = mat2<float>;
using mat3f = mat3<float>;
using mat4f = mat4<float>;
using mat2d = mat2<double>;
using mat3d = mat3<double>;
using mat4d = mat4<double>;
using mat2i = mat2<int>;
using mat3i = mat3<int>;
using mat4i = mat4<int>;
using mat2st = mat2<std::size_t>;
using mat3st = mat3<std::size_t>;
using mat4st = mat4<std::size_t>;

using box2f = box2<float>;
using box3f = box3<float>;
using box4f = box4<float>;
using box2d = box2<double>;
using box3d = box3<double>;
using box4d = box4<double>;
using box2i = box2<int>;
using box3i = box3<int>;
using box4i = box4<int>;
using box2st = box2<std::size_t>;
using box3st = box3<std::size_t>;
using box4st = box4<std::size_t>;

// typedef mat<float,2>   mat2f;
// typedef mat<float,3>   mat3f;
// typedef mat<float,4>   mat4f;
// typedef mat<double,2>  mat2d;
// typedef mat<double,3>  mat3d;
// typedef mat<double,4>  mat4d;
typedef quat<float>    quatf;
typedef quat<double>   quatd;
typedef line<float,2>  line2f;
typedef line<float,3>  line3f;
typedef line<double,2> line2d;
typedef line<double,3> line3d;

template <class T> struct sqt_transform
{
	T scale;
	quat<T> rotation;
	vec<T,3> translation;

	sqt_transform() : scale(1), rotation(euler_xyz<T>(0, static_cast<T>(M_PI), 0)), translation(0,0,0) {}

	// mat<T,4> get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	// {
	//  mat<T,4> lResult = rotation.to_rotation_matrix();
	//  lResult *= mat<T,4>(scale);
	//  lResult(0,3) = translation[0];
	//  lResult(1,3) = translation[1];
	//  lResult(2,3) = translation[2];
	//  return lResult;
	// }
	mat<T,4> get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		mat<T,4> r = rotation.to_rotation_matrix();
		r(0,0) *= scale;
		r(0,1) *= scale;
		r(0,2) *= scale;
		r(1,0) *= scale;
		r(1,1) *= scale;
		r(1,2) *= scale;
		r(2,0) *= scale;
		r(2,1) *= scale;
		r(2,2) *= scale;
		r(0,3) = translation[0];
		r(1,3) = translation[1];
		r(2,3) = translation[2];
		return r;
	}

	// mat<T,4> get_inverse_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	// {
	//  mat<T,4> r = rotation.to_rotation_matrix().transpose();
	//  r(0,0) *= inverse(scale);
	//  r(1,1) *= inverse(scale);
	//  r(2,2) *= inverse(scale);
	//  r(0,3) = -translation[0];
	//  r(1,3) = -translation[1];
	//  r(2,3) = -translation[2];
	// }

	mat<T,3> get_non_translation_model_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		mat<T,3> r = rotation.to_rotation_matrix().upper_left_33();
		r *= mat<T,3>(scale);
		return r;
	}

	vec<T,3> apply_to(const vec<T,3>& v) const
	{
		return ConjugateWithvec(rotation, scale * v) + translation;
	}
};

typedef sqt_transform<float> sqt_transformf;
typedef sqt_transform<double> sqt_transformd;

template <class T> struct sssqt_transform
{
	vec<T,3> scale;
	quat<T> rotation;
	vec<T,3> translation;

	sssqt_transform() : scale(1,1,1), rotation(euler_xyz<T>(0, M_PI, 0)), translation(0,0,0) {}

	mat<T,4> get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		mat<T,4> r = rotation.to_rotation_matrix();
		r(0,0) *= scale[0];
		r(1,1) *= scale[1];
		r(2,2) *= scale[2];
		r(0,3) = translation[0];
		r(1,3) = translation[1];
		r(2,3) = translation[2];
		return r;
	}

	mat<T,3> get_non_translation_model_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		mat<T,3> r = rotation.to_rotation_matrix().upper_left_33();
		r *= mat<T,3>(scale);
		return r;
	}

	vec<T,3> apply_to(const vec<T,3>& v) const
	{
		return ConjugateWithvec(rotation, scale * v) + translation;
	}
};

typedef sssqt_transform<float> sssqt_transformf;
typedef sssqt_transform<double> sssqt_transformd;

template <class T> struct camera_transform
{
	enum class e_projection_type : char { orthographic, projective };
	e_projection_type projection_type;
	T horizontal_angle;
	T vertical_angle;
	T fake_zoom;
	T orthographic_zoom;
	T field_of_view;
	T near_plane;
	T far_plane;
	vec<T,3> position;
	vec<T,3> direction;
	vec<T,3> right;
	vec<T,3> up;
	void add_horizontal_and_vertical_angles(const T horizontal, const T vertical) BOOST_NOEXCEPT_OR_NOTHROW
	{
		BOOST_CONSTEXPR T lPi2 = T(M_PI / 2.0);
		BOOST_CONSTEXPR T l2Pi = T(2.0 * M_PI);
		horizontal_angle += horizontal;
		vertical_angle += vertical;
		horizontal_angle = std::remainder(horizontal_angle, l2Pi);
		if (vertical_angle < -lPi2) vertical_angle = -lPi2;
		else if (vertical_angle > lPi2) vertical_angle = lPi2;
		direction = vec<T,3>(cos(vertical_angle) * sin(horizontal_angle), sin(vertical_angle), cos(vertical_angle) * cos(horizontal_angle));
		right = vec<T,3>(sin(horizontal_angle - lPi2), 0, cos(horizontal_angle - lPi2));
		up = normalize(right % direction);
	}
	mat<T,4> get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return mat<T,4>(position, position + direction, up);
	}
	mat<T, 4> matrix_V() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return mat<T, 4>(position, position + direction, up);
	}
	mat<T, 4> matrix_P(const int width, const int height) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		switch (this->projection_type)
		{
		case e_projection_type::orthographic :
		{
			const T hw = static_cast<T>(width)  * T(0.5) / orthographic_zoom;
			const T hh = static_cast<T>(height) * T(0.5) / orthographic_zoom;
			return mat<T, 4>(-hw, hw, -hh, hh, near_plane, far_plane);
		}
		case e_projection_type::projective :
			return mat<T, 4>(field_of_view, static_cast<T>(width) / static_cast<T>(height), near_plane, far_plane);
		default:
			return mat<T, 4>(1.0f);
		}
	}
	void move_forward(const T amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		position += amount * direction;
	}
	void move_backward(const T amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		position -= amount * direction;
	}
	void move_left(const T amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		position -= amount * right;
	}
	void move_right(const T amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		position += amount * right;
	}
	void move_up(const T amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		position += amount * up;
	}
	void move_down(const T amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		position -= amount * up;
	}
};

struct SQT
{
	float scale;
	quatf rotation;
	vec3f translation;

	SQT() = default;

	SQT(const float scale, const quatf& rotation, const vec3f& translation)
	: scale(scale), rotation(rotation), translation(translation) {}

	mat4f get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		mat4f r = rotation.to_rotation_matrix();
		r(0,0) *= scale;
		r(0,1) *= scale;
		r(0,2) *= scale;
		r(1,0) *= scale;
		r(1,1) *= scale;
		r(1,2) *= scale;
		r(2,0) *= scale;
		r(2,1) *= scale;
		r(2,2) *= scale;
		r(0,3) = translation[0];
		r(1,3) = translation[1];
		r(2,3) = translation[2];
		return r;
	}

	SQT operator & (const SQT& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return SQT(scale * other.scale, rotation * other.rotation, translation + other.translation);
	}

	SQT& operator &= (const SQT& other) BOOST_NOEXCEPT_OR_NOTHROW
	{
		scale *= other.scale;
		rotation *= other.rotation;
		translation += other.translation;
		return *this;
	}
	// Example:
	//
	// SQT a, b;
	//
	// a.scale = 1;
	// a.rotation = quatf::from_angle_axis(M_PI, vec3f(0,1,0));
	// a.translation = vec3f(10,20,-40);
	//
	// b.scale = 0.5f;
	// b.rotation = quatf(0,1,0,0);
	// b.translation = vec3f(1,5,-19);
	//
	// SQT c = a & b;
};

struct SQT_camera : public SQT
{
	enum class e_projection_type : char { orthographic, projective };
	e_projection_type projection_type;
	float horizontal_angle;
	float vertical_angle;
	float fake_zoom;
	float orthographic_zoom;
	float field_of_view;
	float near_plane;
	float far_plane;

	mat4f get_projection_matrix(const int width, const int height) 
		const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return mat4f(1.0f);
	}
};

template <class T, std::size_t N> struct sphere
{
	typedef T float_type;
	vec<float_type,N> center;
	float_type radius;
	inline BOOST_CONSTEXPR sphere() : center(), radius() {}
	template <class U1, class U2> inline BOOST_CONSTEXPR sphere(U1&& u1, U2&& u2)
	: center(std::forward<U1>(u1)), radius(std::forward<U2>(u2)) {}
};

template <class S, class T, std::size_t N> bool intersect(const sphere<S,N>& a, const sphere<T,N>& b)
{
	// if they "touch", then we say that they intersect too.
	// That's why we use <= instead of <.
	return boost::geometry::distance(a.center, b.center) <= (a.radius + b.radius);
}

} // end of namespace gintonic

#ifndef DOXYGEN_NO_TRAITS_SPECIALIZATIONS
namespace boost { namespace geometry { namespace traits {

template <typename T, std::size_t N> struct tag<gintonic::vec<T,N>> { typedef point_tag type; };

template <typename T, std::size_t N> struct coordinate_type<gintonic::vec<T,N>> { typedef T type; };

template <typename T, std::size_t N> struct coordinate_system<gintonic::vec<T,N>> { typedef cs::cartesian type; };

template <typename T, std::size_t N> struct dimension<gintonic::vec<T,N>> : boost::mpl::int_<N> {};

template <typename T, std::size_t D, std::size_t N>
struct access<gintonic::vec<T,N>, D>
{
	static inline T get(gintonic::vec<T,N> const& p)
	{
		static_assert(D < N, "Index must be less than the size of the vector.");
		return p[D];
	}
	static inline void set(gintonic::vec<T,N>& p, T const& value)
	{
		static_assert(D < N, "Index must be less than the size of the vector.");
		p[D] = value;
	}
};

}}} // end of namespace ::boost::geometry::traits
#endif // DOXYGEN_NO_TRAITS_SPECIALIZATIONS

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#endif