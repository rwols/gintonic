#ifndef gintonic_actors_hpp
#define gintonic_actors_hpp

#include "mesh.hpp"
#include <vector>

namespace gintonic {

class light;
class material;
class geometry_pass_shader;
class directional_light_pass_shader;
class point_light_pass_shader;

namespace opengl { class shader; }

class actor : std::enable_shared_from_this<actor>
{
public:
	virtual void think();

	virtual void draw_geometry() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;

	sqt_transformf transform;

	virtual ~actor() BOOST_NOEXCEPT_OR_NOTHROW = default;

protected:
	actor() = default;
};

class static_model_actor : public actor
{
public:
	static_model_actor() = default;
	static_model_actor(FbxScene*);

	virtual ~static_model_actor() BOOST_NOEXCEPT_OR_NOTHROW = default;

	virtual void draw_geometry() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;

	std::vector
	<
		std::tuple
		<
			sqt_transformf,
			std::shared_ptr<mesh>, 
			std::shared_ptr<material>
		>
	> model;

	std::vector
	<
		std::tuple
		<
			sqt_transformf,
			std::shared_ptr<light>
		>
	> lights;
private:
	void traverse_graph(FbxNode*);

	void process_mesh(FbxMesh*, const sqt_transformf&);

	void process_light(FbxLight*, const sqt_transformf&);
};

} // namespace gintonic

#endif