#include "entity.hpp"
#include "SQTstack.hpp"
#include "mat4fstack.hpp"
#include "mesh.hpp"
#include "lights.hpp"
#include "materials.hpp"
#include "octree.hpp"
#include "renderer.hpp"
#include "component.hpp"

namespace gintonic {

entity::entity(
	const SQT&   local_transform,
	const box3f& local_bounding_box,
	octree*      octree_root,
	entity*      parent)
: m_local_transform(local_transform)
, m_local_bounding_box(local_bounding_box)
, m_parent(parent)
{
	update_global_info_start();
	if (m_parent) m_parent->m_children.push_front(this);
	if (octree_root) octree_root->insert(this);
}

entity::entity(entity&& other) noexcept
: m_local_transform(std::move(other.m_local_transform))
, m_global_transform(std::move(other.m_global_transform))
, m_local_bounding_box(std::move(other.m_local_bounding_box))
, m_global_bounding_box(std::move(other.m_global_bounding_box))
, m_children(std::move(other.m_children))
, m_parent(std::move(other.m_parent))
, m_octree(std::move(other.m_octree))
, m_mesh_component(std::move(other.m_mesh_component))
, m_material_component(std::move(other.m_material_component))
, m_light_component(std::move(other.m_light_component))
, m_rigid_body_component(std::move(other.m_rigid_body_component))
, m_logic_component(std::move(other.m_logic_component))
, m_AI_component(std::move(other.m_AI_component))
, m_camera_component(std::move(other.m_camera_component))
, m_proj_info_component(std::move(other.m_proj_info_component))
{
	if (m_parent)
	{
		m_parent->m_children.push_front(this);
	}
}

entity& entity::operator = (entity&& other) noexcept
{
	m_local_transform = std::move(other.m_local_transform);
	m_global_transform = std::move(other.m_global_transform);
	m_local_bounding_box = std::move(other.m_local_bounding_box);
	m_global_bounding_box = std::move(other.m_global_bounding_box);
	m_children = std::move(other.m_children);
	m_parent = std::move(other.m_parent);
	m_octree = std::move(other.m_octree);
	m_mesh_component = std::move(other.m_mesh_component);
	m_material_component = std::move(other.m_material_component);
	m_light_component = std::move(other.m_light_component);
	m_rigid_body_component = std::move(other.m_rigid_body_component);
	m_logic_component = std::move(other.m_logic_component);
	m_AI_component = std::move(other.m_AI_component);
	m_camera_component = std::move(other.m_camera_component);
	m_proj_info_component = std::move(other.m_proj_info_component);

	return *this;
}

void entity::update_global_info(mat4fstack& matrix_stack) noexcept
{
	matrix_stack.push(m_local_transform);
	update_global_datamembers(matrix_stack);
	for (auto* child : m_children)
	{
		if (child) child->update_global_info(matrix_stack);
	}
	matrix_stack.pop();
}

void entity::update_global_info_start() noexcept
{
	mat4fstack matrix_stack(compute_global_transform());
	update_global_datamembers(matrix_stack);
	for (auto* child : m_children)
	{
		if (child) child->update_global_info(matrix_stack);
	}
}

void entity::update_global_datamembers(const mat4fstack& matrix_stack) noexcept
{
	m_global_transform = matrix_stack.top();
	const vec3f t(m_global_transform.data[3]);
	m_global_bounding_box.min_corner = t + m_local_bounding_box.min_corner;
	m_global_bounding_box.max_corner = t + m_local_bounding_box.max_corner;
	if (m_octree) m_octree->notify(this);
}

void entity::set_scale(const vec3f& scale) noexcept
{
	m_local_transform.scale = scale;
	update_global_info_start();
	transform_changed(*this);
}

void entity::multiply_scale(const vec3f& scale) noexcept
{
	m_local_transform.scale *= scale;
	update_global_info_start();
	transform_changed(*this);
}

void entity::set_translation(const vec3f& translation) noexcept
{
	m_local_transform.translation = translation;
	update_global_info_start();
	transform_changed(*this);
}

void entity::set_translation_x(const float x) noexcept
{
	m_local_transform.translation.x = x;
	update_global_info_start();
	transform_changed(*this);
}
void entity::set_translation_y(const float y) noexcept
{
	m_local_transform.translation.y = y;
	update_global_info_start();
	transform_changed(*this);
}

void entity::set_translation_z(const float z) noexcept
{
	m_local_transform.translation.z = z;
	update_global_info_start();
	transform_changed(*this);
}

void entity::add_translation(const vec3f& translation) noexcept
{
	m_local_transform.translation += translation;
	update_global_info_start();
	transform_changed(*this);
}

void entity::set_rotation(const quatf& rotation) noexcept
{
	m_local_transform.rotation = rotation;
	update_global_info_start();
	transform_changed(*this);
}

void entity::post_multiply_rotation(const quatf& rotation) noexcept
{
	m_local_transform.rotation *= rotation;
	update_global_info_start();
	transform_changed(*this);
}

void entity::pre_multiply_rotation(const quatf& rotation) noexcept
{
	m_local_transform.rotation = rotation * m_local_transform.rotation;
	update_global_info_start();
	transform_changed(*this);
}

void entity::set_local_transform(const SQT& sqt) noexcept
{
	m_local_transform = sqt;
	update_global_info_start();
	transform_changed(*this);
}

void entity::post_add_local_transform(const SQT& sqt) noexcept
{
	m_local_transform %= sqt;
	update_global_info_start();
	transform_changed(*this);
}

void entity::pre_add_local_transform(const SQT& sqt) noexcept
{
	m_local_transform = sqt % m_local_transform;
	update_global_info_start();
	transform_changed(*this);
}

void entity::move_forward(const float amount) noexcept
{
	m_local_transform.translation += amount * m_local_transform.rotation.forward_direction();
	update_global_info_start();
	transform_changed(*this);
}

void entity::move_backward(const float amount) noexcept
{
	m_local_transform.translation -= amount * m_local_transform.rotation.forward_direction();
	update_global_info_start();
	transform_changed(*this);
}

void entity::move_right(const float amount) noexcept
{
	m_local_transform.translation += amount * m_local_transform.rotation.right_direction();
	update_global_info_start();
	transform_changed(*this);
}

void entity::move_left(const float amount) noexcept
{
	m_local_transform.translation -= amount * m_local_transform.rotation.right_direction();
	update_global_info_start();
	transform_changed(*this);
}

void entity::move_up(const float amount) noexcept
{
	m_local_transform.translation += amount * m_local_transform.rotation.up_direction();
	update_global_info_start();
	transform_changed(*this);
}

void entity::move_down(const float amount) noexcept
{
	m_local_transform.translation -= amount * m_local_transform.rotation.up_direction();
	update_global_info_start();
	transform_changed(*this);
}

mat4f entity::compute_global_transform() noexcept
{
	mat4f global_matrix(m_local_transform);
	const auto* current_entity = m_parent;
	while (current_entity)
	{
		if (current_entity == this) break; // Cycle in the entity graph.
		global_matrix = mat4f(current_entity->m_local_transform) * global_matrix;
		current_entity = current_entity->m_parent;
	}
	return global_matrix;
}

void entity::add_child(entity& c)
{
	c.m_parent = this;
	m_children.push_front(&c);
	c.update_global_info_start();
	c.transform_changed(c);
}

void entity::remove_child(entity& c)
{
	#ifndef NDEBUG
	bool child_was_removed = false;
	#endif

	for (auto i = begin(); i != end(); ++i)
	{
		if (*i == &c)
		{
			m_children.erase(i);

			#ifndef NDEBUG
			child_was_removed = true;
			#endif

			break;
		}
	}

	#ifndef NDEBUG
	if (child_was_removed) c.m_parent = nullptr;
	else throw std::logic_error("Entity was not a child.");
	#else
	c.m_parent = nullptr;
	#endif
}

void entity::set_parent(entity& p)
{
	if (m_parent) m_parent->remove_child(*this);
	p.add_child(*this);
}

void entity::unset_parent()
{
	if (m_parent) m_parent->remove_child(*this); // Also sets m_parent to nullptr
}

entity::~entity()
{
	about_to_die(*this);
	if (m_octree) m_octree->erase(this);
	unset_parent();
}

} // namespace gintonic