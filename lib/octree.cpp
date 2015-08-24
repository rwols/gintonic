#include "octree.hpp"
#include "exception.hpp"
#include <sstream>

namespace gintonic {

// Constructors.
octree::octree(const box3f& bounds)
: m_bounds(bounds)
{
	m_child[0] = m_child[1] = m_child[2] 
		= m_child[3] = m_child[4] = m_child[5] 
		= m_child[6] = m_child[7] = nullptr;
}

octree::octree(const vec3f& min_corner, const vec3f& max_corner)
: m_bounds(min_corner, max_corner)
{
	m_child[0] = m_child[1] = m_child[2] 
		= m_child[3] = m_child[4] = m_child[5] 
		= m_child[6] = m_child[7] = nullptr;
}

octree::octree(const octree& other)
: m_bounds(other.m_bounds)
, m_entities(other.m_entities)
{
	if (other.m_child[0]) m_child[0] = new octree(*(other.m_child[0]));
	else m_child[0] = nullptr;
	if (other.m_child[1]) m_child[1] = new octree(*(other.m_child[1]));
	else m_child[1] = nullptr;
	if (other.m_child[2]) m_child[2] = new octree(*(other.m_child[2]));
	else m_child[2] = nullptr;
	if (other.m_child[3]) m_child[3] = new octree(*(other.m_child[3]));
	else m_child[3] = nullptr;
	if (other.m_child[4]) m_child[4] = new octree(*(other.m_child[4]));
	else m_child[4] = nullptr;
	if (other.m_child[5]) m_child[5] = new octree(*(other.m_child[5]));
	else m_child[5] = nullptr;
	if (other.m_child[6]) m_child[6] = new octree(*(other.m_child[6]));
	else m_child[6] = nullptr;
	if (other.m_child[7]) m_child[7] = new octree(*(other.m_child[7]));
	else m_child[7] = nullptr;
}

octree::octree(octree&& other)
: m_parent(other.m_parent)
, m_bounds(std::move(other.m_bounds))
, m_entities(std::move(other.m_entities))
{
	m_child[0] = other.m_child[0];
	m_child[1] = other.m_child[1];
	m_child[2] = other.m_child[2];
	m_child[3] = other.m_child[3];
	m_child[4] = other.m_child[4];
	m_child[5] = other.m_child[5];
	m_child[6] = other.m_child[6];
	m_child[7] = other.m_child[7];

	other.m_parent = nullptr;
	
	other.m_child[0] = other.m_child[1] = other.m_child[2] 
		= other.m_child[3] = other.m_child[4] = other.m_child[5] 
		= other.m_child[6] = other.m_child[7] = nullptr;
}

octree& octree::operator = (const octree& other)
{
	// Explicitly call the destructor.
	this->~octree();

	if (other.m_child[0]) m_child[0] = new octree(*(other.m_child[0]));
	else m_child[0] = nullptr;
	if (other.m_child[1]) m_child[1] = new octree(*(other.m_child[1]));
	else m_child[1] = nullptr;
	if (other.m_child[2]) m_child[2] = new octree(*(other.m_child[2]));
	else m_child[2] = nullptr;
	if (other.m_child[3]) m_child[3] = new octree(*(other.m_child[3]));
	else m_child[3] = nullptr;
	if (other.m_child[4]) m_child[4] = new octree(*(other.m_child[4]));
	else m_child[4] = nullptr;
	if (other.m_child[5]) m_child[5] = new octree(*(other.m_child[5]));
	else m_child[5] = nullptr;
	if (other.m_child[6]) m_child[6] = new octree(*(other.m_child[6]));
	else m_child[6] = nullptr;
	if (other.m_child[7]) m_child[7] = new octree(*(other.m_child[7]));
	else m_child[7] = nullptr;

	return *this;
}

octree& octree::operator = (octree&& other)
{
	// Explicitly call the destructor.
	this->~octree();

	m_child[0] = other.m_child[0];
	m_child[1] = other.m_child[1];
	m_child[2] = other.m_child[2];
	m_child[3] = other.m_child[3];
	m_child[4] = other.m_child[4];
	m_child[5] = other.m_child[5];
	m_child[6] = other.m_child[6];
	m_child[7] = other.m_child[7];

	m_parent = other.m_parent; // Move the parent.

	other.m_parent = nullptr;
	
	other.m_child[0] = other.m_child[1] = other.m_child[2] 
		= other.m_child[3] = other.m_child[4] = other.m_child[5] 
		= other.m_child[6] = other.m_child[7] = nullptr;

	return *this;
}

// Non-trivial destructor calls delete on all of its children.
octree::~octree()
{
	for (const auto* c : m_child) delete c;
}

bool octree::is_leaf() const BOOST_NOEXCEPT_OR_NOTHROW
{
	for (const auto* c : m_child) if (c) return false;
	return true;
}

std::size_t octree::count() const
{
	std::size_t result = m_entities.size();
	for (const auto* c : m_child) if (c) result += c->count();
	return result;
}

void octree::insert(entity* object)
{
	#ifndef NDEBUG
	// If the item is not contained in this quad,
	// we made a logical mistake somewhere
	if (!m_bounds.contains(object->global_bounding_box()))
	{
		std::stringstream ss;
		ss << "Entity with bounds " << object->global_bounding_box()
			<< " is not entirely contained within "
			<< m_bounds << ".";
		throw exception(ss.str());
	}
	#endif

	// If we are at a leaf node, subdivide the space.
	if (is_leaf()) subdivide();

	// If the object's bounds are contained within
	// one of the m_child's bounds, then insert the object
	// in that m_child node.
	for (auto* c : m_child)
	{
		if (c && c->m_bounds.contains(object->global_bounding_box()))
		{
			c->insert(object);
			return;
		}
	}

	// If we arrive here, then none of the m_child nodes
	// can contain the object. So we add it to this node.
	m_entities.push_back(object);

	// Give the entity a reference to this node.
	object->m_octree = this;
}

int octree::erase(entity* object)
{
	if (object->m_octree != this) return 0;
	for (auto iter = m_entities.begin(); iter != m_entities.end(); ++iter)
	{
		if (object == *iter)
		{
			m_entities.erase(iter);
			object->m_octree = nullptr;
			break;
		}
	}
	if (m_parent) // check if we need to delete ourself and our neighbors
	{
		int erase_myself = 0;
		for (const auto* c : m_parent->m_child)
		{
			erase_myself += c->has_no_entities() ? 1 : 0;
			erase_myself += c->is_leaf() ? 1 : 0;
		}
		if (erase_myself == (1 << (2+1)))
		{
			for (const auto* c : m_parent->m_child)
			{
				if (c != this) delete c;
			}
			for (auto*& c : m_parent->m_child) c = nullptr;
			delete this;
			return 2;
		}
	}
	return 1;
}

void octree::notify(entity* object)
{
	if (this != object->m_octree)
	{
		throw std::logic_error("Cannot call notify on a node which is not"
		" equal to the object's spatialtree property.");
	}
	// If the object moved but still fits entirely inside
	// the bounds of this quad, then we do nothing at all.
	// It's already part of the list of entities of this quad.
	if (m_bounds.contains(object->global_bounding_box()))
	{
		return;
	}
	// At this point, the object has moved and does not
	// fit anymore in the current quad's bounds. So we erase
	// this object from the current quad's entity list.

	#ifndef NDEBUG
	bool object_has_been_removed = false;
	#endif

	for (auto i = m_entities.begin(); i != m_entities.end(); ++i)
	{
		if (*i == object)
		{
			m_entities.erase(i);

			#ifndef NDEBUG
			object_has_been_removed = true;
			#endif

			break;
		}
	}

	#ifndef NDEBUG
	if (!object_has_been_removed) throw std::logic_error("Object was not present in this quad.");
	#endif

	// If the quad has a parent, we let the parent handle
	// the moved object.
	if (m_parent)
	{
		m_parent->notify_helper(object);
	}
	// If the uad has no parent, then this quad is the root node.
	// But the object doesn't fit anymore; i.e. it doesn't fit
	// in the octree at all.
	// We handle this by just adding this object to the root node.
	else
	{
		object->m_octree = this;
		m_entities.push_back(object);
	}
}

void octree::notify_helper(entity* object)
{
	if (m_bounds.contains(object->global_bounding_box()))
	{
		insert(object);
	}
	else if (m_parent)
	{
		m_parent->notify_helper(object);
	}
	else
	{
		// This node is the root node, but the object
		// still does not fit.
		// We handle this by just adding this object to the root node.
		object->m_octree = this;
		m_entities.push_back(object);
	}
}

octree::octree(octree* parent, const box3f& bounds)
: m_parent(parent)
, m_bounds(bounds)
{
	m_child[0] = m_child[1] = m_child[2] 
		= m_child[3] = m_child[4] = m_child[5] 
		= m_child[6] = m_child[7] = nullptr;
}

octree::octree(octree* parent, const vec3f& min, const vec3f& max)
: m_parent(parent)
, m_bounds(min, max)
{
	m_child[0] = m_child[1] = m_child[2] 
		= m_child[3] = m_child[4] = m_child[5] 
		= m_child[6] = m_child[7] = nullptr;
}

void octree::subdivide()
{
	auto min = m_bounds.min_corner;
	auto max = m_bounds.max_corner;
	const auto half = (max - min) / 2.0f;
	if (half.x <= subdivision_threshold 
		|| half.y <= subdivision_threshold 
		|| half.z <= subdivision_threshold)
	{
		return;
	}
	m_child[0] = new octree(this, min, min + half);
	min.x += half.x;
	m_child[1] = new octree(this, min, min + half);
	min.y += half.y;
	m_child[2] = new octree(this, min, min + half);
	min.x -= half.x;
	m_child[3] = new octree(this, min, min + half);
	min.y -= half.y;
	// at this point, we are at the original location of min
	min.z += half.z;
	m_child[4] = new octree(this, min, min + half);
	min.x += half.x;
	m_child[5] = new octree(this, min, min + half);
	min.y += half.y;
	m_child[6] = new octree(this, min, min + half);
	min.x -= half.x;
	m_child[7] = new octree(this, min, min + half);
}

} // namespace gintonic