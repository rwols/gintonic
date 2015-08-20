#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "vec3f.hpp"

namespace gintonic {

union mat4f; // Forward declaration.

struct camera
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
	
	vec3f position;
	vec3f direction;
	vec3f right;
	vec3f up;

	void add_horizontal_and_vertical_angles(const float horizontal, const float vertical) BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f matrix_V() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f matrix_P(const int width, const int height) const BOOST_NOEXCEPT_OR_NOTHROW;
	void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif