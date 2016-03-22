#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class DirectionalShadowBuffer : public ShadowBuffer
{
public:
	DirectionalShadowBuffer();
	virtual ~DirectionalShadowBuffer() noexcept = default;

	virtual void collect(
		Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept;

	virtual void bindDepthTextures() const noexcept;
	
private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
};

} // namespace gintonic