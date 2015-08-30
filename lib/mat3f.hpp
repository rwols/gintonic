#ifndef gintonic_mat3f_hpp
#define gintonic_mat3f_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

union vec3f; // Forward declaration.
union quatf; // Forward declaration.

/*****************************************************************************
* gintonic::mat3f, column-major storage                                      *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union GINTONIC_ALIGNED(16) mat3f
{
public:
	float data[9];

	struct
	{
		float m00;
		float m10;
		float m20;

		float m01;
		float m11;
		float m21;

		float m02;
		float m12;
		float m22;
	};

	float* value_ptr() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return &m00;
	}

	const float* value_ptr() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return &m00;
	}

	mat3f() = default;

	mat3f(const float diagonal)
	: m00(diagonal), m10(0.0f), m20(0.0f)
	, m01(0.0f), m11(diagonal), m21(0.0f)
	, m02(0.0f), m12(0.0f), m22(diagonal)
	{
		/* Empty on purpose. */
	}


	mat3f(const float scalex, const float scaley, const float scalez)
	: m00(scalex), m10(0.0f), m20(0.0f)
	, m01(0.0f), m11(scaley), m21(0.0f)
	, m02(0.0f), m12(0.0f), m22(scalez)
	{
		/* Empty on purpose. */
	}

	mat3f(const quatf& rotation);

	mat3f(const float m00, const float m01, const float m02,
		const float m10, const float m11, const float m12,
		const float m20, const float m21, const float m22)
	: m00(m00), m10(m10), m20(m20)
	, m01(m01), m11(m11), m21(m21)
	, m02(m02), m12(m12), m22(m22)
	{
		/* Empty on purpose. */
	}

	mat3f(const vec3f& column0, const vec3f& column1, const vec3f& column2);

	mat3f(const vec3f& rotation_axis, const float rotation_angle);

	vec3f operator * (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

	mat3f operator * (const mat3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

	mat3f& invert() BOOST_NOEXCEPT_OR_NOTHROW;

	mat3f& transpose() BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace gintonic

#endif