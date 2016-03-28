#include "SpotShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

#include "../Math/mat4f.hpp"

#include "Light.hpp"
#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"
#include "../Camera.hpp"

namespace gintonic {

SpotShadowBuffer::SpotShadowBuffer()
{
	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
		SHADOW_QUALITY * Renderer::width(), SHADOW_QUALITY * Renderer::height(), 0, 
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	mFramebuffer.checkStatus();
}

void SpotShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	mProjectionMatrix.set_perspective(3.0f, 1.0f, 0.1f, 8.0f);
	// mProjectionMatrix = Renderer::getCameraEntity()->camera->projectionMatrix();
	const auto lProjectionViewMatrix = mProjectionMatrix * lightEntity.getViewMatrix();
	mat4f lProjectionViewModelMatrix;

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glViewport(0, 0, SHADOW_QUALITY * Renderer::width(), SHADOW_QUALITY * Renderer::height());
	glClear(GL_DEPTH_BUFFER_BIT);

	Renderer::cerr() << "mProjectionMatrix:\n" << mProjectionMatrix << "\n\n";
	
	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		lProjectionViewModelMatrix = lProjectionViewMatrix * lGeometryEntity->globalTransform();
		ShadowShaderProgram::get().setMatrixPVM(lProjectionViewModelMatrix);
		lGeometryEntity->mesh->draw();
	}
}

void SpotShadowBuffer::bindDepthTextures() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, DEPTH_TEXTURE_UNIT);
}

} // namespace gintonic