#ifndef gintonic_entity_hpp
#define gintonic_entity_hpp

#include "SQT.hpp"
#include "box3f.hpp"
#include "allocator.hpp"
#include <list>

namespace gintonic {

class octree;     // Forward declaration.
class mesh;       // Forward declaration.
class material;   // Forward declaration.
class light;      // Forward declaration.
class rigid_body; // Forward declaration.
class logic;      // Forward declaration.
class AI;         // Forward declaration.
class proj_info;  // Forward declaration.
class SQTstack;   // Forward declaration.

class entity
{
public:

	typedef std::list<entity*> children_datastructure_type;
	typedef children_datastructure_type::iterator iterator;
	typedef children_datastructure_type::const_iterator const_iterator;

private:

	SQT m_local_transform;
	SQT m_global_transform;

	box3f m_local_bounding_box;
	box3f m_global_bounding_box;

	// Optional list of children of this entity.
	children_datastructure_type m_children;

	// Optional pointer to a parent entity.
	// The MODEL->WORLD transformation matrix
	// is influenced by the parent entity.
	entity* m_parent = nullptr;

	// Pointer to the octree.
	octree* m_octree = nullptr;

	friend class octree;

	void update_global_info(SQTstack&) BOOST_NOEXCEPT_OR_NOTHROW;
	void update_global_info_start() BOOST_NOEXCEPT_OR_NOTHROW;
	void update_global_transform_and_bounding_box(const SQTstack&) BOOST_NOEXCEPT_OR_NOTHROW;
	SQT compute_global_transform() const BOOST_NOEXCEPT_OR_NOTHROW;

public:

	// Optional components.
	// One can set these if so desired.
	mesh*       mesh_component       = nullptr;
	material*   material_component   = nullptr;
	light*      light_component      = nullptr;
	rigid_body* rigid_body_component = nullptr;
	logic*      logic_component      = nullptr;
	AI*         AI_component         = nullptr;
	proj_info*  proj_info_component  = nullptr;


	entity(
		const SQT&   local_transform,
		const box3f& local_bounding_box,
		octree&      octree_root,
		entity*      parent               = nullptr,
		mesh*        mesh_component       = nullptr,
		material*    material_component   = nullptr,
		light*       light_component      = nullptr,
		rigid_body*  rigid_body_component = nullptr,
		logic*       logic_component      = nullptr,
		AI*          AI_component         = nullptr,
		proj_info*   proj_info_component  = nullptr);

	entity(const entity&);
	entity(entity&&) BOOST_NOEXCEPT_OR_NOTHROW;
	entity& operator = (const entity&);
	entity& operator = (entity&&) BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Non-trivial destructor deletes all of the children of this entity.
	~entity();

	// These methods modify the entity's local SQT transform.
	// The entity's global SQT transform, the entity's global bounding box,
	// and recursively all of its children will be updated automatically.
	// If the entity is part of a spatial tree, the spatial tree will
	// be notified of the changed global bounding box, and will update
	// its datastructure accordingly.
	void set_scale(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void multiply_scale(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_translation(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void add_translation(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_rotation(const quatf&) BOOST_NOEXCEPT_OR_NOTHROW;
	void post_multiply_rotation(const quatf&) BOOST_NOEXCEPT_OR_NOTHROW;
	void pre_multiply_rotation(const quatf&) BOOST_NOEXCEPT_OR_NOTHROW;
	void look_at(const entity*) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_local_transform(const SQT&) BOOST_NOEXCEPT_OR_NOTHROW;
	void post_add_local_transform(const SQT&) BOOST_NOEXCEPT_OR_NOTHROW;
	void pre_add_local_transform(const SQT&) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void add_mousedelta(const vec2f& delta) BOOST_NOEXCEPT_OR_NOTHROW;

	// These methods modify the local bounding box.
	// The global bounding box is updated automatically.
	// If the entity is part of a spatial tree, the spatial tree will
	// be notified of the changed global bounding box, and will update
	// its datastructure accordingly.
	void set_local_bounding_box(const box3f&);
	void set_local_bounding_box(const vec2f& min_corner, const vec2f& max_corner);

	// Get a constant reference to the local SQT transform.
	inline const SQT& local_transform() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_local_transform;
	}

	// Get a constant reference to the global SQT transform.
	inline const SQT& global_transform() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_global_transform;
	}

	// Get a constant reference to the local bounding box.
	inline const box3f& local_bounding_box() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_local_bounding_box;
	}

	// Get a constant reference to the global bounding box.
	inline const box3f& global_bounding_box() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_global_bounding_box;
	}

	// Basic add child, remove child and set parent methods.
	void add_child(entity*);
	void remove_child(entity*);
	void set_parent(entity*);
	
	// Basic getters.
	inline       entity*  parent()            BOOST_NOEXCEPT_OR_NOTHROW { return m_parent; }
	inline const entity*  parent()      const BOOST_NOEXCEPT_OR_NOTHROW { return m_parent; }
	inline       iterator begin()             BOOST_NOEXCEPT_OR_NOTHROW { return m_children.begin(); }
	inline       iterator end()               BOOST_NOEXCEPT_OR_NOTHROW { return m_children.end(); }
	inline const_iterator begin()       const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.begin(); }
	inline const_iterator end()         const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.end(); }
	inline const_iterator cbegin()      const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.cbegin(); }
	inline const_iterator cend()        const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.cend(); }
	inline       octree*  octree_node()       BOOST_NOEXCEPT_OR_NOTHROW { return m_octree; }
	inline const octree*  octree_node() const BOOST_NOEXCEPT_OR_NOTHROW { return m_octree; }

	// Comparison operators.
	inline bool operator == (const entity& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		// Compare the locations in memory.
		return this == &other;
	}

	inline bool operator != (const entity& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	inline bool operator < (const entity& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this < &other;
	}

	inline bool operator >= (const entity& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator<(other);
	}

	inline bool operator > (const entity& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this > &other;
	}

	inline bool operator <= (const entity& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator>(other);
	}

	// Draw the mesh of this entity (if present)
	// as well as all of its children.
	void draw_geometry() const;

	// Draw the light of this entity (if present)
	// as well as all of its children.
	void draw_lights() const;

	// Draw the mesh of this entity (if present)
	// given the current SQTstack.
	void draw_geometry(SQTstack&) const;

	// Draw the light of this entity (if present)
	// given the current SQTstack.
	void draw_lights(SQTstack&) const;

	// Create a new entity.
	// Every entity is stored in a global container.
	// An entity has a private constructor, so this
	// static method is the only way to create new entitities.
	// static entity* create();

	// Clear all entities from the global container.
	// static void clear_all();

	// Destroy this entity.
	// Do not call any methods or access any member variables
	// of the entity after this method has been called.
	// void destroy();

	// An entity carries an SQT and a box3f, so we must
	// define custom new/delete operators.
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

// Compute the squared distance between two entitities.
inline float distance2(const entity* a, const entity* b) BOOST_NOEXCEPT_OR_NOTHROW
{
	return distance2(a->global_transform(), b->global_transform());
}

// Compute the distance between two entities.
inline float distance(const entity* a, const entity* b) BOOST_NOEXCEPT_OR_NOTHROW
{
	return distance(a->global_transform(), b->global_transform());
}

} // namespace gintonic

#endif