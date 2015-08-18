#ifndef gintonic_actors_hpp
#define gintonic_actors_hpp

#include "mesh.hpp"
#include <vector>

namespace gintonic {

class light;
class material;
template <class T> class octree;

namespace opengl { class shader; }

class actor : std::enable_shared_from_this<actor>
{
public:
	virtual void think();

	virtual void draw_geometry();
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;

	// sqt_transformf transform;

	std::vector<sqt_transformf> transforms;

	virtual ~actor() BOOST_NOEXCEPT_OR_NOTHROW;

	const box3f& bounds() const BOOST_NOEXCEPT_OR_NOTHROW;

	octree<actor>* spatialtree = nullptr;

protected:
	actor() = default;
	box3f m_bounds;
};

class static_model_actor : public actor
{
public:
	static_model_actor() = default;
	static_model_actor(FbxScene*);

	virtual ~static_model_actor() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void draw_geometry();
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;

	std::vector
	<
		std::tuple
		<
			sqt_transformf,
			std::shared_ptr<mesh>, 
			material
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

	void draw_geometry_instanced();
	void draw_geometry_non_instanced() const BOOST_NOEXCEPT_OR_NOTHROW;
};

class animated_model_actor : public actor
{
	animated_model_actor() = default;
	animated_model_actor(FbxScene*);

	virtual ~animated_model_actor() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void draw_geometry();
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif