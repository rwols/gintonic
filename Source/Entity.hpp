/**
 * @file Entity.hpp
 * @brief Defines the Entity class.
 * @author Raoul Wols
 */

#pragma once

#include "ForwardDeclarations.hpp"
#include "Camera.hpp"

#include "Math/SQT.hpp"
#include "Math/box3f.hpp"
#include "Math/mat4f.hpp"

#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Light.hpp"
#include "Graphics/ShadowBuffer.hpp"

#include <list>
#include <boost/signals2.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

/**
 * @brief Represents an Entity in the world. An Entity can only be
 * instantiated on the heap, using the static method Entity::create.
 * This method is (variadic) templated, simply calling an appropriate
 * Entity constructor. It returns a shared pointer to an Entity.
 * An Entity carries various components, mostly via shared pointers.
 * Examples of components are Material, Mesh and Light.
 */
class Entity : public Object<Entity, std::string>
{
public:

	/// The type of datastructure for the list of children.
	typedef std::list<Entity::SharedPtr> children_datastructure_type;

	/// The iterator type for the list of children.
	typedef children_datastructure_type::iterator iterator;

	/// The constant iterator type for the list of children.
	typedef children_datastructure_type::const_iterator const_iterator;

private:

	SQT mLocalTransform;
	mat4f mGlobalTransform;

	children_datastructure_type mChildren;

	WeakPtr mParent = SharedPtr(nullptr);

	friend class Octree;

	Octree* mOctree = nullptr;
	typename std::list<std::tuple<std::weak_ptr<Entity>, boost::signals2::connection, boost::signals2::connection>>::iterator mOctreeListIter;

	void updateGlobalInfo() noexcept;

public:

	/**
	 * @name Constructors and destructor
	 */

	//@{

	/**
	 * @brief Creates a new Entity::SharedPtr with the given constructor arguments.
	 * Note that this is the *only* way to create new a new Entity. It's impossible
	 * to create an Entity on the stack *by design*, and it is also impossible to
	 * create a new Entity with std::make_shared *by design*.
	 * @param[in] args The constructor argsuments.
	 * @tparam Args The variadic template pack.
	 * @return A new Entity::SharedPtr.
	 * @sa Entity::Entity
	 */
	template <class ...Args>
	inline static SharedPtr create(Args&&... args)
	{
		// Don't use std::make_shared<...>(...)
		// because Entity::operator new and co. need to be called
		// explicitly, while std::make_shared uses its own allocation
		// strategy. In practise, this means std::make_shared does not
		// allocate on a 16-byte boundary, which is required for Entity
		// to function properly.
		return SharedPtr(new Entity(std::forward<Args>(args)...));
	}

private:

	/**
	 * @brief Default constructor.
	 * @details The default constructor constructs an Entity with a local
	 * transform centered at the origin and with the standard unit axes of
	 * three-dimensional space. It has no parent and is not part of an Octree.
	 * The name will be "UntitledEntity".
	 */
	Entity();

	/**
	 * @brief Construct an Entity with a given name.
	 * @details This constructor constructs an Entity with a local
	 * SQT centered at the origin and with the standard unit axes of
	 * three-dimensional space. It has no parent and is not part of an Octree.
	 * @param name The name of the new Entity.
	 */
	Entity(std::string name);

	/**
	 * @brief Construct an Entity with a given name and local SQT.
	 * @param name The name of the new Entity.
	 * @param localTransform The SQT of the new Entity.
	 */
	Entity(std::string name, const SQT& localTransform);

	/**
	 * @brief Build an Entity from an FbxNode.
	 * @param pFbxNode Pointer to an FbxNode.
	 */
	Entity(const FBXSDK_NAMESPACE::FbxNode* pFbxNode);

	/**
	 * @brief Copy constructor.
	 * The copy constructor does not copy the children
	 * and the parent. Nor does it copy the ShadowBuffer.
	 * @param other Another entity.
	 */
	Entity(const Entity& other);

	/**
	 * @brief Move constructor.
	 * The move constructor does move the children
	 * and the parent. It also moves the ShadowBuffer.
	 * @param other Another entity.
	 */
	Entity(Entity&& other) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * The copy assignment operator does not copy the children
	 * and the parent. Nor does it copy the ShadowBuffer.
	 * @param other Another entity.
	 */
	Entity& operator = (const Entity& other);

	/**
	 * @brief Move assignment operator.
	 * The move assignment operator does move the children
	 * and the parent. It also moves the ShadowBuffer.
	 * @param other Another entity.
	 */
	Entity& operator = (Entity&& other) noexcept;

public:

	/**
	 * @brief Highly non-trivial destructor. See details.
	 * @details The destructor of an Entity first fires the event onDie.
	 * It encloses this call into a try-catch block. Every possible
	 * exception is caught and consumed. The exception is then ignored.
	 * If the Entity is part of an Octree, it notifies the Octree of its
	 * destruction so that the Octree removes this Entity. Next, for
	 * each of the children of this Entity, the global transform is
	 * decomposed into a scale, rotation and translation. The parent is
	 * then set to a null pointer, and the local transform is set to
	 * the computed scale, rotation and translation. That is, the global
	 * transform becomes the local transform. Finally, if this Entity
	 * has a parent then it is detached.
	 */
	virtual ~Entity() noexcept;

	/**
	 * @brief Clone this entity; clone all its children too.
	 * @return The cloned entity.
	 */
	SharedPtr cloneRecursive() const;

	//@}

	/**
	 * @brief Set wether this Entity casts a shadow.
	 * @details If the Entity has a Light component, then
	 * the Light will cast a shadow. Do not manage the
	 * ShadowBuffer yourself, the Renderer takes care of that.
	 * If the Entity has a Mesh and Material component, then
	 * it will receive shadow.
	 */
	bool castShadow = false;

	/**
	 * @brief The Material associated to this Entity.
	 */
	Material::SharedPtr material;

	/**
	 * @brief The Mesh associated to this Entity.
	 */
	Mesh::SharedPtr mesh;

	/**
	 * @brief The Light associated to this Entity.
	 */
	Light::SharedPtr light;

	/**
	 * @brief The Camera associated to this Entity.
	 */
	Camera::SharedPtr camera;

	/**
	 * @brief The ShadowBuffer associated to this Entity.
	 */
	std::unique_ptr<ShadowBuffer> shadowBuffer;

	/**
	 * @name Events
	 */

	//@{

	/**
	 * @brief Event that fires when this Entity is about to die.
	 * @param e A pointer to a const Entity that is about to die.
	 */
	boost::signals2::signal<void(Entity*)> onDie;

	/**
	 * @brief Event that fires when the global transform has changed.
	 * @param e A reference to the Entity whose global transform has changed.
	 */
	boost::signals2::signal<void(SharedPtr)> onTransformChange;

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
	 * If the child Entity already had a parent, then this parent
	 * is first removed from the Entity. In the old parent Entity, the child
	 * Entity is searched for and removed from the list.
	 *
	 * @param child The child Entity.
	 */
	void addChild(std::shared_ptr<Entity> child);

	/**
	 * @brief Remove a child Entity of this Entity.
	 * If the child Entity is not present, then nothing will happen.
	 * The parent pointer of the child Entity will be set to null.
	 * @param child The child Entity to remove.
	 */
	void removeChild(std::shared_ptr<Entity> child);

	/**
	 * @brief Remove a child Entity of this Entity.
	 * @param child An iterator pointing to a child.
	 */
	void removeChild(iterator child);

	/**
	 * @brief Set the parent of this Entity.
	 * If the child already has a parent, then this parent is first
	 * removed. In the parent's children list, this Entity is searched for
	 * and removed.
	 * @param parent The parent Entity.
	 */
	void setParent(std::shared_ptr<Entity> parent);

	/**
	 * @brief Set this Entity to have no parent.
	 * The parent pointer will be set to null. If this Entity has a
	 * parent, then this Entity is serached for in the children's list of the
	 * parent Entity and removed.
	 */
	void unsetParent();

	/**
	 * @brief Get a pointer to the parent of this Entity.
	 *
	 * @details The pointer can be null, so be sure to check for that.
	 * In order to get an actual pointer, use the following construct:
	 * @code
	 * if (auto ptr = e->parent().lock())
	 * {
	 *   // do stuff with ptr
	 * }
	 * @endcode
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

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& archive, const unsigned int /*version*/)
	{
		DEBUG_PRINT;
		archive & boost::serialization::base_object<Object<Entity, std::string>>(*this);
		DEBUG_PRINT;
		archive & mLocalTransform;
		DEBUG_PRINT;
		archive & mGlobalTransform;
		DEBUG_PRINT;
		archive & mParent;
		DEBUG_PRINT;
		archive & castShadow;
		DEBUG_PRINT;
		archive & material;
		DEBUG_PRINT;
		archive & mesh;
		DEBUG_PRINT;
		archive & light;
		DEBUG_PRINT;
		archive & camera;
		DEBUG_PRINT;
		archive & mChildren;
		DEBUG_PRINT;
	}
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Entity, boost::serialization::track_always);