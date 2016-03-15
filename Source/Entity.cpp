#include "Entity.hpp"

#include "Foundation/octree.hpp"

#include "Math/vec4f.hpp"
#include "Math/SQTstack.hpp"
#include "Math/mat4fstack.hpp"

#include <fbxsdk.h>

namespace gintonic {

Entity::Entity(const FbxNode* pFbxNode)
: Object<Entity, std::string>(pFbxNode)
{
	// auto lGlobalName = boost::filesystem::path(pFbxNode->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
	// setName(std::move(lGlobalName), pFbxNode->GetName());

	auto lValue = pFbxNode->LclScaling.Get();
	mLocalTransform.scale.x = static_cast<float>(lValue[0]);
	mLocalTransform.scale.y = static_cast<float>(lValue[1]);
	mLocalTransform.scale.z = static_cast<float>(lValue[2]);

	lValue = pFbxNode->LclRotation.Get();
	mLocalTransform.rotation = quatf::yaw_pitch_roll(
		static_cast<float>(lValue[0]), 
		static_cast<float>(lValue[1]), 
		static_cast<float>(lValue[2]));

	lValue = pFbxNode->LclTranslation.Get();
	mLocalTransform.translation.x = static_cast<float>(lValue[0]);
	mLocalTransform.translation.y = static_cast<float>(lValue[1]);
	mLocalTransform.translation.z = static_cast<float>(lValue[2]);

	mGlobalTransform = mLocalTransform;
}

// Entity::Entity(
// 	const SQT& local_transform,
// 	const box3f& local_bounding_box,
// 	octree* octree_root,
// 	std::shared_ptr<Entity> parent)
// : mLocalTransform(local_transform)
// , m_local_bounding_box(local_bounding_box)
// , m_parent(parent)
// {
// 	update_global_info_start();
// 	if (auto ptr = m_parent.lock())
// 	{
// 		ptr->m_children.push_front(shared_from_this());
// 	}
// 	if (octree_root) octree_root->insert(this);
// }

Entity::Entity(Entity&& other) noexcept
: mLocalTransform(std::move(other.mLocalTransform))
, mGlobalTransform(std::move(other.mGlobalTransform))
// , m_local_bounding_box(std::move(other.m_local_bounding_box))
// , m_global_bounding_box(std::move(other.m_global_bounding_box))
// , m_children(std::move(other.m_children))
// , m_parent(std::move(other.m_parent))
// , m_octree(std::move(other.m_octree))
// , m_mesh_component(std::move(other.m_mesh_component))
// , m_material_component(std::move(other.m_material_component))
// , m_light_component(std::move(other.m_light_component))
// , m_rigid_body_component(std::move(other.m_rigid_body_component))
// , m_logic_component(std::move(other.m_logic_component))
// , m_AI_component(std::move(other.m_AI_component))
// , m_camera_component(std::move(other.m_camera_component))
// , m_proj_info_component(std::move(other.m_proj_info_component))
{
	if (auto ptr = m_parent.lock())
	{
		ptr->m_children.push_front(shared_from_this());
	}
}

Entity& Entity::operator = (Entity&& other) noexcept
{
	mLocalTransform = std::move(other.mLocalTransform);
	mGlobalTransform = std::move(other.mGlobalTransform);
	// m_local_bounding_box = std::move(other.m_local_bounding_box);
	// m_global_bounding_box = std::move(other.m_global_bounding_box);
	// m_children = std::move(other.m_children);
	// m_parent = std::move(other.m_parent);
	// m_octree = std::move(other.m_octree);
	// m_mesh_component = std::move(other.m_mesh_component);
	// m_material_component = std::move(other.m_material_component);
	// m_light_component = std::move(other.m_light_component);
	// m_rigid_body_component = std::move(other.m_rigid_body_component);
	// m_logic_component = std::move(other.m_logic_component);
	// m_AI_component = std::move(other.m_AI_component);
	// m_camera_component = std::move(other.m_camera_component);
	// m_proj_info_component = std::move(other.m_proj_info_component);

	return *this;
}

void Entity::update_global_info(mat4fstack& matrix_stack) noexcept
{
	matrix_stack.push(mLocalTransform);
	update_global_datamembers(matrix_stack);
	for (auto child : m_children)
	{
		if (child) child->update_global_info(matrix_stack);
	}
	matrix_stack.pop();
}

void Entity::update_global_info_start() noexcept
{
	mat4fstack matrix_stack(compute_global_transform());
	update_global_datamembers(matrix_stack);
	for (auto child : m_children)
	{
		if (child) child->update_global_info(matrix_stack);
	}
}

void Entity::update_global_datamembers(const mat4fstack& matrix_stack) noexcept
{
	mGlobalTransform = matrix_stack.top();
	const vec3f t(mGlobalTransform.data[3]);
	m_global_bounding_box.min_corner = t + m_local_bounding_box.min_corner;
	m_global_bounding_box.max_corner = t + m_local_bounding_box.max_corner;
	if (m_octree) m_octree->notify(this);
}

void Entity::set_scale(const vec3f& scale) noexcept
{
	mLocalTransform.scale = scale;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::multiply_scale(const vec3f& scale) noexcept
{
	mLocalTransform.scale *= scale;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::set_translation(const vec3f& translation) noexcept
{
	mLocalTransform.translation = translation;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::set_translation_x(const float x) noexcept
{
	mLocalTransform.translation.x = x;
	update_global_info_start();
	transform_changed(shared_from_this());
}
void Entity::set_translation_y(const float y) noexcept
{
	mLocalTransform.translation.y = y;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::set_translation_z(const float z) noexcept
{
	mLocalTransform.translation.z = z;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::add_translation(const vec3f& translation) noexcept
{
	mLocalTransform.translation += translation;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::set_rotation(const quatf& rotation) noexcept
{
	mLocalTransform.rotation = rotation;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::post_multiply_rotation(const quatf& rotation) noexcept
{
	mLocalTransform.rotation *= rotation;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::pre_multiply_rotation(const quatf& rotation) noexcept
{
	mLocalTransform.rotation = rotation * mLocalTransform.rotation;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::set_local_transform(const SQT& sqt) noexcept
{
	mLocalTransform = sqt;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::post_add_local_transform(const SQT& sqt) noexcept
{
	mLocalTransform %= sqt;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::pre_add_local_transform(const SQT& sqt) noexcept
{
	mLocalTransform = sqt % mLocalTransform;
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::move_forward(const float amount) noexcept
{
	mLocalTransform.translation += amount * mLocalTransform.rotation.forward_direction();
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::move_backward(const float amount) noexcept
{
	mLocalTransform.translation -= amount * mLocalTransform.rotation.forward_direction();
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::move_right(const float amount) noexcept
{
	mLocalTransform.translation += amount * mLocalTransform.rotation.right_direction();
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::move_left(const float amount) noexcept
{
	mLocalTransform.translation -= amount * mLocalTransform.rotation.right_direction();
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::move_up(const float amount) noexcept
{
	mLocalTransform.translation += amount * mLocalTransform.rotation.up_direction();
	update_global_info_start();
	transform_changed(shared_from_this());
}

void Entity::move_down(const float amount) noexcept
{
	mLocalTransform.translation -= amount * mLocalTransform.rotation.up_direction();
	update_global_info_start();
	transform_changed(shared_from_this());
}

mat4f Entity::compute_global_transform() noexcept
{
	if (auto ptr = m_parent.lock())
	{
		return ptr->global_transform() * mat4f(mLocalTransform);
	}
	else
	{
		return mat4f(mLocalTransform);
	}
	// mat4f global_matrix(mLocalTransform);
	// const auto* current_Entity = m_parent;
	// while (current_Entity)
	// {
	// 	if (current_Entity == this) break; // Cycle in the Entity graph.
	// 	global_matrix = mat4f(current_Entity->mLocalTransform) * global_matrix;
	// 	current_Entity = current_Entity->m_parent;
	// }
	// return global_matrix;
}

void Entity::add_child(std::shared_ptr<Entity> child)
{
	child->m_parent = shared_from_this();
	m_children.push_front(child);
	child->update_global_info_start();
	child->transform_changed(child);
}

void Entity::remove_child(std::shared_ptr<Entity> child)
{
	#ifndef NDEBUG
	bool child_was_removed = false;
	#endif

	for (auto i = begin(); i != end(); ++i)
	{
		if (i->get() == child.get())
		{
			m_children.erase(i);

			#ifndef NDEBUG
			child_was_removed = true;
			#endif

			break;
		}
	}

	#ifndef NDEBUG
	if (child_was_removed) child->m_parent = std::shared_ptr<Entity>(nullptr);
	else throw std::logic_error("Entity was not a child.");
	#else
	c.m_parent = std::shared_ptr<Entity>(nullptr);
	#endif
}

void Entity::set_parent(std::shared_ptr<Entity> parent)
{
	if (auto ptr = m_parent.lock())
	{
		ptr->remove_child(shared_from_this());
	}
	parent->add_child(shared_from_this()); // also sets m_parent
}

void Entity::unset_parent()
{
	// Also sets m_parent to nullptr
	if (auto ptr = m_parent.lock()) ptr->remove_child(shared_from_this());
}

void Entity::get_view_matrix(mat4f& m) const noexcept
{
	const auto f = global_transform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
	const auto u = global_transform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
	const auto r = global_transform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
	const auto eye = global_transform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

	m.m00 = r.x;
	m.m10 = u.x;
	m.m20 = -f.x;
	m.m30 = 0.0f;

	m.m01 = r.y;
	m.m11 = u.y;
	m.m21 = -f.y;
	m.m31 = 0.0f;

	m.m02 = r.z;
	m.m12 = u.z;
	m.m22 = -f.z;
	m.m32 = 0.0f;

	m.m03 = -dot(r, eye);
	m.m13 = -dot(u, eye);
	m.m23 =  dot(f, eye);
	m.m33 = 1.0f;
}

Entity::~Entity()
{
	about_to_die(*this);
	if (m_octree) m_octree->erase(this);
	unset_parent();
}

} // namespace gintonic
