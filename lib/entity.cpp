#include "entity.hpp"
#include "SQTstack.hpp"
#include "mesh.hpp"
#include "lights.hpp"
#include "materials.hpp"
#include "octree.hpp"
#include "renderer.hpp"

namespace gintonic {

entity::entity(
	const SQT&   local_transform,
	const box3f& local_bounding_box,
	octree*      octree_root,
	entity*      parent,
	mesh*        mesh_component,
	material*    material_component,
	light*       light_component,
	rigid_body*  rigid_body_component,
	logic*       logic_component,
	AI*          AI_component,
	camera*      camera_component,
	proj_info*   proj_info_component)
: m_local_transform(local_transform)
, m_local_bounding_box(local_bounding_box)
, m_parent(parent)
, mesh_component(mesh_component)
, material_component(material_component)
, light_component(light_component)
, rigid_body_component(rigid_body_component)
, logic_component(logic_component)
, AI_component(AI_component)
, camera_component(camera_component)
, proj_info_component(proj_info_component)
{
	update_global_info_start();
	if (octree_root) octree_root->insert(this);
}

entity::entity(entity&& other) BOOST_NOEXCEPT_OR_NOTHROW
: m_local_transform(std::move(other.m_local_transform))
, m_global_transform(std::move(other.m_global_transform))
, m_local_bounding_box(std::move(other.m_local_bounding_box))
, m_global_bounding_box(std::move(other.m_global_bounding_box))
, m_children(std::move(other.m_children))
, m_parent(std::move(other.m_parent))
, m_octree(std::move(other.m_octree))
, mesh_component(std::move(other.mesh_component))
, material_component(std::move(other.material_component))
, light_component(std::move(other.light_component))
, rigid_body_component(std::move(other.rigid_body_component))
, logic_component(std::move(other.logic_component))
, AI_component(std::move(other.AI_component))
, camera_component(std::move(other.camera_component))
, proj_info_component(std::move(other.proj_info_component))
{
	/* Empty on purpose. */
}

entity& entity::operator = (entity&& other) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform = std::move(other.m_local_transform);
	m_global_transform = std::move(other.m_global_transform);
	m_local_bounding_box = std::move(other.m_local_bounding_box);
	m_global_bounding_box = std::move(other.m_global_bounding_box);
	m_children = std::move(other.m_children);
	m_parent = std::move(other.m_parent);
	m_octree = std::move(other.m_octree);
	mesh_component = std::move(other.mesh_component);
	material_component = std::move(other.material_component);
	light_component = std::move(other.light_component);
	rigid_body_component = std::move(other.rigid_body_component);
	logic_component = std::move(other.logic_component);
	AI_component = std::move(other.AI_component);
	camera_component = std::move(other.camera_component);
	proj_info_component = std::move(other.proj_info_component);

	return *this;
}

void entity::update_global_info(SQTstack& sqt_stack) BOOST_NOEXCEPT_OR_NOTHROW
{
	sqt_stack.push(m_local_transform);
	update_global_datamembers(sqt_stack);
	for (auto* child : m_children)
	{
		if (child) child->update_global_info(sqt_stack);
	}
	sqt_stack.pop();
}

void entity::update_global_info_start() BOOST_NOEXCEPT_OR_NOTHROW
{
	SQTstack sqt_stack(compute_global_transform());
	update_global_datamembers(sqt_stack);
	for (auto* child : m_children)
	{
		if (child) child->update_global_info(sqt_stack);
	}
}

void entity::update_global_datamembers(const SQTstack& sqt_stack) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_global_transform = sqt_stack.top();
	m_global_bounding_box.min_corner = m_global_transform.translation + m_local_bounding_box.min_corner;
	m_global_bounding_box.max_corner = m_global_transform.translation + m_local_bounding_box.max_corner;
	if (m_octree) m_octree->notify(this);
}

void entity::set_scale(const vec3f& scale) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.scale = scale;
	update_global_info_start();
}

void entity::multiply_scale(const vec3f& scale) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.scale *= scale;
	update_global_info_start();
}

void entity::set_translation(const vec3f& translation) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation = translation;
	update_global_info_start();
}

void entity::add_translation(const vec3f& translation) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation += translation;
	update_global_info_start();
}

void entity::set_rotation(const quatf& rotation) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.rotation = rotation;
	update_global_info_start();
}

void entity::post_multiply_rotation(const quatf& rotation) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.rotation *= rotation;
	update_global_info_start();
}

void entity::pre_multiply_rotation(const quatf& rotation) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.rotation = rotation * m_local_transform.rotation;
	update_global_info_start();
}

void entity::set_local_transform(const SQT& sqt) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform = sqt;
	update_global_info_start();
}

void entity::post_add_local_transform(const SQT& sqt) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform %= sqt;
	update_global_info_start();
}

void entity::pre_add_local_transform(const SQT& sqt) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform = sqt % m_local_transform;
	update_global_info_start();
}

void entity::move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation += amount * m_local_transform.rotation.forward_direction();
	update_global_info_start();
}

void entity::move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation -= amount * m_local_transform.rotation.forward_direction();
	update_global_info_start();
}

void entity::move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation += amount * m_local_transform.rotation.right_direction();
	update_global_info_start();
}

void entity::move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation -= amount * m_local_transform.rotation.right_direction();
	update_global_info_start();
}

void entity::move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation += amount * m_local_transform.rotation.up_direction();
	update_global_info_start();
}

void entity::move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_local_transform.translation -= amount * m_local_transform.rotation.up_direction();
	update_global_info_start();
}

SQT entity::compute_global_transform() const BOOST_NOEXCEPT_OR_NOTHROW
{
	SQT result(m_local_transform);
	const auto* current_entity = m_parent;
	while (current_entity)
	{
		if (current_entity == this)
		{
			#ifndef NDEBUG
			throw std::logic_error("There's a cycle in the entitities.");
			#else
			break;
			#endif
		}
		result.scale *= current_entity->m_local_transform.scale;
		result.rotation = current_entity->m_local_transform.rotation * result.rotation;
		result.translation += current_entity->m_local_transform.translation;
		current_entity = current_entity->m_parent;
	}
	return result;
}

void entity::add_child(entity& c)
{
	c.m_parent = this;
	m_children.push_front(&c);
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
	#ifndef NDEBUG
	if (m_octree)
	{
		auto result = m_octree->erase(this);
		if (result == 0)
		{
			throw std::logic_error("Entity was not present in the octree.");
		}
	}
	#else
	if (m_octree) m_octree->erase(this);
	#endif
}

} // namespace gintonic