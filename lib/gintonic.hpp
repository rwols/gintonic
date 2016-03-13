/**
 * @file gintonic.hpp
 * @brief The mother-of-all includes. Also defines the mother-of-all init
 * functions to initialize every subsystem.
 * @author Raoul Wols
 */

#ifndef gintonic_hpp
#define gintonic_hpp

#include "Foundation/scancodes.hpp"
#include "Foundation/exception.hpp"
#include "Foundation/octree.hpp"

#include "Graphics/renderer.hpp"
#include "Graphics/materials.hpp"
#include "Graphics/static_mesh.hpp"
#include "Graphics/shaders.hpp"
#include "Graphics/fonts.hpp"
#include "Graphics/basic_shapes.hpp"
#include "Graphics/vertices.hpp"
#include "Graphics/lights.hpp"
#include "Graphics/skybox.hpp"
 
#include "entity.hpp"
#include "camera.hpp"
#include "Graphics/proj_info.hpp"
#include "luabindings.hpp"

namespace gintonic {

/**
 * @brief Initialize all systems of the engine.
 * 
 * @param windowTitle The title of the window.
 * @param entityWithCamera The entity to view the world through.
 */
void initializeEverything(const char* windowTitle, std::shared_ptr<entity> entityWithCamera);

} // namespace gintonic

#endif