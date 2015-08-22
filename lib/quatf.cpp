#include "quatf.hpp"

namespace gintonic {

quatf quatf::axis_angle(const vec3f& rotation_axis, const float rotation_angle)
{
	const auto s = std::sin(rotation_angle * 0.5f);
	const auto c = std::cos(rotation_angle * 0.5f);
	return quatf(c, s * rotation_axis);
}

float quatf::length2() const BOOST_NOEXCEPT_OR_NOTHROW
{
	__m128 l = _mm_mul_ps(data, data);
	__m128 k = _mm_hadd_ps(l, l);
	return _mm_cvtss_f32(_mm_hadd_ps(k, k));
}

quatf quatf::look_at(const vec3f& eye_position, const vec3f& subject_position, const vec3f& up_direction)
{
	auto eye_copy = eye_position;
	auto up_copy = up_direction;
	const auto dotproduct = dot(eye_copy.normalize(), up_copy.normalize());
	if (almost_equal(dotproduct, 1.0f, 5))
	{
		return quatf(1.0f, 0.0f, 0.0f, 0.0f);
	}
	else if (almost_equal(dotproduct, -1.0f, 5))
	{
		return quatf(-1.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		const auto rotation_axis = cross(subject_position, up_copy).normalize();
		const auto rotation_angle = std::acos(dotproduct);
		return axis_angle(rotation_axis, rotation_angle);
	}
}

} // namespace gintonic