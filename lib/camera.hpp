#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "vec2f.hpp"
#include "vec3f.hpp"
#include "mat4f.hpp"

namespace gintonic {

class entity; // forward declaration.

class proj_info
{
public:

	proj_info() = default;

	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};

	projection_type projection = kPerspectiveProjection;

	float fieldofview = 1.22173048f;

	mat4f matrix;

	void update() BOOST_NOEXCEPT_OR_NOTHROW;
};

class camera
{
private:

	entity* m_controlling_entity;

public:

	camera(entity& entity_to_control);

	vec2f angles;

	inline entity& controlling_entity() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return *m_controlling_entity;
	}

	inline const entity&  controlling_entity() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return *m_controlling_entity;
	}

	void set_controlling_entity(entity&);

	void add_mouse(const vec2f& angles) BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif