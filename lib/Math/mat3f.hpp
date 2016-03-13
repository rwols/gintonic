/**
 * @file mat3f.hpp
 * @brief Defines a three by three matrix.
 * @author Raoul Wols
 */

#ifndef gintonic_mat3f_hpp
#define gintonic_mat3f_hpp

#include "../Foundation/utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

union vec3f; // Forward declaration.
union quatf; // Forward declaration.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

/**
 * @brief Three by three matrix, column major storage.
 */
union GINTONIC_ALIGNED(16) mat3f
{
public:

	/// The raw data array.
	float data[9];

	struct
	{
		/// Entry (0,0)
		float m00;
		/// Entry (1,0)
		float m10;
		/// Entry (2,0)
		float m20;

		/// Entry (0,1)
		float m01;
		/// Entry (1,1)
		float m11;
		/// Entry (2,1)
		float m21;

		/// Entry (0,2)
		float m02;
		/// Entry (1,2)
		float m12;
		/// Entry (2,2)
		float m22;
	};

	/// Get a value pointer to the raw array.
	float* value_ptr() noexcept
	{
		return &m00;
	}

	/// Get a const value pointer to the raw array.
	const float* value_ptr() const noexcept
	{
		return &m00;
	}

	/// Default constructor.
	mat3f() = default;

	/// Constructor that builds a diagonal matrix.
	mat3f(const float diagonal)
	: m00(diagonal), m10(0.0f), m20(0.0f)
	, m01(0.0f), m11(diagonal), m21(0.0f)
	, m02(0.0f), m12(0.0f), m22(diagonal)
	{
		/* Empty on purpose. */
	}

	/// Constructor that builds a diagonal matrix.
	mat3f(const float scalex, const float scaley, const float scalez)
	: m00(scalex), m10(0.0f), m20(0.0f)
	, m01(0.0f), m11(scaley), m21(0.0f)
	, m02(0.0f), m12(0.0f), m22(scalez)
	{
		/* Empty on purpose. */
	}

	/// Constructor that builds a rotation matrix.
	mat3f(const quatf& rotation);

	/// Constructor for setting every entry.
	mat3f(const float m00, const float m01, const float m02,
		const float m10, const float m11, const float m12,
		const float m20, const float m21, const float m22)
	: m00(m00), m10(m10), m20(m20)
	, m01(m01), m11(m11), m21(m21)
	, m02(m02), m12(m12), m22(m22)
	{
		/* Empty on purpose. */
	}

	/// Column constructor.
	mat3f(const vec3f& column0, const vec3f& column1, const vec3f& column2);

	/// Axis-angle constructor.
	mat3f(const vec3f& rotation_axis, const float rotation_angle);

	/// Apply this matrix to a vector.
	vec3f operator * (const vec3f&) const noexcept;

	/// Multiply two matrices.
	mat3f operator * (const mat3f&) const noexcept;

	/// Invert this matrix.
	mat3f& invert() noexcept;

	/// Tranpose this matrix.
	mat3f& transpose() noexcept;

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace gintonic

#endif