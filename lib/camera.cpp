#include "camera.hpp"
#include "mat4f.hpp"

namespace gintonic {

void camera::add_horizontal_and_vertical_angles(const float horizontal, const float vertical) BOOST_NOEXCEPT_OR_NOTHROW
{
	BOOST_CONSTEXPR float lPi2 = float(M_PI / 2.0);
	BOOST_CONSTEXPR float l2Pi = float(2.0 * M_PI);
	
	horizontal_angle += horizontal;
	vertical_angle += vertical;
	horizontal_angle = std::remainder(horizontal_angle, l2Pi);
	
	if (vertical_angle < -lPi2) vertical_angle = -lPi2;
	else if (vertical_angle > lPi2) vertical_angle = lPi2;
	
	direction = vec3f(
		std::cos(vertical_angle) * std::sin(horizontal_angle), 
		std::sin(vertical_angle), 
		std::cos(vertical_angle) * std::cos(horizontal_angle));

	right = vec3f(std::sin(horizontal_angle - lPi2), 0, std::cos(horizontal_angle - lPi2));
	
	up = cross(right, direction).normalize();
}

mat4f camera::get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return mat4f(position, position + direction, up);
}
mat4f camera::matrix_V() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return mat4f(position, position + direction, up);
}
mat4f camera::matrix_P(const int width, const int height) const BOOST_NOEXCEPT_OR_NOTHROW
{
	switch (this->projection_type)
	{
	case e_projection_type::orthographic :
	{
		const float hw = static_cast<float>(width)  * float(0.5) / orthographic_zoom;
		const float hh = static_cast<float>(height) * float(0.5) / orthographic_zoom;
		mat4f r;
		r.set_orthographic(-hw, hw, -hh, hh, near_plane, far_plane);
		return r;
	}
	case e_projection_type::projective :
		mat4f r;
		r.set_perspective(field_of_view, static_cast<float>(width) / static_cast<float>(height), near_plane, far_plane);
		return r;
	default:
		return mat4f(1.0f);
	}
}
void camera::move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	position += amount * direction;
}
void camera::move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	position -= amount * direction;
}
void camera::move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	position -= amount * right;
}
void camera::move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	position += amount * right;
}
void camera::move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	position += amount * up;
}
void camera::move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	position -= amount * up;
}

} // namespace gintonic