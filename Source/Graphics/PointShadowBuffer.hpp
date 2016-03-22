#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class PointShadowBuffer : public ShadowBuffer
{
public:
	PointShadowBuffer();
	virtual ~PointShadowBuffer() noexcept = default;

	virtual void collect(
		Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept;

	virtual void bindDepthTextures() const noexcept;

private:
	OpenGL::Framebuffer mFramebuffer[6];
	OpenGL::TextureObject mTexture[6];
};

} // namespace gintonic 