#pragma once
#include "ShadowBuffer.hpp"
#include <gintonic/Graphics/OpenGL/Framebuffer.hpp>
#include <gintonic/Graphics/OpenGL/TextureObject.hpp>
#include <gintonic/Math/mat4f.hpp>

namespace gintonic
{

class SpotShadowBuffer : public ShadowBuffer
{
  public:
    SpotShadowBuffer();
    virtual ~SpotShadowBuffer() noexcept = default;

    virtual void collect(const Entity& lightEntity,
                         const std::vector<std::shared_ptr<Entity>>&
                             shadowCastingGeometryEntities) noexcept;

    virtual void bindDepthTextures() const noexcept;

    inline virtual const mat4f& projectionMatrix() const noexcept
    {
        return mProjectionMatrix;
    }

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    OpenGL::Framebuffer mFramebuffer;
    OpenGL::TextureObject mTexture;
    mat4f mProjectionMatrix;
};

} // namespace gintonic
