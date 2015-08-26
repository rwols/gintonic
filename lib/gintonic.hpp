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

void init_all(const char* window_title, entity& camera);

} // namespace gintonic

#endif