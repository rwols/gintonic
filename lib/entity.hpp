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

class entity
{
public:

	typedef std::list<entity*> children_datastructure_type;
	typedef children_datastructure_type::iterator iterator;
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

	boost::signals2::signal<void(entity&)> about_to_die;
	boost::signals2::signal<void(entity&)> transform_changed;
	
	entity() = default;
	
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

	// These methods modify the entity's local SQT transform.
	// The entity's global SQT transform, the entity's global bounding box,
	// and recursively all of its children will be updated automatically.
	// If the entity is part of a spatial tree, the spatial tree will
	// be notified of the changed global bounding box, and will update
	// its datastructure accordingly.
	void set_scale(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void multiply_scale(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_translation(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_translation_x(const float) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_translation_y(const float) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_translation_z(const float) BOOST_NOEXCEPT_OR_NOTHROW;
	void add_translation(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_rotation(const quatf&) BOOST_NOEXCEPT_OR_NOTHROW;
	void post_multiply_rotation(const quatf&) BOOST_NOEXCEPT_OR_NOTHROW;
	void pre_multiply_rotation(const quatf&) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_local_transform(const SQT&) BOOST_NOEXCEPT_OR_NOTHROW;
	void post_add_local_transform(const SQT&) BOOST_NOEXCEPT_OR_NOTHROW;
	void pre_add_local_transform(const SQT&) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;

	void get_view_matrix(mat4f&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// These methods modify the local bounding box.
	// The global bounding box is updated automatically.
	// If the entity is part of a spatial tree, the spatial tree will
	// be notified of the changed global bounding box, and will update
	// its datastructure accordingly.
	void set_local_bounding_box(const box3f&);
	void set_local_bounding_box(const vec2f& min_corner, const vec2f& max_corner);

	// Basic add child, remove child and set parent methods.
	void add_child(entity&);
	void remove_child(entity&);
	void set_parent(entity&);
	void unset_parent();
	
	// Basic getters.
	inline const SQT&        local_transform()      const BOOST_NOEXCEPT_OR_NOTHROW { return m_local_transform; }
	inline const mat4f&      global_transform()     const BOOST_NOEXCEPT_OR_NOTHROW { return m_global_transform; }
	inline const box3f&      local_bounding_box()   const BOOST_NOEXCEPT_OR_NOTHROW { return m_local_bounding_box; }
	inline const box3f&      global_bounding_box()  const BOOST_NOEXCEPT_OR_NOTHROW { return m_global_bounding_box; }
	inline       entity*     parent()                     BOOST_NOEXCEPT_OR_NOTHROW { return m_parent; }
	inline const entity*     parent()               const BOOST_NOEXCEPT_OR_NOTHROW { return m_parent; }
	inline       controller* controller_component()       BOOST_NOEXCEPT_OR_NOTHROW { return m_controller_component; }
	inline const controller* controller_component() const BOOST_NOEXCEPT_OR_NOTHROW { return m_controller_component; }
	inline       mesh*       mesh_component()             BOOST_NOEXCEPT_OR_NOTHROW { return m_mesh_component; }
	inline const mesh*       mesh_component()       const BOOST_NOEXCEPT_OR_NOTHROW { return m_mesh_component; }
	inline       material*   material_component()         BOOST_NOEXCEPT_OR_NOTHROW { return m_material_component; }
	inline const material*   material_component()   const BOOST_NOEXCEPT_OR_NOTHROW { return m_material_component; }
	inline       light*      light_component()            BOOST_NOEXCEPT_OR_NOTHROW { return m_light_component; }
	inline const light*      light_component()      const BOOST_NOEXCEPT_OR_NOTHROW { return m_light_component; }
	inline       rigid_body* rigid_body_component()       BOOST_NOEXCEPT_OR_NOTHROW { return m_rigid_body_component; }
	inline const rigid_body* rigid_body_component() const BOOST_NOEXCEPT_OR_NOTHROW { return m_rigid_body_component; }
	inline       AI*         AI_component()               BOOST_NOEXCEPT_OR_NOTHROW { return m_AI_component; }
	inline const AI*         AI_component()         const BOOST_NOEXCEPT_OR_NOTHROW { return m_AI_component; }
	inline       camera*     camera_component()           BOOST_NOEXCEPT_OR_NOTHROW { return m_camera_component; }
	inline const camera*     camera_component()     const BOOST_NOEXCEPT_OR_NOTHROW { return m_camera_component; }
	inline       proj_info*  proj_info_component()        BOOST_NOEXCEPT_OR_NOTHROW { return m_proj_info_component; }
	inline const proj_info*  proj_info_component()  const BOOST_NOEXCEPT_OR_NOTHROW { return m_proj_info_component; }
	inline       iterator    begin()                      BOOST_NOEXCEPT_OR_NOTHROW { return m_children.begin(); }
	inline       iterator    end()                        BOOST_NOEXCEPT_OR_NOTHROW { return m_children.end(); }
	inline const_iterator    begin()                const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.begin(); }
	inline const_iterator    end()                  const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.end(); }
	inline const_iterator    cbegin()               const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.cbegin(); }
	inline const_iterator    cend()                 const BOOST_NOEXCEPT_OR_NOTHROW { return m_children.cend(); }
	inline       octree*     octree_node()                BOOST_NOEXCEPT_OR_NOTHROW { return m_octree; }
	inline const octree*     octree_node()          const BOOST_NOEXCEPT_OR_NOTHROW { return m_octree; }

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

	// An entity carries an SQT and a box3f, so we must
	// define custom new/delete operators.
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

} // namespace gintonic

#endif