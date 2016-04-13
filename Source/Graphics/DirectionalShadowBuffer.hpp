#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"
#include "../Math/mat4f.hpp"

namespace gintonic {

/**
 * @brief Shadow buffer for a directional light.
 */
class DirectionalShadowBuffer : public ShadowBuffer
{
public:

	/**
	 * @brief Constructor builds a depth buffer attached to a new OpenGL::Framebuffer.
	 * @throw gintonic::OpenGL::Framebuffer::Exception when the OpenGL::Framebuffer
	 * fails to initialize.
	 */
	DirectionalShadowBuffer();

	/// Default destructor does nothing special.
	virtual ~DirectionalShadowBuffer() noexcept = default;

	virtual void collect(
		const Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept;

	virtual void bindDepthTextures() const noexcept;

	virtual const mat4f& projectionMatrix() const noexcept;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	
private:
	
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
	mat4f mProjectionMatrix;

	void updateProjectionMatrix(const Entity& lightEntity) noexcept;
};

} // namespace gintonic