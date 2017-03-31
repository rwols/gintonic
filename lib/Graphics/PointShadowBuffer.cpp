#include <gintonic/Component/Camera.hpp>
#include <gintonic/Entity.hpp>
#include <gintonic/Foundation/exception.hpp>
#include <gintonic/Graphics/Mesh.hpp>
#include <gintonic/Graphics/PointShadowBuffer.hpp>
#include <gintonic/Graphics/Renderer.hpp>
#include <gintonic/Graphics/ShaderPrograms.hpp>
#include <gintonic/Math/mat4f.hpp>

using namespace gintonic;

PointShadowBuffer::PointShadowBuffer() { /* Empty on purpose. */}

void PointShadowBuffer::collect(
    const Entity& /*lightEntity*/,
    const std::vector<
        std::shared_ptr<Entity>>& /*shadowCastingGeometryEntities*/) noexcept
{
    /* Empty on purpose. */
}

void PointShadowBuffer::bindDepthTextures() const noexcept
{
    /* Empty on purpose. */
}
