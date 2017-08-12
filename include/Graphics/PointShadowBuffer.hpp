#pragma once
#include "../Math/mat4f.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"
#include "ShadowBuffer.hpp"

namespace gintonic {

class GINTONIC_EXPORT PointShadowBuffer : public ShadowBuffer
{
  public:
    PointShadowBuffer();
    virtual ~PointShadowBuffer() noexcept = default;

    virtual void collect(const Entity& lightEntity,
                         const std::vector<std::shared_ptr<Entity>>&
                             shadowCastingGeometryEntities) noexcept;

    virtual void bindDepthTextures() const noexcept;

    inline virtual const mat4f& projectionMatrix() const noexcept
    {
        return mDummyMatrix;
    }

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    mat4f mDummyMatrix = 1.0f;
};

} // namespace gintonic
