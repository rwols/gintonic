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
	octree&      octree_root,
	entity*      parent,
	mesh*        mesh_component,
	material*    material_component,
	light*       light_component,
	rigid_body*  rigid_body_component,
	logic*       logic_component,
	AI*          AI_component)
: m_local_transform(local_transform)
, m_local_bounding_box(local_bounding_box)
, m_parent(parent)
, mesh_component(mesh_component)
, material_component(material_component)
, light_component(light_component)
, rigid_body_component(rigid_body_component)
, logic_component(logic_component)
, AI_component(AI_component)
{
	update_global_info_start();
	octree_root.insert(this);
}

entity::entity(const entity& other)
: m_local_transform(other.m_local_transform)
, m_global_transform(other.m_global_transform)
, m_local_bounding_box(other.m_local_bounding_box)
, m_global_bounding_box(other.m_global_bounding_box)
, m_parent(other.m_parent)
, m_octree(other.m_octree)
, mesh_component(other.mesh_component)
, material_component(other.material_component)
, light_component(other.light_component)
, rigid_body_component(other.rigid_body_component)
, logic_component(other.logic_component)
, AI_component(other.AI_component)
{
	// First, if the object to copy is part of an octree,
	// then we have to update that octree too.
	if (m_octree) m_octree->insert(this);

	// Now we have to copy all the children recursively.
	for (auto* child : other.m_children)
	{
		if (child)
		{
			// Note that this recurses.
			auto* child_copy = new entity(*child);
			m_children.push_back(child_copy);
		}
		else
		{
			m_children.push_back(nullptr);
		}
	}
}

entity::entity(entity&& other)
: m_local_transform(std::move(other.m_local_transform))
, m_global_transform(std::move(other.m_global_transform))
, m_local_bounding_box(std::move(other.m_local_bounding_box))
, m_global_bounding_box(std::move(other.m_global_bounding_box))
, m_parent(other.m_parent)
, m_octree(other.m_octree)
, mesh_component(other.mesh_component)
, material_component(other.material_component)
, light_component(other.light_component)
, rigid_body_component(other.rigid_body_component)
, logic_component(other.logic_component)
, AI_component(other.AI_component)
{
	other.m_parent = nullptr;
	other.m_octree = nullptr;
	other.mesh_component = nullptr;
	other.material_component = nullptr;
	other.light_component = nullptr;
	other.rigid_body_component = nullptr;
	other.logic_component = nullptr;
	other.AI_component = nullptr;

	// Now we have to update the octree.
	if (m_octree)
	{
		for (auto i = m_octree->m_entities.begin(); 
			i != m_octree->m_entities.end(); ++i)
		{
			if (*i == &other)
			{
				*i = this;
				break;
			}
		}
	}

	// Now we have to move all the children.
	// Note that recursion is NOT necessary.
	for (auto* child : other.m_children)
	{
		if (child)
		{
			// This does NOT recurse. But that's okay.
			m_children.push_back(child);
			child->m_parent = this;
		}
		else
		{
			m_children.push_back(nullptr);
		}
	}
	// We clear the children container of the moved entity.
	// When the destructor of the moved entity is called,
	// it will do nothing.
	other.m_children.clear();
}
entity& entity::operator = (const entity& other)
{
	m_local_transform = other.m_local_transform;
	m_global_transform = other.m_global_transform;
	m_local_bounding_box = other.m_local_bounding_box;
	m_global_bounding_box = other.m_global_bounding_box;
	m_parent = other.m_parent;
	m_octree = other.m_octree;
	mesh_component = other.mesh_component;
	material_component = other.material_component;
	light_component = other.light_component;
	rigid_body_component = other.rigid_body_component;
	logic_component = other.logic_component;
	AI_component = other.AI_component;

	// Explicitly call the destructor.
	this->~entity();

	// Now we have to copy all the children recursively.
	for (auto* child : other.m_children)
	{
		if (child)
		{
			// Note that this recurses.
			auto* child_copy = new entity(*child);
			m_children.push_back(child_copy);
		}
		else
		{
			m_children.push_back(nullptr);
		}
	}

	return *this;
}

entity& entity::operator = (entity&& other)
{
	m_local_transform = std::move(other.m_local_transform);
	m_global_transform = std::move(other.m_global_transform);
	m_local_bounding_box = std::move(other.m_local_bounding_box);
	m_global_bounding_box = std::move(other.m_global_bounding_box);
	m_parent = other.m_parent;
	m_octree = other.m_octree;
	mesh_component = other.mesh_component;
	material_component = other.material_component;
	light_component = other.light_component;
	rigid_body_component = other.rigid_body_component;
	logic_component = other.logic_component;
	AI_component = other.AI_component;

	// Explicitly call the destructor.
	this->~entity();

	// Clear the children container. We can safely do this
	// because the destructor has been called explicitly.
	m_children.clear();

	// Now we have to move all the children.
	// Note that recursion is NOT necessary.
	for (auto* child : other.m_children)
	{
		if (child)
		{
			// This does NOT recurse. But that's okay.
			m_children.push_back(child);
			child->m_parent = this;
		}
		else
		{
			m_children.push_back(nullptr);
		}
	}
	// We clear the children container of the moved entity.
	// When the destructor of the moved entity is called,
	// it will do nothing.
	other.m_children.clear();

	return *this;
}

void entity::update_global_info(SQTstack& sqt_stack) BOOST_NOEXCEPT_OR_NOTHROW
{
	sqt_stack.push(m_local_transform);
	update_global_transform_and_bounding_box(sqt_stack);
	for (auto* child : m_children)
	{
		if (child) child->update_global_info(sqt_stack);
	}
	sqt_stack.pop();
}

void entity::update_global_info_start() BOOST_NOEXCEPT_OR_NOTHROW
{
	SQTstack sqt_stack(compute_global_transform());
	update_global_transform_and_bounding_box(sqt_stack);
	for (auto* child : m_children)
	{
		if (child) child->update_global_info(sqt_stack);
	}
}

void entity::update_global_transform_and_bounding_box(const SQTstack& sqt_stack) BOOST_NOEXCEPT_OR_NOTHROW
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

SQT entity::compute_global_transform() const BOOST_NOEXCEPT_OR_NOTHROW
{
	SQT result(vec3f(1.0f, 1.0f, 1.0f), quatf(1.0f, 0.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 0.0f));
	const auto* current_entity = this;
	while (current_entity)
	{
		result.scale *= current_entity->m_local_transform.scale;
		result.rotation = current_entity->m_local_transform.rotation * result.rotation;
		result.translation += current_entity->m_local_transform.translation;
		current_entity = current_entity->m_parent;
	}
	return result;
}

void entity::add_child(entity* c)
{
	c->m_parent = this;
	m_children.push_front(c);
}

void entity::remove_child(entity* c)
{
	#ifndef NDEBUG
	bool child_was_removed = false;
	#endif

	for (auto i = cbegin(); i != cend(); ++i)
	{
		if (*i == c)
		{
			m_children.erase(i);

			#ifndef NDEBUG
			child_was_removed = true;
			#endif

			break;
		}
	}

	#ifndef NDEBUG
	if (child_was_removed) c->m_parent = nullptr;
	else throw std::logic_error("Entity was not a child.");
	#else
	c->m_parent = nullptr;
	#endif
}

void entity::set_parent(entity* p)
{
	if (m_parent) m_parent->remove_child(this);
	if (p) p->add_child(this);
	else m_parent = nullptr;
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
	for (const auto* child : m_children) delete child;
}

void entity::draw_geometry() const
{
	SQTstack sqt_stack;
	draw_geometry(sqt_stack);
}

void entity::draw_geometry(SQTstack& sqt_stack) const
{
	sqt_stack.push(m_local_transform);

	if (mesh_component)
	{
		renderer::set_model_matrix(sqt_stack.top());
		if (material_component)
		{
			material_component->bind();
			mesh_component->draw();
		}
		else
		{
			// Make a "null" material.
			material mat(
				vec4f(0.6f, 0.6f, 0.6f, 0.9f),
				vec4f(1.0f, 1.0f, 1.5f, 1.0f));
			mat.bind();
			mesh_component->draw();
		}
	}

	for (const auto* child : m_children)
	{
		if (child) child->draw_geometry(sqt_stack);
	}

	sqt_stack.pop();
}

void entity::draw_lights() const
{
	SQTstack sqt_stack;
	draw_lights(sqt_stack);
}

void entity::draw_lights(SQTstack& sqt_stack) const
{
	sqt_stack.push(m_local_transform);

	if (light_component)
	{
		light_component->shine(sqt_stack.top());
	}

	for (const auto* child : m_children)
	{
		if (child) child->draw_lights(sqt_stack);
	}

	sqt_stack.pop();
}

// entity* entity::create()
// {
// 	s_entitities_lock.obtain();
// 	s_entitities.push_front(entity());
// 	s_entitities.front().m_handle = s_entitities.begin();
// 	s_entitities_lock.release();
// }

// void entity::clear_all()
// {
// 	s_entitities_lock.obtain();
// 	s_entitities.clear();
// 	s_entitities_lock.release();
// }

// void entity::destroy()
// {
// 	s_entitities_lock.obtain();
// 	s_entitities.erase(m_handle);
// 	s_entitities_lock.release();
// }

} // namespace gintonic