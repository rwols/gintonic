#include "camera.hpp"
#include "mat4f.hpp"
#include "renderer.hpp"
#include "entity.hpp"

namespace gintonic {

void proj_info::update() BOOST_NOEXCEPT_OR_NOTHROW
{
	// fieldofview = 60.0f;
	switch (projection)
	{
		case kOrthographicProjection:
			matrix.set_orthographic(
				static_cast<float>(renderer::width()), 
				static_cast<float>(renderer::height()), 
				0.1f, 
				100.0f);
			break;
		case kPerspectiveProjection:
			matrix.set_perspective(
				fieldofview, 
				renderer::aspectratio(), 
				0.1f, 
				100.0f);
			break;
		default:
			matrix.set_perspective(
				fieldofview, 
				renderer::aspectratio(), 
				0.1f, 
				100.0f);
	}
}

camera::camera(entity& entity_to_control)
: m_controlling_entity(&entity_to_control)
{
	if (m_controlling_entity->camera_component)
	{
		throw std::runtime_error("Camera already attached to this entity.");
	}
	m_controlling_entity->camera_component = this;
}

#define M_PIf2 static_cast<float>(M_PI) * 0.5f
#define M_2PIf static_cast<float>(M_PI) * 2.0f

void camera::add_mouse(const vec2f& amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	angles += amount;

	angles.x = std::remainder(angles.x, M_2PIf);

	if (angles.y < -M_PIf2) angles.y = -M_PIf2;
	else if (angles.y > M_PIf2) angles.y = M_PIf2;
	
	m_controlling_entity->set_rotation(quatf::mouse(angles));
}

void camera::set_controlling_entity(entity& new_ent)
{
	m_controlling_entity->camera_component = nullptr;
	m_controlling_entity = &new_ent;
	new_ent.camera_component = this;
}

// mat4f camera::get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	return mat4f(position, position + direction, up);
// }

// quatf camera::get_rotation() const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	quatf q;

// 	q.w = ( right.x + up.y + direction.z + 1.0f) / 4.0f;
// 	q.x = ( right.x - up.y - direction.z + 1.0f) / 4.0f;
// 	q.y = (-right.x + up.y - direction.z + 1.0f) / 4.0f;
// 	q.z = (-right.x - up.y + direction.z + 1.0f) / 4.0f;
	
// 	if (q.w < 0.0f) q.w = 0.0f;
// 	if (q.x < 0.0f) q.x = 0.0f;
// 	if (q.y < 0.0f) q.y = 0.0f;
// 	if (q.z < 0.0f) q.z = 0.0f;

// 	q.data = _mm_sqrt_ps(q.data);

// 	#define SIGN(x) (x) < 0.0f ? -1.0f : 1.0f
	
// 	if (q.w >= q.x && q.w >= q.y && q.w >= q.z) 
// 	{
// 		// q.w *= +1.0f;
// 		q.x *= SIGN(up.z - direction.y);
// 		q.y *= SIGN(direction.x - right.z);
// 		q.z *= SIGN(right.y - up.x);
// 	} 
// 	else if (q.x >= q.w && q.x >= q.y && q.x >= q.z) 
// 	{
// 		q.w *= SIGN(up.z - direction.y);
// 		// q.x *= +1.0f;
// 		q.y *= SIGN(right.y + up.x);
// 		q.z *= SIGN(direction.x + right.z);
// 	} 
// 	else if (q.y >= q.w && q.y >= q.x && q.y >= q.z) 
// 	{
// 		q.w *= SIGN(direction.x - right.z);
// 		q.x *= SIGN(right.y + up.x);
// 		// q.y *= +1.0f;
// 		q.z *= SIGN(up.z + direction.y);
// 	} 
// 	else if (q.z >= q.w && q.z >= q.x && q.z >= q.y) 
// 	{
// 		q.w *= SIGN(right.y - up.x);
// 		q.x *= SIGN(right.z + direction.x);
// 		q.y *= SIGN(up.z + direction.y);
// 		// q.z *= +1.0f;
// 	}

// 	#undef SIGN

// 	return q.normalize();
// }

// mat4f camera::matrix_V() const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	return mat4f(position, position + direction, up);
// }

// #define NEARPLANE 0.1f
// #define FARPLANE 100.0f

// mat4f camera::matrix_P() const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	switch (this->projection)
// 	{
// 	case kOrthographicProjection:
// 	{
// 		const float hw = static_cast<float>(renderer::width())  * float(0.5) / orthographic_zoom;
// 		const float hh = static_cast<float>(renderer::height()) * float(0.5) / orthographic_zoom;
// 		mat4f r;
// 		r.set_orthographic(-hw, hw, -hh, hh, NEARPLANE, FARPLANE);
// 		return r;
// 	}
// 	case kPerspectiveProjection:
// 		mat4f r;
// 		r.set_perspective(fieldofview, renderer::aspectratio(), NEARPLANE, FARPLANE);
// 		return r;
// 	default:
// 		return mat4f(1.0f);
// 	}
// }
// void camera::move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	position += amount * direction;
// 	controlling_entity->set_translation(position);
// }
// void camera::move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	position -= amount * direction;
// 	controlling_entity->set_translation(position);
// }
// void camera::move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	position -= amount * right;
// 	controlling_entity->set_translation(position);
// }
// void camera::move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	position += amount * right;
// 	controlling_entity->set_translation(position);
// }
// void camera::move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	position += amount * up;
// 	controlling_entity->set_translation(position);
// }
// void camera::move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	position -= amount * up;
// 	controlling_entity->set_translation(position);
// }

} // namespace gintonic