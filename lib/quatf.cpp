#include "quatf.hpp"

namespace gintonic {

quatf quatf::axis_angle(const vec3f& rotation_axis, const float rotation_angle)
{
	const auto s = std::sin(rotation_angle * 0.5f);
	const auto c = std::cos(rotation_angle * 0.5f);
	return quatf(c, s * rotation_axis);
}

quatf quatf::look_at(vec3f eye_position, vec3f subject_position, const vec3f& up_direction)
{
	const auto dotproduct = dot(eye_position.normalize(), subject_position.normalize());
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
		const auto rotation_axis = cross(subject_position, up_direction).normalize();
		const auto rotation_angle = std::acos(dotproduct);
		return axis_angle(rotation_axis, rotation_angle);
	}
}

} // namespace gintonic