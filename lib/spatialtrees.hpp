#ifndef gintonic_spatialtrees_hpp
#define gintonic_spatialtrees_hpp

#include "math.hpp"
#include "exception.hpp"
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/within.hpp>

namespace gintonic {

template <std::size_t N, typename T>
std::ostream& operator << (std::ostream& os, const box<T,N>& b)
{
	os << "((" << b.min_corner() << "), (" << b.max_corner() << "))";
	return os;
}

namespace detail {

template <typename T>
class has_bounds_method
{
	typedef char one;
	typedef long two;

	template <typename C> static one test(decltype(&C::bounds));
	template <typename C> static two test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

#define GINTONIC_SPATIALTREE_ERR "Template type T must have a method called "\
"bounds() that returns its bounding box in the correct dimension."

template <std::size_t N, class T, class Derived> 
class spatialtree
{
private:
	virtual void subdivide() = 0;
protected:

	Derived* m_parent = nullptr;
	box<float,N> m_bounds;
	std::list<T*> m_objects;
	Derived* m_child[1 << N];

	template <class U0, class U1>
	spatialtree(U0&& min_corner, U1&& max_corner)
	: m_bounds(std::forward<U0>(min_corner), std::forward<U1>(max_corner))
	{
		static_assert(has_bounds_method<T>::value, GINTONIC_SPATIALTREE_ERR);
	}

	template <class U0, class U1>
	spatialtree(Derived* parent, U0&& min_corner, U1&& max_corner)
	: m_parent(parent)
	, m_bounds(std::forward<U0>(min_corner), std::forward<U1>(max_corner))
	{
		static_assert(has_bounds_method<T>::value, GINTONIC_SPATIALTREE_ERR);
	}

public:

	float subdivision_threshold = 5.0f;

	inline bool is_root() const
	{
		return !m_parent;
	}

	bool is_leaf() const
	{
		for (const auto* c : m_child) if (c) return false;
		return true;
	}

	inline bool has_no_objects() const
	{
		return m_objects.empty();
	}

	std::size_t count() const
	{
		std::size_t result = m_objects.size();
		for (const auto* c : m_child) if (c) result += c->count();
		return result;
	}

	template <class OutputIter> 
	void get_content(OutputIter iter)
	{
		std::copy(m_objects.begin(), m_objects.end(), iter);
		for (auto* c : m_child) if (c) c->get_content(iter);
	}

	template <class OutputIter> 
	void get_content(OutputIter iter) const
	{
		std::copy(m_objects.begin(), m_objects.end(), iter);
		for (const auto* c : m_child) if (c) c->get_content(iter);
	}

	template <class OutputIter>
	void query(const box<float,N>& area, OutputIter iter)
	{
		for (auto* object : m_objects)
		{
			if (boost::geometry::intersects(area, object->bounds()))
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
			if (boost::geometry::within(area, c->m_bounds))
			{
				c->query(area, iter);
				break;
			}
			// Case 2: Sub-quad completely contained by search area 
			// if the query area completely contains a sub-quad,
			// just add all the contents of that quad and it's children 
			// to the result set. You need to continue the loop to test 
			// the other quads
			else if (boost::geometry::within(c->m_bounds, area))
			{
				c->get_content(iter);
				continue;
			}
			// Case 3: search area intersects with sub-quad
			// traverse into this quad, continue the loop to search other
			// quads
			else if (boost::geometry::intersects(area, c->m_bounds))
			{
				c->query(area, iter);
				continue;
			}
		}
	}

	template <class OutputIter>
	void query(const box<float,N>& area, OutputIter iter) const
	{
		for (const auto* object : m_objects)
		{
			if (boost::geometry::intersects(area, object->bounds()))
			{
				++iter;
				*iter = object;
			}
		}
		for (const auto* c : m_child)
		{
			// Case 0: If there is no child node, continue
			if (!c) continue;
			// Case 1: search area completely contained by sub-quad
			// if a node completely contains the query area, go down that
			// branch and skip the remaining nodes (break this loop)
			if (boost::geometry::within(area, c->m_bounds))
			{
				c->query(area, iter);
				break;
			}
			// Case 2: Sub-quad completely contained by search area 
			// if the query area completely contains a sub-quad,
			// just add all the contents of that quad and it's m_children 
			// to the result set. You need to continue the loop to test 
			// the other quads
			else if (boost::geometry::within(c->m_bounds, area))
			{
				c->get_content(iter);
				continue;
			}
			// Case 3: search area intersects with sub-quad
			// traverse into this quad, continue the loop to search other
			// quads
			else if (boost::geometry::intersects(area, c->m_bounds))
			{
				c->query(area, iter);
				continue;
			}
		}
	}

	template <class Func> 
	void foreach(Func f)
	{
		for (auto* o : m_objects) f(o);
		for (auto* c : m_child) if (c) c->foreach(f);
	}

	template <class Func>
	void foreach(Func f) const
	{
		for (const auto* o : m_objects) f(o);
		for (const auto* c : m_child) if (c) c->foreach(f);
	}

	virtual ~spatialtree()
	{
		for (const auto* c : m_child)
		{
			if (c)
			{
				delete c;
			}
		}
	}

	void insert(T* object)
	{
		// If the item is not contained in this quad,
		// we made a logical mistake somewhere
		if (!boost::geometry::within(object->bounds(), m_bounds))
		{
			std::stringstream ss;
			ss << "Object with bounds " << object->bounds() << " is not "
				<< "entirely within " << m_bounds << ".";
			throw exception(ss.str());
		}

		// If we are at a leaf node, subdivide the space.
		// This is the point where we need the polymorphism.
		if (is_leaf()) subdivide();

		// If the object's bounds are contained within
		// one of the m_child's bounds, then insert the object
		// in that m_child node.
		for (auto* c : m_child)
		{
			if (c && boost::geometry::within(object->bounds(), c->m_bounds))
			{
				c->insert(object);
				return;
			}
		}

		// If we arrive here, then none of the m_child nodes
		// can contain the object. So we add it to this node.
		m_objects.push_back(object);
		object->spatialtree = reinterpret_cast<Derived*>(this);
	}

	// returns 0 if object was not present
	// returns 1 if succesfully removed, but node was not removed
	// returns 2 if succesfully removed, and node was also removed
	int erase(T* object)
	{
		if (object->spatialtree != this) return 0;
		for (auto iter = m_objects.begin(); iter != m_objects.end(); ++iter)
		{
			if (object == *iter)
			{
				m_objects.erase(iter);
				object->spatialtree = nullptr;
				break;
			}
		}
		if (m_parent) // check if we need to delete ourself and our neighbors
		{
			int erase_myself = 0;
			for (const auto* c : m_parent->m_child)
			{
				erase_myself += c->has_no_objects() ? 1 : 0;
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

	void notify(T* object)
	{
		if (this != object->spatialtree)
		{
			throw std::logic_error("Cannot call notify on a node which is not"
			" equal to the object's spatialtree property.");
		}
		if (boost::geometry::within(object->bounds(), m_bounds))
		{
			// the object moved, but it still fits entirely
			// inside the bounds of this node.
			return;
		}
		else if (m_parent)
		{
			erase(object);
			m_parent->notify_helper(object);
		}
		else
		{
			// This node is the root node, but the object
			// still does not fit. We make a silent error.
			object->spatialtree = nullptr;
		}
	}

private:

	void notify_helper(T* object)
	{
		if (boost::geometry::within(object->bounds(), m_bounds))
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
			// still does not fit. We make a silent error.
			object->spatialtree = nullptr;
		}
	}
};

} // namespace detail

template <class T> 
class quadtree : public detail::spatialtree<2, T, quadtree<T>>
{
public:
	template <class U0, class U1>
	quadtree(U0&& min_corner, U1&& max_corner)
	: detail::spatialtree<2, T, quadtree<T>>(std::forward<U0>(min_corner), 
		std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] 
			= this->m_child[2] = this->m_child[3] = nullptr;
	}

	template <class U0, class U1, class ForwardIter>
	quadtree(U0&& min_corner, U1&& max_corner, ForwardIter first, 
		ForwardIter last)
	: detail::spatialtree<2, T, quadtree<T>>(std::forward<U0>(min_corner), 
		std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] 
			= this->m_child[2] = this->m_child[3] = nullptr;
		while (first != last)
		{
			this->insert(&(*first));
			++first;
		}
	}

	virtual ~quadtree() {}

private:
	template <class U0, class U1>
	quadtree(quadtree* parent, U0&& min_corner, U1&& max_corner)
	: detail::spatialtree<2, T, quadtree<T>>(parent, 
		std::forward<U0>(min_corner), std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] 
			= this->m_child[2] = this->m_child[3] = nullptr;
	}

	virtual void subdivide()
	{
		auto min = this->m_bounds.min_corner();
		auto max = this->m_bounds.max_corner();
		const auto half = (max - min) / 2.0f;
		if (half[0] <= this->subdivision_threshold 
			|| half[1] <= this->subdivision_threshold)
		{
			return;
		}
		this->m_child[0] = new quadtree(this, min, min + half);
		min[0] += half[0];
		this->m_child[1] = new quadtree(this, min, min + half);
		min[1] += half[1];
		this->m_child[2] = new quadtree(this, min, min + half);
		min[0] -= half[0];
		this->m_child[3] = new quadtree(this, min, min + half);
	}
};

template <class T>
class octree : public detail::spatialtree<3, T, octree<T>>
{
public:
	template <class U0, class U1>
	octree(U0&& min_corner, U1&& max_corner)
	: detail::spatialtree<3, T, octree<T>>(std::forward<U0>(min_corner), 
		std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] = this->m_child[2] 
			= this->m_child[3] = this->m_child[4] = this->m_child[5] 
			= this->m_child[6] = this->m_child[7] = nullptr;
	}

	template <class U0, class U1, class ForwardIter>
	octree(U0&& min_corner, U1&& max_corner, ForwardIter first, 
		ForwardIter last)
	: detail::spatialtree<3, T, octree<T>>(std::forward<U0>(min_corner), 
		std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] = this->m_child[2] 
			= this->m_child[3] = this->m_child[4] = this->m_child[5] 
			= this->m_child[6] = this->m_child[7] = nullptr;
		while (first != last)
		{
			this->insert(&(*first));
			++first;
		}
	}

	virtual ~octree() {}

private:

	template <class U0, class U1>
	octree(octree* parent, U0&& min_corner, U1&& max_corner)
	: detail::spatialtree<3, T, octree<T>>(parent, 
		std::forward<U0>(min_corner), std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] = this->m_child[2] 
			= this->m_child[3] = this->m_child[4] = this->m_child[5] 
			= this->m_child[6] = this->m_child[7] = nullptr;
	}

	virtual void subdivide()
	{
		auto min = this->m_bounds.min_corner();
		auto max = this->m_bounds.max_corner();
		const auto half = (max - min) / 2.0f;
		if (half[0] <= this->subdivision_threshold 
			|| half[1] <= this->subdivision_threshold 
			|| half[2] <= this->subdivision_threshold)
		{
			return;
		}
		this->m_child[0] = new octree(this, min, min + half);
		min[0] += half[0];
		this->m_child[1] = new octree(this, min, min + half);
		min[1] += half[1];
		this->m_child[2] = new octree(this, min, min + half);
		min[0] -= half[0];
		this->m_child[3] = new octree(this, min, min + half);
		min[1] -= half[1];
		// at this point, we are at the original location of min
		min[2] += half[2];
		this->m_child[4] = new octree(this, min, min + half);
		min[0] += half[0];
		this->m_child[5] = new octree(this, min, min + half);
		min[1] += half[1];
		this->m_child[6] = new octree(this, min, min + half);
		min[0] -= half[0];
		this->m_child[7] = new octree(this, min, min + half);
	}
};

} // namespace gintonic

#endif