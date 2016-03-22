#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class SpotShadowBuffer : public ShadowBuffer
{
public:
	SpotShadowBuffer();
	virtual ~SpotShadowBuffer() noexcept = default;

	virtual void collect(
		Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept;

	virtual void bindDepthTextures() const noexcept;
	
private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
};

} // namespace gintonic