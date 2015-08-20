#ifndef gintonic_quadtree_hpp
#define gintonic_quadtree_hpp

#include "box2f.hpp"
#include "exception.hpp"

Nu worden alle mogelijke materials, shaders en welke shader bij welke material hoort gehardcode in shaders.cpp. Dit is super inflexible en vooral veelste veel duplicate code(al veel inconsistente naamgeving gevonden hierdoor)

Mee eens :-)
De properties die een material altijd heeft (in dit model) zijn

* diffuse_color
* specular_color

Dit zijn simpele vectortjes, dus niet erg dat iedere material ze altijd draagt. De optionele properties zijn:

* diffuse_texture
* specular_texture
* normal_texture

Er zijn nog wat oude materials & bijbehorende shaders die niet meer in dit model passen, die heb ik in de laatste commit eruit gehaald als het goed is...

Dus wat we willen is een functie:

Set_van_mogelijke_eigenschappen -> shadermethodes.

Ik snap niet zo goed wat een "eigenschap" hier is, en wat een "shadermethode" hier is. Zijn dit klassen? :-)
De standard library van c++ bevat een std::map, het c# equivalent zou een System.Collections.SortedDictionary zijn.
Hoe werkt registratie van nieuwe eigenschappen?

Als dan elke material een member heeft die een lijst van eigenschappen is, dan hebben alleen nog een 'fold' functie nodig die over deze lijst heen loopt en alle shader methodes aan elkaar rijgt om de perfecte shader op te leveren specifiek voor deze material.

Ik weet niet wat een fold functie is, kun je dat uitleggen? Ik heb er wel een idee bij.
Het creeeren van een shader "on the fly" is wel iets wat ooit nodig gaat worden.
Maar zoiets implementeren is niet geheel triviaal.
Ik denk aan een klasse waar je wat properties "aan zet", die dan vervolgens een material shader geeft.
Zoiets als dit?

material_shader_builder builder;

// Eerste argument is wat de naam wordt in de shader,
// tweede argument is welke color buffer de property beinvloedt.
// Is maar een idee.
builder.add_material_property("diffuse_color", GBUFFER_DIFFUSE);
builder.add_material_property("diffuse_texture", GBUFFER_DIFFUSE);
builder.add_material_property("normal_texture", GBUFFER_NORMAL);

// Hoe moet de shader builder nu weten wat ie met de matrix_PVM moet doen?
// Zijn sommige uniform variables altijd standaard aanwezig?
builder.add_uniform("matrix_PVM");

// Deze method compiled ofwel een nieuwe shader, ofwel
// geeft er eentje van een centrale repo die er al op lijkt.
// Dus je gaat ook reference counting nodig hebben, mocht
// je dit pad betreden.
auto* my_cool_ptr = builder.get_shader();

// Nog een probleem: hoe kan ik nu bij de uniform locations
// van de properties die ik heb gemaakt?

Is de volgorde van belang denk je?

Nee, zolang alle geometry maar gerenderd wordt voordat de lichten worden gerenderd.

Ik ben het wel met je eens dat shaders.cpp een grote clusterfuck wordt als er niet iets aan wordt gedaan, alleen ik ben nog niet helemaal overtuigd van je verhaal.

namespace gintonic {

#define GINTONIC_SPATIALTREE_ERR "Template type T must have a method called "\
"bounds() that returns its bounding box in the correct dimension."

class quadtree
{
private:

	quadtree* m_parent = nullptr;

	box2f m_bounds;

	std::list<actor*> m_actors;

	quadtree* m_child[4];

public:

	template <class U0, class U1>
	quadtree(U0&& min_corner, U1&& max_corner)
	: m_bounds(std::forward<U0>(min_corner), std::forward<U1>(max_corner))
	{
		m_child[0] = m_child[1] = m_child[2] = m_child[3] = nullptr;
	}

	template <class U0, class U1, class ForwardIter>
	quadtree(U0&& min_corner, U1&& max_corner, ForwardIter first, ForwardIter last)
	: m_bounds(std::forward<U0>(min_corner), std::forward<U1>(max_corner))
	{
		m_child[0] = m_child[1] = m_child[2] = m_child[3] = nullptr;
		while (first != last)
		{
			this->insert(&(*first));
			++first;
		}
	}

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

	inline bool has_no_actors() const
	{
		return m_actors.empty();
	}

	std::size_t count() const
	{
		std::size_t result = m_actors.size();
		for (const auto* c : m_child) if (c) result += c->count();
		return result;
	}

	template <class OutputIter> 
	void get_actors(OutputIter iter)
	{
		std::copy(m_actors.begin(), m_actors.end(), iter);
		for (auto* c : m_child) if (c) c->get_actors(iter);
	}

	template <class OutputIter> 
	void get_actors(OutputIter iter) const
	{
		std::copy(m_actors.begin(), m_actors.end(), iter);
		for (const auto* c : m_child) if (c) c->get_actors(iter);
	}

	template <class OutputIter>
	void query(const box2f& area, OutputIter iter)
	{
		for (auto* object : m_actors)
		{
			if (area.intersects(object->bounds()))
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
				c->get_actors(iter);
				continue;
			}
			// Case 3: search area intersects with sub-quad
			// traverse into this quad, continue the loop to search other
			// quads
			else if (area.intersects(c->m_bounds))
			{
				c->query(area, iter);
				continue;
			}
		}
	}

	template <class OutputIter>
	void query(const box2f& area, OutputIter iter)
	{
		for (auto* object : m_actors)
		{
			if (area.intersects(object->bounds()))
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
				c->get_actors(iter);
				continue;
			}
			// Case 3: search area intersects with sub-quad
			// traverse into this quad, continue the loop to search other
			// quads
			else if (area.intersects(c->m_bounds))
			{
				c->query(area, iter);
				continue;
			}
		}
	}

	template <class Func> 
	void foreach(Func f)
	{
		for (auto* o : m_actors) f(o);
		for (auto* c : m_child) if (c) c->foreach(f);
	}

	template <class Func>
	void foreach(Func f) const
	{
		for (const auto* o : m_actors) f(o);
		for (const auto* c : m_child) if (c) c->foreach(f);
	}

	// non-trivial destructor deletes all of its children.
	~quadtree();

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

	void insert(actor* object)
	{
		// If the item is not contained in this quad,
		// we made a logical mistake somewhere
		if (!m_bounds.contains(object->bounds()))
		{
			std::stringstream ss;
			ss << "Object with bounds " << object->bounds() << " is not "
				<< "entirely within " << m_bounds << ".";
			throw exception(ss.str());
		}

		// If we are at a leaf node, subdivide the space.
		if (is_leaf()) subdivide();

		// If the object's bounds are contained within
		// one of the m_child's bounds, then insert the object
		// in that m_child node.
		for (auto* c : m_child)
		{
			if (c && c->m_bounds.contains(object->bounds()))
			{
				c->insert(object);
				return;
			}
		}

		// If we arrive here, then none of the m_child nodes
		// can contain the object. So we add it to this node.
		m_actors.push_back(object);

		// Give the actor a reference to this node.
		object->spatialtree = this;
	}

	// returns 0 if object was not present
	// returns 1 if succesfully removed, but node was not removed
	// returns 2 if succesfully removed, and node was also removed
	int erase(actor* object)
	{
		if (object->spatialtree != this) return 0;
		for (auto iter = m_actors.begin(); iter != m_actors.end(); ++iter)
		{
			if (object == *iter)
			{
				m_actors.erase(iter);
				object->spatialtree = nullptr;
				break;
			}
		}
		if (m_parent) // check if we need to delete ourself and our neighbors
		{
			int erase_myself = 0;
			for (const auto* c : m_parent->m_child)
			{
				erase_myself += c->has_no_actors() ? 1 : 0;
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
		if (m_bounds.contains(object->bounds()))
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
class quadtree : public detail::spatialtree<3, T, quadtree<T>>
{
public:
	template <class U0, class U1>
	quadtree(U0&& min_corner, U1&& max_corner)
	: detail::spatialtree<3, T, quadtree<T>>(std::forward<U0>(min_corner), 
		std::forward<U1>(max_corner))
	{
		this->m_child[0] = this->m_child[1] = this->m_child[2] 
			= this->m_child[3] = this->m_child[4] = this->m_child[5] 
			= this->m_child[6] = this->m_child[7] = nullptr;
	}

	template <class U0, class U1, class ForwardIter>
	quadtree(U0&& min_corner, U1&& max_corner, ForwardIter first, 
		ForwardIter last)
	: detail::spatialtree<3, T, quadtree<T>>(std::forward<U0>(min_corner), 
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

	virtual ~quadtree() {}

private:

	template <class U0, class U1>
	quadtree(quadtree* parent, U0&& min_corner, U1&& max_corner)
	: detail::spatialtree<3, T, quadtree<T>>(parent, 
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
		this->m_child[0] = new quadtree(this, min, min + half);
		min[0] += half[0];
		this->m_child[1] = new quadtree(this, min, min + half);
		min[1] += half[1];
		this->m_child[2] = new quadtree(this, min, min + half);
		min[0] -= half[0];
		this->m_child[3] = new quadtree(this, min, min + half);
		min[1] -= half[1];
		// at this point, we are at the original location of min
		min[2] += half[2];
		this->m_child[4] = new quadtree(this, min, min + half);
		min[0] += half[0];
		this->m_child[5] = new quadtree(this, min, min + half);
		min[1] += half[1];
		this->m_child[6] = new quadtree(this, min, min + half);
		min[0] -= half[0];
		this->m_child[7] = new quadtree(this, min, min + half);
	}
};

} // namespace gintonic

#endif