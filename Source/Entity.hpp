/**
 * @file Entity.hpp
 * @brief Defines the Entity class.
 * @author Raoul Wols
 */

#pragma once

#include "ForwardDeclarations.hpp"

#include "Foundation/Object.hpp"

#include "Math/SQT.hpp"
#include "Math/box3f.hpp"
#include "Math/mat4f.hpp"

#include "Graphics/ShadowBuffer.hpp"

#include <list>
#include <boost/signals2.hpp>

namespace gintonic {

class Octree;

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
class Entity : public Object<Entity, std::string>
{
public:

	/// The type of datastructure for the list of children.
	typedef std::list<std::shared_ptr<Entity>> children_datastructure_type;

	/// The iterator type for the list of children.
	typedef children_datastructure_type::iterator iterator;

	/// The constant iterator type for the list of children.
	typedef children_datastructure_type::const_iterator const_iterator;

private:

	SQT mLocalTransform;
	mat4f mGlobalTransform;

	children_datastructure_type mChildren;

	std::weak_ptr<Entity> mParent = std::shared_ptr<Entity>(nullptr);

	friend class Octree;

	Octree* mOctree = nullptr;

	void updateGlobalInfo() noexcept;

public:

	Entity(const FBXSDK_NAMESPACE::FbxNode* pFbxNode);
	Entity(const Entity&);
	Entity(Entity&&) noexcept;
	Entity& operator = (const Entity&);
	Entity& operator = (Entity&&) noexcept;

	/**
	 * @brief Constructor.
	 *
	 * @param local_transform The local transform that the Entity starts out
	 * with.
	 *
	 * @param local_bounding_box The local bounding box that the Entity starts
	 * out with.
	 *
	 * @param octree_root If the Entity should be part of an octree, you can
	 * pass the root of the octree here.
	 * 
	 * @param parent Wether the Entity should have a parent Entity.
	 */
	// Entity(
	// 	const SQT& local_transform,
	// 	const box3f& local_bounding_box,
	// 	octree* octree_root = nullptr,
	// 	std::shared_ptr<Entity> parent = std::shared_ptr<Entity>(nullptr));

	/**
	 * @brief Default constructor.
	 *
	 * @details The default constructor constructs an Entity with a local
	 * transform centered at the origin and with the standard unit axes of
	 * three-dimensional space. It has a trivial local bounding box, so a
	 * point. It has no parent and is not part of an octree.
	 */
	Entity();

	Entity(std::string name);

	Entity(std::string name, const SQT& localTransform);

	bool castShadow = false;

	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Light> light;
	std::shared_ptr<Camera> camera;
	std::unique_ptr<ShadowBuffer> shadowBuffer;

	/**
	 * @name Events
	 */

	//@{

	/**
	 * @brief Event that fires when this Entity is about to die.
	 *
	 * @param e A reference to the Entity that is about to die.
	 */
	boost::signals2::signal<void(Entity&)> onDie;

	/**
	 * @brief Event that fires when the global transform has changed.
	 *
	 * @param e A reference to the Entity whose global transform has changed.
	 */
	boost::signals2::signal<void(std::shared_ptr<Entity>)> onTransformChange;

	//@}

	/**
	 * @name Constructors and destructor
	 */

	//@{

	/// Destructor.
	virtual ~Entity() noexcept;

	/**
	 * @brief Clone this entity; clone all its children too.
	 * @return The cloned entity.
	 */
	std::shared_ptr<Entity> cloneRecursive() const;

	//@}

	/**
	 * @name Local SQT transform and bounding box management
	 */

	//@{

	/**
	 * @brief Set the scale of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 * @param s The new scale.
	 */
	void setScale(const vec3f& s) noexcept;

	/**
	 * @brief Multiply the current scale of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param s The scale to multiply the current local scale with.
	 */
	void multiplyScale(const vec3f& s) noexcept;

	/**
	 * @brief Set the translation of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param t The new translation.
	 */
	void setTranslation(const vec3f& t) noexcept;

	/**
	 * @brief Set the translation's X-coordinate of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param x The new X-coordinate.
	 */
	void setTranslationX(const float x) noexcept;

	/**
	 * @brief Set the translation's Y-coordinate of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param y The new Y-coordinate.
	 */
	void setTranslationY(const float y) noexcept;

	/**
	 * @brief Set the translation's Z-coordinate of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param z The new Z-coordinate.
	 */
	void setTranslationZ(const float z) noexcept;

	/**
	 * @brief Add a translation to the current SQT transform's translation.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param t The translation to add.
	 */
	void addTranslation(const vec3f& t) noexcept;

	/**
	 * @brief Set the rotation quaternion of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param q The new rotation quaternion.
	 */
	void setRotation(const quatf& q) noexcept;

	/**
	 * @brief Post-multiply the current rotation of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param q The rotation quaternion to post-multiply with.
	 */
	void postMultiplyRotation(const quatf& q) noexcept;

	/**
	 * @brief Pre-multiply the current rotation of the local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param q The rotation quaternion to pre-multiply with.
	 */
	void preMultiplyRotation(const quatf& q) noexcept;

	/**
	 * @brief Set the local SQT transform of this Entity.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param sqt The new SQT transform.
	 */
	void setLocalTransform(const SQT& sqt) noexcept;

	/**
	 * @brief Post-add an SQT to the current local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param sqt The SQT transform to post-add.
	 */
	void postAddLocalTransform(const SQT& sqt) noexcept;

	/**
	 * @brief Pre-add an SQT to the current local SQT transform.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param sqt The SQT transform to pre-add.
	 */
	void preAddLocalTransform(const SQT& sqt) noexcept;

	/**
	 * @brief Move the Entity in the direction of the local forward direction.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void moveForward(const float amount) noexcept;

	/**
	 * @brief Move the Entity in the direction of the local backward
	 * direction.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void moveBackward(const float amount) noexcept;

	/**
	 * @brief Move the Entity in the direction of the local right direction.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void moveRight(const float amount) noexcept;

	/**
	 * @brief Move the Entity in the direction of the local left direction.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void moveLeft(const float amount) noexcept;

	/**
	 * @brief Move the Entity in the direction of the local up direction.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void moveUp(const float amount) noexcept;

	/**
	 * @brief Move the Entity in the direction of the local down direction.
	 *
	 * @details If the Entity has children, this will update the global
	 * transform of all of its children.
	 *
	 * @param amount The amount of translation.
	 */
	void moveDown(const float amount) noexcept;

	/**
	 * @brief Get the local SQT transform.
	 *
	 * @return A constant reference to the local SQT transform.
	 */
	inline const SQT& localTransform() const noexcept
	{
		return mLocalTransform;
	}

	//@}

	/**
	 * @name Children entities and parent Entity management
	 */

	//@{

	/**
	 * @brief Add a child Entity to this Entity.
	 *
	 * @details If the child Entity already had a parent, then this parent
	 * is first removed from the Entity. In the old parent Entity, the child
	 * Entity is searched for and removed from the list.
	 *
	 * @param child The child Entity.
	 */
	void addChild(std::shared_ptr<Entity> child);

	/**
	 * @brief Remove a child Entity of this Entity.
	 *
	 * @details If the child Entity is not present, then nothing will happen.
	 * The parent pointer of the child Entity will be set to null.
	 *
	 * @param child The child Entity to remove.
	 */
	void removeChild(std::shared_ptr<Entity> child);

	/**
	 * @brief Remove a child Entity of this Entity.
	 *
	 * @param iter An iterator pointing to a child.
	 */
	void removeChild(iterator child);

	/**
	 * @brief Set the parent of this Entity.
	 *
	 * @details If the child already has a parent, then this parent is first
	 * removed. In the parent's children list, this Entity is searched for
	 * and removed.
	 *
	 * @param parent The parent Entity.
	 */
	void setParent(std::shared_ptr<Entity> parent);

	/**
	 * @brief Set this Entity to have no parent.
	 *
	 * @details The parent pointer will be set to null. If this Entity has a
	 * parent, then this Entity is serached for in the children's list of the
	 * parent Entity and removed.
	 */
	void unsetParent();


	/**
	 * @brief Get a pointer to the parent of this Entity.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 * In order to get an actual pointer, use the following construct:
	 * if (auto ptr = e->parent().lock())
	 * {
	 *   // do stuff with ptr
	 * }
	 *
	 * @return A weak pointer to the parent of this Entity.
	 */
	inline std::weak_ptr<Entity> parent() noexcept
	{
		return mParent;
	}

	/**
	 * @brief Get a constant pointer to the parent of this Entity.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 *
	 * @return A constant pointer to the parent of this Entity.
	 */
	inline const std::weak_ptr<const Entity> parent() const noexcept
	{
		return mParent;
	}

	/**
	 * @brief Get an iterator to the beginning of children list.
	 *
	 * @return An iterator to the beginning of the children list.
	 */
	inline iterator begin() noexcept
	{
		return mChildren.begin();
	}

	/**
	 * @brief Get an iterator to one-past-the-end of children list.
	 *
	 * @return An iterator to one-past-the-end of the children list.
	 */
	inline iterator end() noexcept
	{
		return mChildren.end();
	}

	/**
	 * @brief Get a const iterator to the beginning of children list.
	 *
	 * @return A const iterator to the beginning of the children list.
	 */
	inline const_iterator begin() const noexcept
	{
		return mChildren.begin();
	}

	/**
	 * @brief Get a const iterator to one-past-the-end of children list.
	 *
	 * @return a const iterator to one-past-the-end of the children list.
	 */
	inline const_iterator end() const noexcept
	{
		return mChildren.end();
	}

	/**
	 * @brief Get a const iterator to the beginning of children list.
	 *
	 * @return A const iterator to the beginning of the children list.
	 */
	inline const_iterator cbegin() const noexcept
	{
		return mChildren.cbegin();
	}

	/**
	 * @brief Get a const iterator to one-past-the-end of children list.
	 *
	 * @return a const iterator to one-past-the-end of the children list.
	 */
	inline const_iterator cend() const noexcept
	{
		return mChildren.cend();
	}

	//@}

	/**
	 * @name Global information
	 */

	//@{

	/**
	 * @brief Get the `VIEW` matrix of this Entity.
	 *
	 * @param m The matrix to set.
	 * 
	 * @sa Entity::updateViewMatrix
	 */
	void getViewMatrix(mat4f& m) const noexcept;

	/**
	 * @brief Update the given `VIEW` matrix.
	 * @details The assumption is that the given matrix
	 * is already an affine matrix. If it's not, this 
	 * method will produce incorrect results. This method
	 * is a tiny bit more efficient than Entity::getViewMatrix
	 * 
	 * @param alreadyAffineMatrix A mutable reference to a
	 * matrix which is assumed to already be affine.
	 * 
	 * @sa Entity::getViewMatrix
	 */
	void updateViewMatrix(mat4f& alreadyAffineMatrix) const noexcept;

	/**
	 * @brief Get the `VIEW` matrix of this Entity.
	 * @return The view matrix.
	 * 
	 * @sa Entity::updateViewMatrix
	 */
	mat4f getViewMatrix() const noexcept;

	/**
	 * @brief Get the global transformation matrix, i.e. from `MODEL` space
	 * to `WORLD` space.
	 *
	 * @return A constant reference to the global transformation matrix.
	 */
	inline const mat4f& globalTransform() const noexcept
	{
		return mGlobalTransform;
	}

	/**
	 * @brief Get the global bounding box.
	 *
	 * @return A constant reference to the global bounding box.
	 */
	box3f globalBoundingBox() const noexcept;

	//@}

	/**
	 * @name Comparison operators
	 */

	//@{

	/**
	 * @brief Two entities compare equal if they live at the same memory
	 * address.
	 *
	 * @param other Another Entity.
	 *
	 * @return True if they live at the same memory address, false otherwise.
	 */
	inline bool operator == (const Entity& other) const
		noexcept
	{
		return this == &other;
	}

	/**
	 * @brief Two entities compare unequal if they don't live at the same
	 * memory address.
	 *
	 * @param other Another Entity.
	 *
	 * @return True if they live don't at the same memory address,
	 * false otherwise.
	 */
	inline bool operator != (const Entity& other) const
		noexcept
	{
		return !operator==(other);
	}

	/**
	 * @brief Compares less via the memory addresses.
	 *
	 * @param other Another Entity.
	 *
	 * @return True if this Entity's memory address is less than the other
	 * Entity's memory address. False otherwise.
	 */
	inline bool operator < (const Entity& other) const
		noexcept
	{
		return this < &other;
	}

	/**
	 * @brief Compares greater-than-or-equal via the memory addresses.
	 *
	 * @param other Another Entity.
	 *
	 * @return True if this Entity's memory address is greater than or equal
	 * to the other Entity's memory address. False otherwise.
	 */
	inline bool operator >= (const Entity& other) const
		noexcept
	{
		return !operator<(other);
	}

	/**
	 * @brief Compares greater-than via the memory addresses.
	 *
	 * @param other Another Entity.
	 *
	 * @return True if this Entity's memory address is greater than the other
	 * Entity's memory address. False otherwise.
	 */
	inline bool operator > (const Entity& other) const
		noexcept
	{
		return this > &other;
	}

	/**
	 * @brief Compares less-than-or-equal-to via the memory addresses.
	 *
	 * @param other Another Entity.
	 *
	 * @return True if this Entity's memory address is less than or equal to
	 * the other Entity's memory address. False otherwise.
	 */
	inline bool operator <= (const Entity& other) const
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
