#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"
#include "../Math/mat4f.hpp"

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

	virtual const mat4f& projectionMatrix() const noexcept;

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond
	
private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
	mat4f mProjectionMatrix;
};

} // namespace gintonic