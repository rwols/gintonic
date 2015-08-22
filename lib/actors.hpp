#ifndef gintonic_actors_hpp
#define gintonic_actors_hpp

#include <vector>
#include "mesh.hpp"
#include "SQT.hpp"
#include "allocator.hpp"
#include "box3f.hpp"

namespace GINTONIC_NAMESPACE_FBX
{
	class FbxNode;
	class FbxScene;
	class FbxMesh;
	class FbxLight;
}

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

	// SQT transform

	std::vector<SQT, allocator<SQT>> transforms;

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
	static_model_actor(GINTONIC_NAMESPACE_FBX::FbxScene*);

	virtual ~static_model_actor() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void draw_geometry();
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;

	std::vector
	<
		std::tuple
		<
			SQT,
			std::shared_ptr<mesh>, 
			material
		>
	> model;

	std::vector
	<
		std::tuple
		<
			SQT,
			std::shared_ptr<light>
		>
	> lights;
private:
	void traverse_graph(GINTONIC_NAMESPACE_FBX::FbxNode*);

	void process_mesh(GINTONIC_NAMESPACE_FBX::FbxMesh*, const SQT&);

	void process_light(GINTONIC_NAMESPACE_FBX::FbxLight*, const SQT&);

	void draw_geometry_instanced();
	void draw_geometry_non_instanced() const BOOST_NOEXCEPT_OR_NOTHROW;
};

class animated_model_actor : public actor
{
	animated_model_actor() = default;
	animated_model_actor(GINTONIC_NAMESPACE_FBX::FbxScene*);

	virtual ~animated_model_actor() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void draw_geometry();
	virtual void draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW;
};

} // namespace gintonic

#endif