/**
 * @file gintonic.hpp
 * @brief The mother-of-all includes. Also defines the mother-of-all init
 * functions to initialize every subsystem.
 * @author Raoul Wols
 */

#pragma once

// #include "Foundation/scancodes.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "EntityVisitor.hpp"
#include "Foundation/Octree.hpp"
#include "Foundation/exception.hpp"
#include "Graphics/AmbientLight.hpp"
#include "Graphics/AnimationClip.hpp"
#include "Graphics/DirectionalLight.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/PointLight.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderPrograms.hpp"
#include "Graphics/Skeleton.hpp"
#include "Graphics/SpotLight.hpp"
#include "Graphics/skybox.hpp"

namespace gintonic
{

/**
 * @brief Initialize all systems of the engine with a default camera.
 *
 * @param windowTitle The title of the window.
 */
void initializeEverything(const char* windowTitle);

/**
 * @brief Initialize all systems of the engine.
 *
 * @param windowTitle The title of the window.
 * @param entityWithCamera The entity to view the world through.
 */
void initializeEverything(const char* windowTitle,
                          std::shared_ptr<Entity> entityWithCamera);

} // namespace gintonic
