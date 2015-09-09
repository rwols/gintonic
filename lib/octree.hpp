/**
 * @file octree.hpp
 * @brief Defines an octree datastructure.
 * @author Raoul Wols
 */

#ifndef gintonic_octree_hpp
#define gintonic_octree_hpp

#include "box3f.hpp"
#include "entity.hpp"
#include <list>

namespace gintonic {

/**
 * @brief An octree datastructure.
 */
class octree
{
private:

	friend class entity;

	octree* m_parent = nullptr;
	box3f m_bounds;
	std::list<entity*> m_entities;
	octree* m_child[8];

public:

	/**
	 * @brief Subdivision threshold.
	 *
	 * @details The subdivision threshold is the minimum of the width and
	 * height of a bounding box such that no more subdivision takes place.
	 * After the octree has reached this point, all objects will just be
	 * placed in this octree.
	 */
	float subdivision_threshold = 5.0f;

	/**
	 * @brief Constructor that takes a bounding box.
	 *
	 * @param b The bounding box of the octree.
	 */
	octree(const box3f& b);

	/**
	 * @brief Constructor that takes a bounding box.
	 * @param min_corner The minimum corner of the bounding box.
	 * @param max_corner The maximum corner of the bounding box.
	 */
	octree(const vec3f& min_corner, const vec3f& max_corner);
	
	/**
	 * @brief Constructor that inserts elements from a container.
	 * @tparam ForwardIter The forward iterator type.
	 * @param b The bounding box of the octree.
	 * @param first Iterator pointing to the first element.
	 * @param last Iterator pointing to one-past-the-end element.
	 */
	template <class ForwardIter> 
	octree(
		const box3f& b, 
		ForwardIter first, 
		ForwardIter last);

	// Constructor that inserts elements at construction time.
	
	/**
	 * @brief Constructor that inserts elements from a container.
	 * @tparam ForwardIter The forward iterator type.
	 * @param min_corner The minimum corner of the bounding box.
	 * @param max_corner the maximum corner of the bounding box.
	 * @param first Iterator pointing to the first element.
	 * @param last Iterator pointing to one-past-the-end element.
	 */
	template <class ForwardIter> 
	octree(
		const vec3f& min_corner, 
		const vec3f& max_corner, 
		ForwardIter first, 
		ForwardIter last);

	/**
	 * @brief Copy constructor.
	 *
	 * @details This copies all of the children too. However, it does not
	 * copy the parent. The parent of the new copy will be a null pointer.
	 * Essentially, it will create a new octree with the copy as a new root.
	 *
	 * @param other Another octree.
	 */
	octree(const octree& other);

	/**
	 * @brief Move constructor.
	 *
	 * @details This will also move the parent. Note that this is a big
	 * contrast with the copy constructor.
	 *
	 * @param other Another octree.
	 */
	octree(octree&& other);

	/**
	 * @brief Copy assignment operator.
	 *
	 * @details This copies all of the children too. However, it does not
	 * copy the parent. The parent of the new copy will be a null pointer.
	 * Essentially, it will create a new octree with the copy as a new root.
	 *
	 * @param other Another octree.
	 *
	 * @return *this.
	 */
	octree& operator = (const octree& other);

	/**
	 * @brief Move assignment operator.
	 *
	 * @details This will also move the parent. Note that this is a big
	 * contrast with the copy constructor.
	 *
	 * @param other Another octree.
	 *
	 * @return *this.
	 */
	octree& operator = (octree&& other);

	/**
	 * @brief Destructor.
	 *
	 * @details The destructor calls delete on all of
	 * the children entities.
	 */
	~octree();

	/**
	 * @brief Get the axis-aligned bounding box of this octree.
	 * @return A const reference to the axis-aligned bounding box.
	 */
	inline const box3f& bounds() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_bounds;
	}

	/**
	 * @brief Check wether this octree is a root.
	 * @details An octree is a root if it has no parent.
	 * @return True if this octree is a root, false otherwise.
	 */
	inline bool is_root() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !m_parent;
	}

	/**
	 * @brief Check wether this octree is a leaf.
	 * @details An octree is a leaf if it has no children.
	 * @return True if this octree is a leaf, false otherwise.
	 */
	bool is_leaf() const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Check wether this octree has no entities it refers to.
	 * @return True if it houses no entities, false otherwise.
	 */
	inline bool has_no_entities() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_entities.empty();
	}

	/**
	 * @brief Get the cumulative sum of this octree and all of its children.
	 * @details This method recurses into the tree, so cache the result if
	 * you need it multiple times.
	 * @return The cumulative sum of this octree and all of its children.
	 */
	std::size_t count() const;

	/**
	 * @brief Get the entities of this octree and of its children too.
	 * @tparam OutputIter The output iterator type.
	 * @param iter An output iterator.
	 */
	template <class OutputIter> 
	void get_entities(OutputIter iter);

	/**
	 * @brief Get the entities of this octree and of its children too.
	 * @tparam OutputIter The output iterator type.
	 * @param iter An output iterator.
	 */
	template <class OutputIter> 
	void get_entities(OutputIter iter) const;

	/**
	 * @brief Query an area to obtain all the entities in that area.
	 * @details This method is the raison d'etre for using an octree. The
	 * runtime complexity is \f$O(\log N)\f$, where \f$N\f$ is the total
	 * number of entities in the tree. This is the non-const version, so
	 * you'll get a container of mutable entities.
	 */
	template <class OutputIter>
	void query(const box3f& area, OutputIter iter);

	/**
	 * @brief Query an area to obtain all the entities in that area.
	 * @details This method is the raison d'etre for using an octree. The
	 * runtime complexity is \f$O(\log N)\f$, where \f$N\f$ is the total
	 * number of entities in the tree. This is the const version, so you'll 
	 * get a container of immutable entities.
	 */
	template <class OutputIter>
	void query(const box3f& area, OutputIter iter) const;

	/**
	 * @brief Apply a function to every entity.
	 * @tparam Func Type of a function pointer, lambda, functor, etc.
	 * @param f A function pointer, lambda, functor, etc.
	 */
	template <class Func> 
	void foreach(Func f);

	/**
	 * @brief Apply a function to every entity, const version.
	 * @tparam Func Type of a function pointer, lambda, functor, etc.
	 * @param f A function pointer, lambda, functor, etc.
	 */
	template <class Func>
	void foreach(Func f) const;

	/**
	 * @brief Insert an entity into the tree.
	 * @details This method recurses down into the tree if the entity can
	 * fit into a smaller box. It will also recursively create new nodes if
	 * needed.
	 * @param e The entity to insert.
	 */
	void insert(entity* e);

	/**
	 * @brief Erase an entity from the tree.
	 * @details This method does not recurse down into the tree. It will only
	 * attempt to erase the entity in this node's entity list. If no entities
	 * remain in the node's list and if this is a leaf node, then the node
	 * will delete itself.
	 * @param e The entity to erase.
	 * @return
	 * * 0 if the entity was not present.
	 * * 1 if succesfully removed, but the node was not removed.
	 * * 2 if succesfully removed, and the node was also removed.
	 */
	int erase(entity* e);

	// Notify the octree that an entity's global bounding box
	// has changed. This can result in possibly mutating the tree,
	// even changing the parent.
	
	/**
	 * @brief Notify this octree node that the entity has moved.
	 * @deprecated We probably want to use an event system for this. An
	 * entity right now has an event that fires when its global transform has
	 * changed so the octree should probably just subscribe to that event and
	 * do its thing via this route.
	 * @param e The entity whose global transform has changed.
	 */
	void notify(entity* e);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	octree(octree*, const box3f&);

	octree(octree* parent, const vec3f& min, const vec3f& max);

	void notify_helper(entity*);

	void subdivide();
};

template <class ForwardIter>
octree::octree(
	const box3f& bounds,
	ForwardIter first, ForwardIter last)
: m_bounds(bounds)
{
	m_child[0] = m_child[1] = m_child[2] 
		= m_child[3] = m_child[4] = m_child[5] 
		= m_child[6] = m_child[7] = nullptr;
	while (first != last)
	{
		this->insert(&(*first));
		++first;
	}
}

template <class ForwardIter>
octree::octree(
	const vec3f& min_corner, 
	const vec3f& max_corner, 
	ForwardIter first, 
	ForwardIter last)
: m_bounds(min_corner, max_corner)
{
	m_child[0] = m_child[1] = m_child[2] 
		= m_child[3] = m_child[4] = m_child[5] 
		= m_child[6] = m_child[7] = nullptr;
	while (first != last)
	{
		this->insert(&(*first));
		++first;
	}
}

// Get the entities of this octree and of its children too.
// This method is recursive.
template <class OutputIter> 
void octree::get_entities(OutputIter iter)
{
	std::copy(m_entities.begin(), m_entities.end(), iter);
	for (auto* c : m_child) if (c) c->get_entities(iter);
}

// Get the entities of this octree and of its children too.
// This method is recursive. This is the const version.
template <class OutputIter> 
void octree::get_entities(OutputIter iter) const
{
	std::copy(m_entities.begin(), m_entities.end(), iter);
	for (const auto* c : m_child) if (c) c->get_entities(iter);
}

// Query an area defined by a box3f to obtain all the entities
// in this area. This method is the raison d'etre for using
// a octree.
// The runtime complexity is O(log N), where N is the total
// number of entities.
// This is the non-const version. You'll get a container
// of mutable entities.
template <class OutputIter>
void octree::query(const box3f& area, OutputIter iter)
{
	for (auto* object : m_entities)
	{
		if (intersects(area, object->global_bounding_box()))
		{
			++iter;
			*iter = object;
		}
	}
	for (auto* c : m_child)
	{
		// Case 0: If there is no child node, continue
		if (c == nullptr) continue;
		// Case 1: search area completely contained by sub-quad
		// if a node completely contains the query area, go down that
		// branch and skip the remaining nodes (break this loop)
		if (c->m_bounds.contains(area))
		{
			c->query(area, iter);
			break;
		}
		// Case 2: Sub-quad completely contained by search area 
		// if the query area completely contains a sub-quad,
		// just add all the contents of that quad and it's children 
		// to the result set. You need to continue the loop to test 
		// the other quads
		else if (area.contains(c->m_bounds))
		{
			c->get_entities(iter);
			continue;
		}
		// Case 3: search area intersects with sub-quad
		// traverse into this quad, continue the loop to search other
		// quads
		else if (intersects(area, c->m_bounds))
		{
			c->query(area, iter);
			continue;
		}
	}
}

// Query an area defined by a box3f to obtain all the entities
// in this area. This method is the raison d'etre for using
// a octree.
// The runtime complexity is O(log N), where N is the total
// number of entities.
// This is the const version.
template <class OutputIter>
void octree::query(const box3f& area, OutputIter iter) const
{
	for (const auto* object : m_entities)
	{
		if (intersects(area, object->global_bounding_box()))
		{
			++iter;
			*iter = object;
		}
	}
	for (const auto* c : m_child)
	{
		// Case 0: If there is no child node, continue.
		if (c == nullptr) continue;
		// Case 1: search area completely contained by sub-quad.
		// If a node completely contains the query area, go down that
		// branch and skip the remaining nodes (break this loop)
		if (c->m_bounds.contains(area))
		{
			c->query(area, iter);
			break;
		}
		// Case 2: Sub-quad completely contained by search area.
		// If the query area completely contains a sub-quad,
		// just add all the contents of that quad and its children 
		// to the result set. We need to continue the loop to test 
		// the other quads.
		else if (area.contains(c->m_bounds))
		{
			c->get_entities(iter);
			continue;
		}
		// Case 3: search area intersects with sub-quad.
		// Traverse into this quad, continue the loop to search other
		// quads.
		else if (intersects(area, c->m_bounds))
		{
			c->query(area, iter);
			continue;
		}
	}
}

template <class Func> 
void octree::foreach(Func f)
{
	for (auto* o : m_entities) f(o);
	for (auto* c : m_child) if (c) c->foreach(f);
}

template <class Func>
void octree::foreach(Func f) const
{
	for (const auto* o : m_entities) f(o);
	for (const auto* c : m_child) if (c) c->foreach(f);
}

} // namespace gintonic

#endif