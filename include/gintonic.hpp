/**
 * @file gintonic.hpp
 * @brief Includes everything.
 * @author Raoul Wols
 */

#pragma once

#include <gintonic/Camera.hpp>
#include <gintonic/Entity.hpp>
#include <gintonic/EntityVisitor.hpp>
#include <gintonic/FbxImporter.hpp>
#include <gintonic/Foundation/Octree.hpp>
#include <gintonic/Foundation/exception.hpp>
#include <gintonic/Foundation/scancodes.hpp>
#include <gintonic/Graphics/AmbientLight.hpp>
#include <gintonic/Graphics/AnimationClip.hpp>
#include <gintonic/Graphics/DirectionalLight.hpp>
#include <gintonic/Graphics/Font.hpp>
#include <gintonic/Graphics/Material.hpp>
#include <gintonic/Graphics/Mesh.hpp>
#include <gintonic/Graphics/PointLight.hpp>
#include <gintonic/Graphics/Renderer.hpp>
#include <gintonic/Graphics/ShaderPrograms.hpp>
#include <gintonic/Graphics/Skeleton.hpp>
#include <gintonic/Graphics/SpotLight.hpp>
#include <gintonic/Graphics/skybox.hpp>

// namespace gintonic
// {

// /**
//  * @brief Initialize all systems of the engine with a default camera.
//  *
//  * @param windowTitle The title of the window.
//  */
// void initializeEverything(const char* windowTitle);

// *
//  * @brief Initialize all systems of the engine.
//  *
//  * @param windowTitle The title of the window.
//  * @param entityWithCamera The entity to view the world through.

// void initializeEverything(const char* windowTitle,
//                           std::shared_ptr<Entity> entityWithCamera);

// } // namespace gintonic
