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
#include <list>
#include <boost/signals2.hpp>

namespace gintonic {

class octree;     // Forward declaration.
class mesh;       // Forward declaration.
class material;   // Forward declaration.
class light;      // Forward declaration.
class rigid_body; // Forward declaration.
class logic;      // Forward declaration.
class AI;         // Forward declaration.
class proj_info;  // Forward declaration.
class camera;     // Forward declaration.
class SQTstack;   // Forward declaration.
class mat4fstack; // Forward declaration.
class controller; // Forward declaration.

/**
 * @brief Represents an entity in the world.
 */
class entity
{
public:

	/// The type of datastructure for the list of children.
	typedef std::list<entity*> children_datastructure_type;

	/// The iterator type for the list of children.
	typedef children_datastructure_type::iterator iterator;

	/// The constant iterator type for the list of children.
	typedef children_datastructure_type::const_iterator const_iterator;

private:

	SQT m_local_transform;
	mat4f m_global_transform;

	box3f m_local_bounding_box;
	box3f m_global_bounding_box;

	children_datastructure_type m_children;

	entity*     m_parent               = nullptr;
	octree*     m_octree               = nullptr;
	controller* m_controller_component = nullptr;
	mesh*       m_mesh_component       = nullptr;
	material*   m_material_component   = nullptr;
	light*      m_light_component      = nullptr;
	rigid_body* m_rigid_body_component = nullptr;
	logic*      m_logic_component      = nullptr;
	AI*         m_AI_component         = nullptr;
	camera*     m_camera_component     = nullptr;
	proj_info*  m_proj_info_component  = nullptr;

	friend class octree;
	friend class controller;
	friend class mesh;
	friend class material;
	friend class light;
	friend class rigid_body;
	friend class logic;
	friend class AI;
	friend class camera;
	friend class proj_info;

	void update_global_info(mat4fstack&) BOOST_NOEXCEPT_OR_NOTHROW;
	void update_global_info_start() BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f compute_global_transform() BOOST_NOEXCEPT_OR_NOTHROW;
	void update_global_datamembers(const mat4fstack&) BOOST_NOEXCEPT_OR_NOTHROW;

public:

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
	boost::signals2::signal<void(entity&)> transform_changed;

	//@}

	/**
	 * @name Constructors and destructor
	 */

	//@{
	
	/**
	 * @brief Default constructor.
	 * 
	 * @details The default constructor constructs an entity with a local
	 * transform centered at the origin and with the standard unit axes of
	 * three-dimensional space. It has a trivial local bounding box, so a
	 * point. It has no parent and is not part of an octree.
	 */
	entity() = default;
	
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
		const SQT&   local_transform,
		const box3f& local_bounding_box,
		octree*      octree_root         = nullptr,
		entity*      parent              = nullptr);

	/// You cannot copy entities. This could create cycles in the entity tree.
	entity(const entity&) = delete;

	/// You can move entities.
	entity(entity&&) BOOST_NOEXCEPT_OR_NOTHROW;

	/// You cannot copy entities. This could create cycles in the entity tree.
	entity& operator = (const entity&) = delete;

	/// You can move entities.
	entity& operator = (entity&&) BOOST_NOEXCEPT_OR_NOTHROW;
	
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
	void set_scale(const vec3f& s) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Multiply the current scale of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param s The scale to multiply the current local scale with.
	 */
	void multiply_scale(const vec3f& s) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the translation of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param t The new translation.
	 */
	void set_translation(const vec3f& t) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the translation's X-coordinate of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param x The new X-coordinate.
	 */
	void set_translation_x(const float x) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the translation's Y-coordinate of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param y The new Y-coordinate.
	 */
	void set_translation_y(const float y) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the translation's Z-coordinate of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param z The new Z-coordinate.
	 */
	void set_translation_z(const float z) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Add a translation to the current SQT transform's translation.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param t The translation to add.
	 */
	void add_translation(const vec3f& t) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the rotation quaternion of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param q The new rotation quaternion.
	 */
	void set_rotation(const quatf& q) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Post-multiply the current rotation of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param q The rotation quaternion to post-multiply with.
	 */
	void post_multiply_rotation(const quatf& q) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Pre-multiply the current rotation of the local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param q The rotation quaternion to pre-multiply with.
	 */
	void pre_multiply_rotation(const quatf& q) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the local SQT transform of this entity.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param sqt The new SQT transform.
	 */
	void set_local_transform(const SQT& sqt) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Post-add an SQT to the current local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param sqt The SQT transform to post-add.
	 */
	void post_add_local_transform(const SQT& sqt) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Pre-add an SQT to the current local SQT transform.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param sqt The SQT transform to pre-add.
	 */
	void pre_add_local_transform(const SQT& sqt) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Move the entity in the direction of the local forward direction.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param amount The amount of translation.
	 */
	void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Move the entity in the direction of the local backward
	 * direction.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param amount The amount of translation.
	 */
	void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Move the entity in the direction of the local right direction.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param amount The amount of translation.
	 */
	void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Move the entity in the direction of the local left direction.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param amount The amount of translation.
	 */
	void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Move the entity in the direction of the local up direction.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param amount The amount of translation.
	 */
	void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Move the entity in the direction of the local down direction.
	 * 
	 * @details If the entity has children, this will update the global
	 * transform of all of its children.
	 * 
	 * @param amount The amount of translation.
	 */
	void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Get the local bounding box.
	 * 
	 * @return A constant reference to the local bounding box.
	 */
	inline const box3f& local_bounding_box() const BOOST_NOEXCEPT_OR_NOTHROW
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
	inline const SQT& local_transform() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_local_transform;
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
	 * @param e The child entity.
	 */
	void add_child(entity& e);

	/**
	 * @brief Remove a child entity of this entity.
	 * 
	 * @details If the child entity is not present, then nothing will happen.
	 * The parent pointer of the child entity will be set to null.
	 * 
	 * @param e The child entity to remove.
	 */
	void remove_child(entity& e);

	/**
	 * @brief Set the parent of this entity.
	 * 
	 * @details If the child already has a parent, then this parent is first
	 * removed. In the parent's children list, this entity is searched for
	 * and removed.
	 * 
	 * @param e The parent entity.
	 */
	void set_parent(entity& e);

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
	 * 
	 * @return A pointer to the parent of this entity.
	 */
	inline entity* parent() BOOST_NOEXCEPT_OR_NOTHROW
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
	inline const entity* parent() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_parent;
	}

	/**
	 * @brief Get an iterator to the beginning of children list.
	 * 
	 * @return An iterator to the beginning of the children list.
	 */
	inline iterator begin() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_children.begin();
	}

	/**
	 * @brief Get an iterator to one-past-the-end of children list.
	 * 
	 * @return An iterator to one-past-the-end of the children list.
	 */
	inline iterator end() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_children.end();
	}

	/**
	 * @brief Get a const iterator to the beginning of children list.
	 * 
	 * @return A const iterator to the beginning of the children list.
	 */
	inline const_iterator begin() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_children.begin();
	}

	/**
	 * @brief Get a const iterator to one-past-the-end of children list.
	 * 
	 * @return a const iterator to one-past-the-end of the children list.
	 */
	inline const_iterator end() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_children.end();
	}

	/**
	 * @brief Get a const iterator to the beginning of children list.
	 * 
	 * @return A const iterator to the beginning of the children list.
	 */
	inline const_iterator cbegin() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_children.cbegin();
	}

	/**
	 * @brief Get a const iterator to one-past-the-end of children list.
	 * 
	 * @return a const iterator to one-past-the-end of the children list.
	 */
	inline const_iterator cend() const BOOST_NOEXCEPT_OR_NOTHROW
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
	 * 
	 * @deprecated This method doesn't work. It's better to construct a view
	 * matrix manually. Perhaps in the future I'll get around to fixing this.
	 */
	void get_view_matrix(mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Get the global transformation matrix, i.e. from `MODEL` space
	 * to `WORLD` space.
	 * 
	 * @return A constant reference to the global transformation matrix.
	 */
	inline const mat4f& global_transform() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_global_transform;
	}

	/**
	 * @brief Get the global bounding box.
	 * 
	 * @return A constant reference to the global bounding box.
	 */
	inline const box3f& global_bounding_box() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_global_bounding_box;
	}

	//@}

	/**
	 * @name Components
	 */

	//@{

	/**
	 * @brief Get a pointer to the controller component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the controller component of this entity.
	 */
	inline controller* controller_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_controller_component;
	}

	/**
	 * @brief Get a constant pointer to the controller component of this
	 * entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the controller component of this entity.
	 */
	inline const controller* controller_component() const 
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_controller_component;
	}

	/**
	 * @brief Get a pointer to the mesh component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the mesh component of this entity.
	 */
	inline mesh* mesh_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_mesh_component;
	}

	/**
	 * @brief Get a constant pointer to the mesh component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the mesh component of this entity.
	 */
	inline const mesh* mesh_component() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_mesh_component;
	}

	/**
	 * @brief Get a pointer to the material component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the material component of this entity.
	 */
	inline material* material_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_material_component;
	}

	/**
	 * @brief Get a constant pointer to the material component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the material component of this entity.
	 */
	inline const material* material_component() const
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_material_component;
	}

	/**
	 * @brief Get a pointer to the light component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the light component of this entity.
	 */
	inline light* light_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_light_component;
	}

	/**
	 * @brief Get a constant pointer to the light component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the light component of this entity.
	 */
	inline const light* light_component() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_light_component;
	}

	/**
	 * @brief Get a pointer to the rigid_body component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the rigid_body component of this entity.
	 */
	inline rigid_body* rigid_body_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_rigid_body_component;
	}

	/**
	 * @brief Get a constant pointer to the rigid_body component of this
	 * entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the rigid_body component of this entity.
	 */
	inline const rigid_body* rigid_body_component() const 
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_rigid_body_component;
	}

	/**
	 * @brief Get a pointer to the AI component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the AI component of this entity.
	 */
	inline AI* AI_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_AI_component;
	}

	/**
	 * @brief Get a constant pointer to the AI component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the AI component of this entity.
	 */
	inline const AI* AI_component() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_AI_component;
	}

	/**
	 * @brief Get a pointer to the camera component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the camera component of this entity.
	 */
	inline camera* camera_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_camera_component;
	}

	/**
	 * @brief Get a constant pointer to the camera component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the camera component of this entity.
	 */
	inline const camera* camera_component() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_camera_component;
	}

	/**
	 * @brief Get a pointer to the proj_info component of this entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A pointer to the proj_info component of this entity.
	 */
	inline proj_info*  proj_info_component() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_proj_info_component;
	}

	/**
	 * @brief Get a constant pointer to the proj_info component of this
	 * entity.
	 * 
	 * @details The pointer can be null, so be sure to check for that.
	 * 
	 * @return A constant pointer to the proj_info component of this entity.
	 */
	inline const proj_info*  proj_info_component() const
		BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_proj_info_component;
	}

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
	inline octree* octree_node() BOOST_NOEXCEPT_OR_NOTHROW
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
	inline const octree* octree_node() const BOOST_NOEXCEPT_OR_NOTHROW
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
		BOOST_NOEXCEPT_OR_NOTHROW
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
		BOOST_NOEXCEPT_OR_NOTHROW
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
		BOOST_NOEXCEPT_OR_NOTHROW
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
		BOOST_NOEXCEPT_OR_NOTHROW
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
		BOOST_NOEXCEPT_OR_NOTHROW
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
		BOOST_NOEXCEPT_OR_NOTHROW
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