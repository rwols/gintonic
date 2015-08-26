#ifndef gintonic_hpp
#define gintonic_hpp

#include "renderer.hpp"
#include "actors.hpp"
#include "entity.hpp"
#include "octree.hpp"
#include "materials.hpp"
#include "mesh.hpp"
#include "shaders.hpp"
#include "fonts.hpp"
#include "basic_shapes.hpp"
#include "vertices.hpp"
#include "lights.hpp"
#include "skybox.hpp"
#include "scancodes.hpp"
#include "camera.hpp"

namespace gintonic {

// camera& get_default_camera() BOOST_NOEXCEPT_OR_NOTHROW;

// proj_info* get_default_proj_info() BOOST_NOEXCEPT_OR_NOTHROW;

// entity& get_root_entity() BOOST_NOEXCEPT_OR_NOTHROW;

// entity& get_default_camera_entity() BOOST_NOEXCEPT_OR_NOTHROW;

// octree& get_root_octree() BOOST_NOEXCEPT_OR_NOTHROW;

void init_all(const char* window_title, entity* camera);

} // namespace gintonic

#endif