#ifndef gintonic_mat4f_hpp
#define gintonic_mat4f_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

union vec3f; // Forward declaration.
union vec4f; // Forward declaration.
union quatf; // Forward declaration.
union mat3f; // Forward declaration.
struct SQT;  // Forward declaration.

/*****************************************************************************
* gintonic::mat4f, column-major storage                                      *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union mat4f
{
public:
	__m128 data[4];

	struct
	{
		float m00;
		float m10;
		float m20;
		float m30;

		float m01;
		float m11;
		float m21;
		float m31;

		float m02;
		float m12;
		float m22;
		float m32;

		float m03;
		float m13;
		float m23;
		float m33;
	};

	inline float* value_ptr() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return &m00;
	}

	inline const float* value_ptr() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return &m00;
	}

	mat4f() = default;

	mat4f(const float diagonal)
	: m00(diagonal), m10(0.0f), m20(0.0f), m30(0.0f)
	, m01(0.0f), m11(diagonal), m21(0.0f), m31(0.0f)
	, m02(0.0f), m12(0.0f), m22(diagonal), m32(0.0f)
	, m03(0.0f), m13(0.0f), m23(0.0f), m33(diagonal)
	{
		/* Empty on purpose. */
	}

	mat4f(const float scalex, const float scaley, const float scalez)
	: m00(scalex), m10(0.0f), m20(0.0f), m30(0.0f)
	, m01(0.0f), m11(scaley), m21(0.0f), m31(0.0f)
	, m02(0.0f), m12(0.0f), m22(scalez), m32(0.0f)
	, m03(0.0f), m13(0.0f), m23(0.0f), m33(1.0f)
	{
		/* Empty on purpose. */
	}

	mat4f(const quatf& rotation);

	mat4f(const vec3f& translation);

	mat4f(const SQT& transform);

	mat4f(const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33)
	: m00(m00), m10(m10), m20(m20), m30(m30)
	, m01(m01), m11(m11), m21(m21), m31(m31)
	, m02(m02), m12(m12), m22(m22), m32(m32)
	, m03(m03), m13(m13), m23(m23), m33(m33)
	{
		/* Empty on purpose. */
	}

	mat4f(const __m128& column0, const __m128& column1, const __m128& column2, const __m128& column3)
	: data{column0, column1, column2, column3}
	{
		/* Empty on purpose. */
	}

	mat4f(const mat3f& rotation_part);

	mat4f(const vec4f& column0, const vec4f& column1, const vec4f& column2, const vec4f& column3);

	mat4f(const vec3f& rotation_axis, const float rotation_angle);

	mat4f(const vec3f& eye_location, const vec3f& subject_location, const vec3f& up_direction);

	mat4f& set_orthographic(const float left, const float right, const float bottom, const float top, const float nearplane, const float farplane);
	mat4f& set_orthographic(const float width, const float height, const float nearplane, const float farplane);
	mat4f& set_perspective(const float fieldofview, const float aspectratio, const float nearplane, const float farplane);
	mat4f& set_perspective_infinite(const float fieldofview, const float aspectratio, const float nearplane);
	mat4f& set_inverse_perspective(const float fieldofview, const float aspectratio, const float nearplane, const float farplane);
	void unproject_perspective(float& fieldofview, float& aspectratio, float& nearplane, float& farplane);

	inline mat4f& operator += (const mat4f& other)
	{
		data[0] = _mm_add_ps(data[0], other.data[0]);
		data[1] = _mm_add_ps(data[1], other.data[1]);
		data[2] = _mm_add_ps(data[2], other.data[2]);
		data[3] = _mm_add_ps(data[3], other.data[3]);
		return *this;
	}

	inline mat4f& operator -= (const mat4f& other)
	{
		data[0] = _mm_sub_ps(data[0], other.data[0]);
		data[1] = _mm_sub_ps(data[1], other.data[1]);
		data[2] = _mm_sub_ps(data[2], other.data[2]);
		data[3] = _mm_sub_ps(data[3], other.data[3]);
		return *this;
	}

	mat4f& operator *= (const mat4f& other);

	inline mat4f operator + (const mat4f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return mat4f(
			_mm_add_ps(data[0], other.data[0]), 
			_mm_add_ps(data[1], other.data[1]),
			_mm_add_ps(data[2], other.data[2]),
			_mm_add_ps(data[3], other.data[3]));
	}

	inline mat4f operator + (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return *this + mat4f(s);
	}

	inline mat4f operator - (const mat4f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return mat4f(
			_mm_sub_ps(data[0], other.data[0]), 
			_mm_sub_ps(data[1], other.data[1]),
			_mm_sub_ps(data[2], other.data[2]),
			_mm_sub_ps(data[3], other.data[3]));
	}

	inline mat4f operator - (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return *this - mat4f(s);
	}

	vec4f operator * (const vec4f& v) const;

	mat4f operator * (const mat4f& other) const;

	inline mat4f operator * (const float s) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto tmp = _mm_set1_ps(s);
		return mat4f(
			_mm_mul_ps(data[0], tmp),
			_mm_mul_ps(data[1], tmp),
			_mm_mul_ps(data[2], tmp),
			_mm_mul_ps(data[3], tmp));
	}

	inline friend mat4f operator * (const float s, const mat4f& m) BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto tmp = _mm_set1_ps(s);
		return mat4f(
			_mm_mul_ps(tmp, m.data[0]),
			_mm_mul_ps(tmp, m.data[1]),
			_mm_mul_ps(tmp, m.data[2]),
			_mm_mul_ps(tmp, m.data[3]));
	}

	inline mat4f& transpose() BOOST_NOEXCEPT_OR_NOTHROW
	{
		std::swap(m01, m10);
		std::swap(m02, m20);
		std::swap(m03, m30);
		std::swap(m12, m21);
		std::swap(m13, m31);
		std::swap(m23, m32);
		return *this;
	}

	vec3f apply_to_point(const vec3f& point) const BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f apply_to_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW;

	mat3f upper_left_33() const;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(m00) & BOOST_SERIALIZATION_NVP(m10) 
			& BOOST_SERIALIZATION_NVP(m20) & BOOST_SERIALIZATION_NVP(m30) 
			& BOOST_SERIALIZATION_NVP(m01) & BOOST_SERIALIZATION_NVP(m11) 
			& BOOST_SERIALIZATION_NVP(m21) & BOOST_SERIALIZATION_NVP(m31) 
			& BOOST_SERIALIZATION_NVP(m02) & BOOST_SERIALIZATION_NVP(m12) 
			& BOOST_SERIALIZATION_NVP(m22) & BOOST_SERIALIZATION_NVP(m32) 
			& BOOST_SERIALIZATION_NVP(m03) & BOOST_SERIALIZATION_NVP(m13) 
			& BOOST_SERIALIZATION_NVP(m23) & BOOST_SERIALIZATION_NVP(m33);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline std::ostream& operator << (std::ostream& os, const mat4f& m)
{
	return os << m.m00 << ' ' << m.m10 << ' ' << m.m20 << ' ' << m.m30 << ' '
		<< m.m01 << ' ' << m.m11 << ' ' << m.m21 << ' ' << m.m31 << ' '
		<< m.m02 << ' ' << m.m12 << ' ' << m.m22 << ' ' << m.m32 << ' '
		<< m.m03 << ' ' << m.m13 << ' ' << m.m23 << ' ' << m.m33;
}

inline std::istream& operator >> (std::istream& is, mat4f& m)
{
	is >> m.m00 >> m.m10 >> m.m20 >> m.m30
		>> m.m01 >> m.m11 >> m.m21 >> m.m31
		>> m.m02 >> m.m12 >> m.m22 >> m.m32
		>> m.m03 >> m.m13 >> m.m23 >> m.m33;
	return is;
}

} // namespace gintonic

#endif