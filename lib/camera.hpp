#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "vec2f.hpp"
#include "vec3f.hpp"
#include "mat4f.hpp"

namespace gintonic {

class entity; // forward declaration.

class proj_info
{
public:

	proj_info() = default;

	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};

	projection_type projection = kPerspectiveProjection;

	float fieldofview = 1.22173048f;

	mat4f matrix;

	void update() BOOST_NOEXCEPT_OR_NOTHROW;
};

// class camera
// {
// public:

// 	vec2f angles;

// 	vec3f right;
// 	vec3f up;
// 	vec3f forward;

// 	mat4f matrix;

// 	void update(const vec2f& mousedelta) BOOST_NOEXCEPT_OR_NOTHROW;
// };

class camera
{
public:

	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};

	projection_type projection = kPerspectiveProjection;

	float fieldofview = 1.22173048f;
	float orthographic_zoom = 1.0f;

	camera(entity& entity_to_control);

	entity* controlling_entity;

	float horizontal_angle;
	float vertical_angle;

	vec3f direction;
	vec3f up;
	vec3f right;
	vec3f position;

	void add_horizontal_and_vertical_angles(const float horizontal, const float vertical) BOOST_NOEXCEPT_OR_NOTHROW;
	quatf get_rotation() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f matrix_V() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f matrix_P() const BOOST_NOEXCEPT_OR_NOTHROW;
	void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif