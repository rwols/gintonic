/**
 * @file gintonic.hpp
 * @brief The mother-of-all includes. Also defines the mother-of-all init
 * functions to initialize every subsystem.
 * @author Raoul Wols
 */

#pragma once

#include "Foundation/scancodes.hpp"
#include "Foundation/exception.hpp"
#include "Foundation/Octree.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/static_mesh.hpp"
#include "Graphics/shaders.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/basic_shapes.hpp"
#include "Graphics/vertices.hpp"
#include "Graphics/AmbientLight.hpp"
#include "Graphics/DirectionalLight.hpp"
#include "Graphics/PointLight.hpp"
#include "Graphics/SpotLight.hpp"
#include "Graphics/skybox.hpp"
 
#include "Entity.hpp"
#include "Camera.hpp"
#include "FbxImporter.hpp"
#include "luabindings.hpp"

namespace gintonic {

/**
 * @brief Initialize all systems of the engine.
 * 
 * @param windowTitle The title of the window.
 * @param entityWithCamera The entity to view the world through.
 */
void initializeEverything(const char* windowTitle, std::shared_ptr<Entity> entityWithCamera);

} // namespace gintonic
