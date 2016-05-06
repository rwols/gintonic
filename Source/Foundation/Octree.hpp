/**
 * @file Octree.hpp
 * @brief Defines an Octree datastructure.
 * @author Raoul Wols
 */

#pragma once

#include "../Math/box3f.hpp"
#include "../Entity.hpp"
#include <list>

namespace gintonic {

/**
 * @brief An Octree datastructure.
 */
class Octree
{
private:

	// friend class Entity;

	struct EntityHolder
	{
		std::weak_ptr<Entity>       entity;
		boost::signals2::connection transformChangeConnection;
		boost::signals2::connection destructConnection;

		EntityHolder()                                = default;
		EntityHolder(const EntityHolder&)             = default;
		EntityHolder(EntityHolder&&)                  = default;
		EntityHolder& operator= (const EntityHolder&) = default;
		EntityHolder& operator= (EntityHolder&&)      = default;
		~EntityHolder() noexcept                      = default;

		template <class A, class B, class C>
		EntityHolder(A&& a, B&& b, C&& c)
		: entity(std::forward<A>(a))
		, transformChangeConnection(std::forward<B>(b))
		, destructConnection(std::forward<C>(c))
		{
			/* Empty on purpose. */
		}
	};

	Octree* mParent = nullptr;
	Octree* mChild[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	box3f mBounds;
	std::list<EntityHolder> mEntities;

public:

	enum class ErasureStatus
	{
		entityNotPresent, 
		EntityRemovedAndOctreeNodeNotRemoved, 
		EntityRemovedAndOctreeNodeRemoved
	};

	/**
	 * @brief Subdivision threshold.
	 *
	 * @details The subdivision threshold is the minimum of the width and
	 * height of a bounding box such that no more subdivision takes place.
	 * After the Octree has reached this point, all objects will just be
	 * placed in this Octree.
	 */
	float subdivisionThreshold = 1.0f;

	/**
	 * @brief Constructor that takes a bounding box.
	 *
	 * @param b The bounding box of the Octree.
	 */
	Octree(const box3f& b);

	/**
	 * @brief Constructor that takes a bounding box.
	 * @param minCorner The minimum corner of the bounding box.
	 * @param maxCorner The maximum corner of the bounding box.
	 */
	Octree(const vec3f& minCorner, const vec3f& maxCorner);
	
	/**
	 * @brief Constructor that inserts elements from a container.
	 * @tparam ForwardIter The forward iterator type.
	 * @param b The bounding box of the Octree.
	 * @param first Iterator pointing to the first element.
	 * @param last Iterator pointing to one-past-the-end element.
	 */
	template <class ForwardIter> 
	Octree(
		const box3f& b, 
		ForwardIter first, 
		ForwardIter last);

	// Constructor that inserts elements at construction time.
	
	/**
	 * @brief Constructor that inserts elements from a container.
	 * @tparam ForwardIter The forward iterator type.
	 * @param minCorner The minimum corner of the bounding box.
	 * @param maxCorner the maximum corner of the bounding box.
	 * @param first Iterator pointing to the first element.
	 * @param last Iterator pointing to one-past-the-end element.
	 */
	template <class ForwardIter> 
	Octree(
		const vec3f& minCorner, 
		const vec3f& maxCorner, 
		ForwardIter first, 
		ForwardIter last);

	/**
	 * @brief Copy constructor.
	 *
	 * @details This copies all of the children too. However, it does not
	 * copy the parent. The parent of the new copy will be a null pointer.
	 * Essentially, it will create a new Octree with the copy as a new root.
	 *
	 * @param other Another Octree.
	 */
	Octree(const Octree& other);

	/**
	 * @brief Move constructor.
	 *
	 * @details This will also move the parent. Note that this is a big
	 * contrast with the copy constructor.
	 *
	 * @param other Another Octree.
	 */
	Octree(Octree&& other);

	/**
	 * @brief Copy assignment operator.
	 *
	 * @details This copies all of the children too. However, it does not
	 * copy the parent. The parent of the new copy will be a null pointer.
	 * Essentially, it will create a new Octree with the copy as a new root.
	 *
	 * @param other Another Octree.
	 *
	 * @return *this.
	 */
	Octree& operator = (const Octree& other);

	/**
	 * @brief Move assignment operator.
	 *
	 * @details This will also move the parent. Note that this is a big
	 * contrast with the copy constructor.
	 *
	 * @param other Another Octree.
	 *
	 * @return *this.
	 */
	Octree& operator = (Octree&& other);

	/**
	 * @brief Destructor.
	 *
	 * @details The destructor calls delete on all of
	 * the children entities.
	 */
	~Octree();

	/**
	 * @brief Get the axis-aligned bounding box of this Octree.
	 * @return A const reference to the axis-aligned bounding box.
	 */
	inline const box3f& bounds() const noexcept
	{
		return mBounds;
	}

	/**
	 * @brief Check wether this Octree is a root.
	 * @details An Octree is a root if it has no parent.
	 * @return True if this Octree is a root, false otherwise.
	 */
	inline bool isRoot() const noexcept
	{
		return !mParent;
	}

	/**
	 * @brief Check wether this Octree is a leaf.
	 * @details An Octree is a leaf if it has no children.
	 * @return True if this Octree is a leaf, false otherwise.
	 */
	bool isLeaf() const noexcept;

	/**
	 * @brief Check wether this Octree has no entities it refers to.
	 * @return True if it houses no entities, false otherwise.
	 */
	inline bool hasNoEntities() const noexcept
	{
		return mEntities.empty();
	}

	/**
	 * @brief Get the cumulative sum of this Octree and all of its children.
	 * @details This method recurses into the tree, so cache the result if
	 * you need it multiple times.
	 * @return The cumulative sum of this Octree and all of its children.
	 */
	std::size_t count() const;

	/**
	 * @brief Get the entities of this Octree and of its children too.
	 * @tparam OutputIter The output iterator type.
	 * @param iter An output iterator.
	 */
	template <class OutputIter> 
	void getEntities(OutputIter iter);

	/**
	 * @brief Get the entities of this Octree and of its children too.
	 * @tparam OutputIter The output iterator type.
	 * @param iter An output iterator.
	 */
	template <class OutputIter> 
	void getEntities(OutputIter iter) const;

	/**
	 * @brief Query an area to obtain all the entities in that area.
	 * @details This method is the raison d'etre for using an Octree. The
	 * runtime complexity is \f$O(\log N)\f$, where \f$N\f$ is the total
	 * number of entities in the tree. This is the non-const version, so
	 * you'll get a container of mutable entities.
	 */
	template <class OutputIter>
	void query(const box3f& area, OutputIter iter);

	/**
	 * @brief Query an area to obtain all the entities in that area.
	 * @details This method is the raison d'etre for using an Octree. The
	 * runtime complexity is \f$O(\log N)\f$, where \f$N\f$ is the total
	 * number of entities in the tree. This is the const version, so you'll 
	 * get a container of immutable entities.
	 */
	template <class OutputIter>
	void query(const box3f& area, OutputIter iter) const;

	/**
	 * @brief Apply a function to every Entity.
	 * @tparam Func Type of a function pointer, lambda, functor, etc.
	 * @param f A function pointer, lambda, functor, etc.
	 */
	template <class Func> 
	void foreach(Func f);

	/**
	 * @brief Apply a function to every Entity, const version.
	 * @tparam Func Type of a function pointer, lambda, functor, etc.
	 * @param f A function pointer, lambda, functor, etc.
	 */
	template <class Func>
	void foreach(Func f) const;

	template <class Func>
	void forEachNode(Func f);

	template <class Func>
	void forEachNode(Func f) const;

	/**
	 * @brief Insert an Entity into the tree.
	 * @details This method recurses down into the tree if the Entity can
	 * fit into a smaller box. It will also recursively create new nodes if
	 * needed.
	 * @param entity The Entity to insert.
	 */
	void insert(std::shared_ptr<Entity> entity);

	/**
	 * @brief Erase an Entity from the tree.
	 * @details This method does not recurse down into the tree. It will only
	 * attempt to erase the Entity in this node's Entity list. If no entities
	 * remain in the node's list and if this is a leaf node, then the node
	 * will delete itself. This process continues up the tree.
	 * @param entity The Entity to erase.
	 * @return The farthest parent who still has non-leaf children, or entities
	 * inside it, or the root node, or nullptr if the entity was not present in
	 * this Octree node. Client code should only check for nullptr because that
	 * signals a logical error.
	 */
	Octree* erase(std::shared_ptr<Entity> entity);

	// Notify the Octree that an Entity's global bounding box
	// has changed. This can result in possibly mutating the tree,
	// even changing the parent.
	
	/**
	 * @brief Notify this Octree node that the Entity has moved.
	 * @deprecated We probably want to use an event system for this. An
	 * Entity right now has an event that fires when its global transform has
	 * changed so the Octree should probably just subscribe to that event and
	 * do its thing via this route.
	 * @param entity The Entity whose global transform has changed.
	 */
	void notify(std::shared_ptr<Entity> entity);

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	Octree(const float subdivisionThreshold, Octree*, const box3f&);

	Octree(const float subdivisionThreshold, Octree* parent, const vec3f& min, const vec3f& max);

	void backRecursiveInsert(std::shared_ptr<Entity>);
	Octree* backRecursiveDelete();

	void notifyHelper(std::shared_ptr<Entity>);

	void subdivide();
};

template <class ForwardIter>
Octree::Octree(
	const box3f& bounds,
	ForwardIter first, ForwardIter last)
: mBounds(bounds)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
	while (first != last)
	{
		this->insert(&(*first));
		++first;
	}
}

template <class ForwardIter>
Octree::Octree(
	const vec3f& minCorner, 
	const vec3f& maxCorner, 
	ForwardIter first, 
	ForwardIter last)
: mBounds(minCorner, maxCorner)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
	while (first != last)
	{
		this->insert(&(*first));
		++first;
	}
}

// Get the entities of this Octree and of its children too.
// This method is recursive.
template <class OutputIter> 
void Octree::getEntities(OutputIter iter)
{
	for (auto& lHolder : mEntities)
	{
		if (auto lEntityPtr = lHolder.entity.lock())
		{
			*iter = lEntityPtr;
			++iter;
		}
	}
	for (auto* lChildNode : mChild) if (lChildNode) lChildNode->getEntities(iter);
}

// Get the entities of this Octree and of its children too.
// This method is recursive. This is the const version.
template <class OutputIter> 
void Octree::getEntities(OutputIter iter) const
{
	for (const auto& lHolder : mEntities)
	{
		if (const auto lEntityPtr = lHolder.entity.lock())
		{
			*iter = lEntityPtr;
			++iter;
		}
	}
	for (const auto* lChildNode : mChild) if (lChildNode) lChildNode->getEntities(iter);
}

// Query an area defined by a box3f to obtain all the entities
// in this area. This method is the raison d'etre for using
// a Octree.
// The runtime complexity is O(log N), where N is the total
// number of entities.
// This is the non-const version. You'll get a container
// of mutable entities.
template <class OutputIter>
void Octree::query(const box3f& area, OutputIter iter)
{
	for (auto& lHolder : mEntities)
	{
		if (auto lEntityPtr = lHolder.entity.lock())
		{
			if (intersects(area, lEntityPtr->globalBoundingBox()))
			{
				++iter;
				*iter = lEntityPtr;
			}
		}
	}
	for (auto* lChildNode : mChild)
	{
		// Case 0: If there is no child node, continue
		if (lChildNode == nullptr)
		{
			// DEBUG_PRINT;
			continue;
		}
		// Case 1: search area completely contained by sub-quad
		// if a node completely contains the query area, go down that
		// branch and skip the remaining nodes (break this loop)
		//
		// +-----+-----+
		// | +-+ | skip|
		// | |s| | skip|
		// | +-+ | skip|
		// +-----+-----+
		// |skip | skip|
		// |skip | skip|
		// |skip | skip|
		// +-----+-----+
		//
		if (lChildNode->mBounds.contains(area))
		{
			// DEBUG_PRINT;
			lChildNode->query(area, iter);
			break;
		}
		// Case 2: Sub-quad completely contained by search area 
		// if the query area completely contains a sub-quad,
		// just add all the contents of that quad and its children 
		// to the result set. You need to continue the loop to test 
		// the other quads
		//
		// +---------+
		// |sssssssss|
		// |s+-----+-+---+
		// |s|sssss|s| ? |
		// |s|sssss|s| ? |
		// |s|sssss|s| ? |
		// |s+-----+-+---+
		// |s|sssss|s| ? |
		// +-+-----+-+ ? |
		//   | ??? | ??? |
		//   +-----+-----+
		//
		else if (area.contains(lChildNode->mBounds))
		{
			// DEBUG_PRINT;
			lChildNode->getEntities(iter);
		}
		// Case 3: search area intersects with sub-quad
		// traverse into this quad, continue the loop to search other
		// quads
		//
		// +-----+
		// |sssss|
		// |s+---+-+-----+
		// |s|sss| | ??? |
		// +-+---+ | ??? |
		//   |     | ??? |
		//   +-----+-----+
		//   | ??? | ??? |
		//   | ??? | ??? |
		//   | ??? | ??? |
		//   +-----+-----+
		//
		else if (intersects(area, lChildNode->mBounds))
		{
			// DEBUG_PRINT;
			lChildNode->query(area, iter);
		}
		// Case 4: The search area and sub-quad are disjoint.
		else
		{
			// DEBUG_PRINT;
			// throw std::logic_error(":(");
		}
	}
}

// Query an area defined by a box3f to obtain all the entities
// in this area. This method is the raison d'etre for using
// a Octree.
// The runtime complexity is O(log N), where N is the total
// number of entities.
// This is the const version.
template <class OutputIter>
void Octree::query(const box3f& area, OutputIter iter) const
{
	for (const auto& lHolder : mEntities)
	{
		if (const auto lEntityPtr = lHolder.entity.lock())
		{
			if (intersects(area, lEntityPtr->globalBoundingBox()))
			{
				++iter;
				*iter = lEntityPtr;
			}
		}

	}
	for (const auto* lChildNode : mChild)
	{
		// Case 0: If there is no child node, continue
		if (lChildNode == nullptr) continue;
		// Case 1: search area completely contained by sub-quad
		// if a node completely contains the query area, go down that
		// branch and skip the remaining nodes (break this loop)
		if (lChildNode->mBounds.contains(area))
		{
			lChildNode->query(area, iter);
			break;
		}
		// Case 2: Sub-quad completely contained by search area 
		// if the query area completely contains a sub-quad,
		// just add all the contents of that quad and it's children 
		// to the result set. You need to continue the loop to test 
		// the other quads
		else if (area.contains(lChildNode->mBounds))
		{
			lChildNode->getEntities(iter);
			continue;
		}
		// Case 3: search area intersects with sub-quad
		// traverse into this quad, continue the loop to search other
		// quads
		else if (intersects(area, lChildNode->mBounds))
		{
			lChildNode->query(area, iter);
			continue;
		}
	}
}

template <class Func> 
void Octree::foreach(Func f)
{
	for (auto& o : mEntities) if (auto ptr = o.entity.lock()) f(ptr);
	for (auto* c : mChild) if (c) c->foreach(f);
}

template <class Func>
void Octree::foreach(Func f) const
{
	for (const auto& o : mEntities) if (const auto ptr = o.entity.lock()) f(ptr);
	for (const auto* c : mChild) if (c) c->foreach(f);
}

template <class Func>
void Octree::forEachNode(Func f)
{
	f(this);
	for (auto* lChildNode : mChild) if (lChildNode) lChildNode->forEachNode(f);
}

template <class Func>
void Octree::forEachNode(Func f) const
{
	f(this);
	for (auto* lChildNode : mChild) if (lChildNode) lChildNode->forEachNode(f);
}

} // namespace gintonic
