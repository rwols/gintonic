/**
 * @file gintonic.hpp
 * @brief The mother-of-all includes. Also defines the mother-of-all init
 * functions to initialize every subsystem.
 * @author Raoul Wols
 */

#ifndef gintonic_hpp
#define gintonic_hpp

#include "renderer.hpp"
#include "entity.hpp"
#include "octree.hpp"
#include "materials.hpp"
#include "static_mesh.hpp"
#include "shaders.hpp"
#include "fonts.hpp"
#include "basic_shapes.hpp"
#include "vertices.hpp"
#include "lights.hpp"
#include "skybox.hpp"
#include "scancodes.hpp"
#include "camera.hpp"
#include "proj_info.hpp"
#include "exception.hpp"
#include "luabindings.hpp"

namespace gintonic {

/**
 * @brief Initialize all systems of the engine.
 * 
 * @param window_title The title of the window.
 * @param camera The entity to view the world through.
 */
void init_all(const char* window_title, entity& camera);

} // namespace gintonic

#endif