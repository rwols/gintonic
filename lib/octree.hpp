#ifndef gintonic_octree_hpp
#define gintonic_octree_hpp

#include "box3f.hpp"
#include "entity.hpp"
#include <list>

namespace gintonic {

class octree
{
private:

	friend class entity;

	octree* m_parent = nullptr;
	box3f m_bounds;
	std::list<entity*> m_entities;
	octree* m_child[8];

public:

	// The subdivision threshold is the minimum of the width and height
	// of a bounding box such that no more subdivision takes place.
	// After the octree has reached this point, all objects will just
	// be placed in this octree.
	float subdivision_threshold = 5.0f;

	// Constructor that takes a bounding box.
	octree(const box3f&);

	// Constructor that takes a minimum corner and a maximum corner
	// for the bounding box.
	octree(const vec3f& min_corner, const vec3f& max_corner);
	
	// Constructor that inserts elements at construction time.
	template <class ForwardIter> 
	octree(
		const box3f&, 
		ForwardIter first, 
		ForwardIter last);

	// Constructor that inserts elements at construction time.
	template <class ForwardIter> 
	octree(
		const vec3f& min_corner, 
		const vec3f& max_corner, 
		ForwardIter first, 
		ForwardIter last);

	// Copy constructor. This copies all of the children too.
	// However, it does not copy the parent. The parent of
	// the new copy will be a null pointer.
	octree(const octree&);

	// Move constructor. Will also move the parent!
	octree(octree&&);

	// Assignment operators. See copy and move constructors above.
	octree& operator = (const octree&);
	octree& operator = (octree&&);

	// Non-trivial destructor calls delete on all of its children.
	~octree();

	// Get the bounding box for this octree.
	inline const box3f& bounds() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_bounds;
	}

	// A octree is a root if it has no parent.
	inline bool is_root() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !m_parent;
	}

	// A octree is a leaf if it has no child octrees.
	bool is_leaf() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Check wether this octree has any entities.
	inline bool has_no_entities() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_entities.empty();
	}

	// Count the number of entities.
	// This method is recursive; it will accumulate
	// the entity count of its children too.
	// So cache this result if you are using it in a loop.
	std::size_t count() const;

	// Get the entities of this octree and of its children too.
	// This method is recursive.
	template <class OutputIter> 
	void get_entities(OutputIter iter);

	// Get the entities of this octree and of its children too.
	// This method is recursive.
	// This is the const version.
	template <class OutputIter> 
	void get_entities(OutputIter iter) const;

	// Query an area defined by a box3f to obtain all the entities
	// in this area. This method is the raison d'etre for using
	// a octree.
	// The runtime complexity is O(log N), where N is the total
	// number of entities.
	// This is the non-const version. You'll get a container
	// of mutable entities.
	template <class OutputIter>
	void query(const box3f& area, OutputIter iter);

	// Query an area defined by a box3f to obtain all the entities
	// in this area. This method is the raison d'etre for using
	// a octree.
	// The runtime complexity is O(log N), where N is the total
	// number of entities.
	// This is the const version.
	template <class OutputIter>
	void query(const box3f& area, OutputIter iter) const;

	template <class Func> 
	void foreach(Func f);

	template <class Func>
	void foreach(Func f) const;

	// Insert an entity. Goes recursively down into the tree.
	void insert(entity*);

	// returns 0 if was the entity is not present.
	// returns 1 if succesfully removed, but node was not removed.
	// returns 2 if succesfully removed, and node was also removed.
	int erase(entity*);

	// Notify the octree that an entity's global bounding box
	// has changed. This can result in possibly mutating the tree,
	// even changing the parent.
	void notify(entity*);

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

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