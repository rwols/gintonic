/**
 * @file entity.hpp
 * @brief Defines the entity class.
 * @author Raoul Wols
 */

#ifndef gintonic_entity_hpp
#define gintonic_entity_hpp

#include "SQT.hpp"
#include "box3f.hpp"
#include "mat4f.hpp"
#include "allocator.hpp"
#include "ShadowBuffer.hpp"
#include "Object.hpp"
#include <list>
#include <boost/signals2.hpp>
#include <fbxsdk.h>

namespace gintonic {

class octree;     // Forward declaration.
class Mesh;       // Forward declaration.
class Material;   // Forward declaration.
class Light;      // Forward declaration.
class rigid_body; // Forward declaration.
class logic;      // Forward declaration.
class AI;         // Forward declaration.
class proj_info;  // Forward declaration.
class Camera;     // Forward declaration.
class SQTstack;   // Forward declaration.
class mat4fstack; // Forward declaration.
class controller; // Forward declaration.
class ShadowBuffer; // Forward declaration.

/**
 * @brief Represents an entity in the world.
 *
 * @details Entities should be created on the heap and managed via shared
 * pointers. Entities carry various components. The components are instantiated
 * with a unique pointer. Thus entities own components. Each component may have
 * a shared poiner to some large data somewhere in memory. Thus, a component
 * is like a vector in that the component itself should have a small memory
 * footprint, while the data that they point to might be very memory-intensive.
 */
class entity : public Object<entity, std::string>
{
public:

	/// The type of datastructure for the list of children.
	typedef std::list<std::shared_ptr<entity>> children_datastructure_type;

	/// The iterator type for the list of children.
	typedef children_datastructure_type::iterator iterator;

	/// The constant iterator type for the list of children.
	typedef children_datastructure_type::const_iterator const_iterator;

private:

	SQT mLocalTransform;
	mat4f mGlobalTransform;

	box3f m_local_bounding_box;
	box3f m_global_bounding_box;

	children_datastructure_type m_children;

	std::weak_ptr<entity> m_parent = std::shared_ptr<entity>(nullptr);

	octree*     m_octree               = nullptr;
	controller* m_controller_component = nullptr;
	Mesh*       m_mesh_component       = nullptr;
	Material*   m_material_component   = nullptr;
	Light*      m_light_component      = nullptr;
	rigid_body* m_rigid_body_component = nullptr;
	logic*      m_logic_component      = nullptr;
	AI*         m_AI_component         = nullptr;
	Camera*     m_camera_component     = nullptr;
	proj_info*  m_proj_info_component  = nullptr;

	friend class octree;
	friend class controller;
	friend class Mesh;
	friend class Material;
	friend class Light;
	friend class rigid_body;
	friend class logic;
	friend class AI;
	friend class Camera;
	friend class proj_info;

	void update_global_info(mat4fstack&) noexcept;
	void update_global_info_start() noexcept;
	mat4f compute_global_transform() noexcept;
	void update_global_datamembers(const mat4fstack&) noexcept;

public:

	entity(const FbxNode* pFbxNode);

	/**
	 * @brief Constructor.
	 *
	 * @param local_transform The local transform that the entity starts out
	 * with.
	 *
	 * @param local_bounding_box The local bounding box that the entity starts
	 * out with.
	 *
	 * @param octree_root If the entity should be part of an octree, you can
	 * pass the root of the octree here.
	 * 
	 * @param parent Wether the entity should have a parent entity.
	 */
	entity(
		const SQT& local_transform,
		const box3f& local_bounding_box,
		octree* octree_root = nullptr,
		std::shared_ptr<entity> parent = std::shared_ptr<entity>(nullptr));

	/**
	 * @brief Default constructor.
	 *
	 * @details The default constructor constructs an entity with a local
	 * transform centered at the origin and with the standard unit axes of
	 * three-dimensional space. It has a trivial local bounding box, so a
	 * point. It has no parent and is not part of an octree.
	 */
	entity() = default;

	bool castsShadow = false;

	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Light> light;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<proj_info> proj_info;
	std::unique_ptr<ShadowBuffer> shadowBuffer;

	// template <class ComponentType>
	// ComponentType* get() noexcept
	// {
	// 	return reinterpret_cast<ComponentType*>(m_components[ComponentType::this_type].get());
	// };

	// template <class ComponentType>
	// const ComponentType* get() const noexcept
	// {
	// 	return reinterpret_cast<ComponentType*>(m_components[ComponentType::this_type].get());
	// }

	/**
	 * @name Events
	 */

	//@{

	/**
	 * @brief Event that fires when this entity is about to die.
	 *
	 * @param e A reference to the entity that is about to die.
	 */
	boost::signals2::signal<void(entity&)> about_to_die;

	/**
	 * @brief Event that fires when the global transform has changed.
	 *
	 * @param e A reference to the entity whose global transform has changed.
	 */
	boost::signals2::signal<void(std::shared_ptr<entity>)> transform_changed;

	//@}

	/**
	 * @name Constructors and destructor
	 */

	//@{

	/**
	 * @brief Returns a shared pointer to a new entity.
	 * @return A shared pointer to a new entity.
	 */
	// static std::shared_ptr<entity> create();

	/**
	 * @brief Returns a shared pointer to a new entity.
	 * 
	 * @param local_transform The local transform that the entity starts out
	 * with.
	 *
	 * @param local_bounding_box The local bounding box that the entity starts
	 * out with.
	 *
	 * @param octree_root If the entity should be part of an octree, you can
	 * pass the root of the octree here.
	 * 
	 * @param parent Wether the entity should have a parent entity.
	 * 
	 * @return A shared pointer a new entity.
	 */
	// static std::shared_ptr<entity> create(
	// 	const SQT& local_transform, 
	// 	const box3f& local_bounding_box,
	// 	octree* octree_node = nullptr,
	// 	std::shared_ptr<entity> parent = std::shared_ptr<entity>(nullptr));

	/// You cannot copy entities. This could create cycles in the entity tree.
	entity(const entity&) = delete;

	/// You can move entities.
	entity(entity&&) noexcept;

	/// You cannot copy entities. This could create cycles in the entity tree.
	entity& operator = (const entity&) = delete;

	/// You can move entities.
	entity& operator = (entity&&) noexcept;

	/// Destructor.
	virtual ~entity();

	//@}

	/**
	 * @name Local SQT transform and bounding box management
	 */

	//@{

	/**
	 * @brief Set the scale of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * @param s The new scale.
	 */
	void set_scale(const vec3f& s) noexcept;

	/**
	 * @brief Multiply the current scale of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param s The scale to multiply the current local scale with.
	 */
	void multiply_scale(const vec3f& s) noexcept;

	/**
	 * @brief Set the translation of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param t The new translation.
	 */
	void set_translation(const vec3f& t) noexcept;

	/**
	 * @brief Set the translation's X-coordinate of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param x The new X-coordinate.
	 */
	void set_translation_x(const float x) noexcept;

	/**
	 * @brief Set the translation's Y-coordinate of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param y The new Y-coordinate.
	 */
	void set_translation_y(const float y) noexcept;

	/**
	 * @brief Set the translation's Z-coordinate of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param z The new Z-coordinate.
	 */
	void set_translation_z(const float z) noexcept;

	/**
	 * @brief Add a translation to the current SQT transform's translation.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param t The translation to add.
	 */
	void add_translation(const vec3f& t) noexcept;

	/**
	 * @brief Set the rotation quaternion of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param q The new rotation quaternion.
	 */
	void set_rotation(const quatf& q) noexcept;

	/**
	 * @brief Post-multiply the current rotation of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param q The rotation quaternion to post-multiply with.
	 */
	void post_multiply_rotation(const quatf& q) noexcept;

	/**
	 * @brief Pre-multiply the current rotation of the local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param q The rotation quaternion to pre-multiply with.
	 */
	void pre_multiply_rotation(const quatf& q) noexcept;

	/**
	 * @brief Set the local SQT transform of this entity.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param sqt The new SQT transform.
	 */
	void set_local_transform(const SQT& sqt) noexcept;

	/**
	 * @brief Post-add an SQT to the current local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param sqt The SQT transform to post-add.
	 */
	void post_add_local_transform(const SQT& sqt) noexcept;

	/**
	 * @brief Pre-add an SQT to the current local SQT transform.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param sqt The SQT transform to pre-add.
	 */
	void pre_add_local_transform(const SQT& sqt) noexcept;

	/**
	 * @brief Move the entity in the direction of the local forward direction.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void move_forward(const float amount) noexcept;

	/**
	 * @brief Move the entity in the direction of the local backward
	 * direction.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void move_backward(const float amount) noexcept;

	/**
	 * @brief Move the entity in the direction of the local right direction.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void move_right(const float amount) noexcept;

	/**
	 * @brief Move the entity in the direction of the local left direction.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void move_left(const float amount) noexcept;

	/**
	 * @brief Move the entity in the direction of the local up direction.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void move_up(const float amount) noexcept;

	/**
	 * @brief Move the entity in the direction of the local down direction.
	 *
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void move_down(const float amount) noexcept;

	/**
	 * @brief Get the local bounding box.
	 *
	 * @return A constant reference to the local bounding box.
	 */
	inline const box3f& local_bounding_box() const noexcept
	{
		return m_local_bounding_box;
	}

	/**
	 * @brief Set the local bounding box of this entity.
	 *
	 * @details The global bounding box is updated automatically. If the
	 * entity is part of a spatial tree, the spatial tree will be notified of
	 * the changed global bounding box, and will update its datastructure
	 * accordingly.
	 *
	 * @param b The new local bounding box.
	 */
	void set_local_bounding_box(const box3f& b);

	/**
	 * @brief Set the local bounding box of this entity.
	 *
	 * @details The global bounding box is updated automatically. If the
	 * entity is part of a spatial tree, the spatial tree will be notified of
	 * the changed global bounding box, and will update its datastructure
	 * accordingly.
	 *
	 * @param min_corner The new minimum corner of the local bounding box.
	 * @param max_corner The new maximum corner of the local bounding box.
	 */
	void set_local_bounding_box(
		const vec2f& min_corner,
		const vec2f& max_corner);

	/**
	 * @brief Get the local SQT transform.
	 *
	 * @return A constant reference to the local SQT transform.
	 */
	inline const SQT& local_transform() const noexcept
	{
		return mLocalTransform;
	}

	//@}

	/**
	 * @name Children entities and parent entity management
	 */

	//@{

	/**
	 * @brief Add a child entity to this entity.
	 *
	 * @details If the child entity already had a parent, then this parent
	 * is first removed from the entity. In the old parent entity, the child
	 * entity is searched for and removed from the list.
	 *
	 * @param child The child entity.
	 */
	void add_child(std::shared_ptr<entity> child);

	/**
	 * @brief Remove a child entity of this entity.
	 *
	 * @details If the child entity is not present, then nothing will happen.
	 * The parent pointer of the child entity will be set to null.
	 *
	 * @param child The child entity to remove.
	 */
	void remove_child(std::shared_ptr<entity> child);

	/**
	 * @brief Remove a child entity of this entity.
	 *
	 * @param iter An iterator pointing to a child.
	 */
	void remove_child(iterator child);

	/**
	 * @brief Set the parent of this entity.
	 *
	 * @details If the child already has a parent, then this parent is first
	 * removed. In the parent's children list, this entity is searched for
	 * and removed.
	 *
	 * @param parent The parent entity.
	 */
	void set_parent(std::shared_ptr<entity> parent);

	/**
	 * @brief Set this entity to have no parent.
	 *
	 * @details The parent pointer will be set to null. If this entity has a
	 * parent, then this entity is serached for in the children's list of the
	 * parent entity and removed.
	 */
	void unset_parent();


	/**
	 * @brief Get a pointer to the parent of this entity.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 * In order to get an actual pointer, use the following construct:
	 * if (auto ptr = e->parent().lock())
	 * {
	 *   // do stuff with ptr
	 * }
	 *
	 * @return A weak pointer to the parent of this entity.
	 */
	inline std::weak_ptr<entity> parent() noexcept
	{
		return m_parent;
	}

	/**
	 * @brief Get a constant pointer to the parent of this entity.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 *
	 * @return A constant pointer to the parent of this entity.
	 */
	inline const std::weak_ptr<const entity> parent() const noexcept
	{
		return m_parent;
	}

	/**
	 * @brief Get an iterator to the beginning of children list.
	 *
	 * @return An iterator to the beginning of the children list.
	 */
	inline iterator begin() noexcept
	{
		return m_children.begin();
	}

	/**
	 * @brief Get an iterator to one-past-the-end of children list.
	 *
	 * @return An iterator to one-past-the-end of the children list.
	 */
	inline iterator end() noexcept
	{
		return m_children.end();
	}

	/**
	 * @brief Get a const iterator to the beginning of children list.
	 *
	 * @return A const iterator to the beginning of the children list.
	 */
	inline const_iterator begin() const noexcept
	{
		return m_children.begin();
	}

	/**
	 * @brief Get a const iterator to one-past-the-end of children list.
	 *
	 * @return a const iterator to one-past-the-end of the children list.
	 */
	inline const_iterator end() const noexcept
	{
		return m_children.end();
	}

	/**
	 * @brief Get a const iterator to the beginning of children list.
	 *
	 * @return A const iterator to the beginning of the children list.
	 */
	inline const_iterator cbegin() const noexcept
	{
		return m_children.cbegin();
	}

	/**
	 * @brief Get a const iterator to one-past-the-end of children list.
	 *
	 * @return a const iterator to one-past-the-end of the children list.
	 */
	inline const_iterator cend() const noexcept
	{
		return m_children.cend();
	}

	//@}

	/**
	 * @name Global information
	 */

	//@{

	/**
	 * @brief Get the `VIEW` matrix of this entity.
	 *
	 * @param m The matrix to set.
	 */
	void get_view_matrix(mat4f& m) const noexcept;

	/**
	 * @brief Get the global transformation matrix, i.e. from `MODEL` space
	 * to `WORLD` space.
	 *
	 * @return A constant reference to the global transformation matrix.
	 */
	inline const mat4f& global_transform() const noexcept
	{
		return mGlobalTransform;
	}

	/**
	 * @brief Get the global bounding box.
	 *
	 * @return A constant reference to the global bounding box.
	 */
	inline const box3f& global_bounding_box() const noexcept
	{
		return m_global_bounding_box;
	}

	//@}

	// /**
	//  * @name Components
	//  */

	// //@{

	// /**
	//  * @brief Get a pointer to the controller component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the controller component of this entity.
	//  */
	// inline controller* controller_component() noexcept
	// {
	// 	return m_controller_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the controller component of this
	//  * entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the controller component of this entity.
	//  */
	// inline const controller* controller_component() const
	// 	noexcept
	// {
	// 	return m_controller_component;
	// }

	// /**
	//  * @brief Get a pointer to the mesh component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the mesh component of this entity.
	//  */
	// inline mesh* mesh_component() noexcept
	// {
	// 	return m_mesh_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the mesh component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the mesh component of this entity.
	//  */
	// inline const mesh* mesh_component() const noexcept
	// {
	// 	return m_mesh_component;
	// }

	// /**
	//  * @brief Get a pointer to the material component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the material component of this entity.
	//  */
	// inline material* material_component() noexcept
	// {
	// 	return m_material_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the material component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the material component of this entity.
	//  */
	// inline const material* material_component() const
	// 	noexcept
	// {
	// 	return m_material_component;
	// }

	// /**
	//  * @brief Get a pointer to the light component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the light component of this entity.
	//  */
	// inline light* light_component() noexcept
	// {
	// 	return m_light_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the light component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the light component of this entity.
	//  */
	// inline const light* light_component() const noexcept
	// {
	// 	return m_light_component;
	// }

	// /**
	//  * @brief Get a pointer to the rigid_body component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the rigid_body component of this entity.
	//  */
	// inline rigid_body* rigid_body_component() noexcept
	// {
	// 	return m_rigid_body_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the rigid_body component of this
	//  * entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the rigid_body component of this entity.
	//  */
	// inline const rigid_body* rigid_body_component() const
	// 	noexcept
	// {
	// 	return m_rigid_body_component;
	// }

	// /**
	//  * @brief Get a pointer to the AI component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the AI component of this entity.
	//  */
	// inline AI* AI_component() noexcept
	// {
	// 	return m_AI_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the AI component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the AI component of this entity.
	//  */
	// inline const AI* AI_component() const noexcept
	// {
	// 	return m_AI_component;
	// }

	// /**
	//  * @brief Get a pointer to the camera component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the camera component of this entity.
	//  */
	// inline camera* camera_component() noexcept
	// {
	// 	return m_camera_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the camera component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the camera component of this entity.
	//  */
	// inline const camera* camera_component() const noexcept
	// {
	// 	return m_camera_component;
	// }

	// /**
	//  * @brief Get a pointer to the proj_info component of this entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A pointer to the proj_info component of this entity.
	//  */
	// inline proj_info*  proj_info_component() noexcept
	// {
	// 	return m_proj_info_component;
	// }

	// /**
	//  * @brief Get a constant pointer to the proj_info component of this
	//  * entity.
	//  *
	//  * @details The pointer can be null, so be sure to check for that.
	//  *
	//  * @return A constant pointer to the proj_info component of this entity.
	//  */
	// inline const proj_info*  proj_info_component() const
	// 	noexcept
	// {
	// 	return m_proj_info_component;
	// }

	//@}

	/**
	 * @name Octree management
	 */

	//@{

	/**
	 * @brief Get a pointer to the octree node that this entity belongs to.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 *
	 * @return A pointer to the octree node that this entity belongs to.
	 */
	inline octree* octree_node() noexcept
	{
		return m_octree;
	}

	/**
	 * @brief Get a const pointer to the octree node that this entity belongs
	 * to.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 *
	 * @return A const pointer to the octree node that this entity belongs to.
	 */
	inline const octree* octree_node() const noexcept
	{
		return m_octree;
	}

	//@}

	/**
	 * @name Comparison operators
	 */

	//@{

	/**
	 * @brief Two entities compare equal if they live at the same memory
	 * address.
	 *
	 * @param other Another entity.
	 *
	 * @return True if they live at the same memory address, false otherwise.
	 */
	inline bool operator == (const entity& other) const
		noexcept
	{
		return this == &other;
	}

	/**
	 * @brief Two entities compare unequal if they don't live at the same
	 * memory address.
	 *
	 * @param other Another entity.
	 *
	 * @return True if they live don't at the same memory address,
	 * false otherwise.
	 */
	inline bool operator != (const entity& other) const
		noexcept
	{
		return !operator==(other);
	}

	/**
	 * @brief Compares less via the memory addresses.
	 *
	 * @param other Another entity.
	 *
	 * @return True if this entity's memory address is less than the other
	 * entity's memory address. False otherwise.
	 */
	inline bool operator < (const entity& other) const
		noexcept
	{
		return this < &other;
	}

	/**
	 * @brief Compares greater-than-or-equal via the memory addresses.
	 *
	 * @param other Another entity.
	 *
	 * @return True if this entity's memory address is greater than or equal
	 * to the other entity's memory address. False otherwise.
	 */
	inline bool operator >= (const entity& other) const
		noexcept
	{
		return !operator<(other);
	}

	/**
	 * @brief Compares greater-than via the memory addresses.
	 *
	 * @param other Another entity.
	 *
	 * @return True if this entity's memory address is greater than the other
	 * entity's memory address. False otherwise.
	 */
	inline bool operator > (const entity& other) const
		noexcept
	{
		return this > &other;
	}

	/**
	 * @brief Compares less-than-or-equal-to via the memory addresses.
	 *
	 * @param other Another entity.
	 *
	 * @return True if this entity's memory address is less than or equal to
	 * the other entity's memory address. False otherwise.
	 */
	inline bool operator <= (const entity& other) const
		noexcept
	{
		return !operator>(other);
	}

	//@}

	//! @cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//! @endcond
};

} // namespace gintonic

#endif
