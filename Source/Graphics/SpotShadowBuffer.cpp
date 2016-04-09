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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 
		SHADOW_QUALITY, SHADOW_QUALITY, 0, 
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
	mProjectionMatrix.set_perspective
	(
		2.0f * std::acos(1.0f - lightEntity.light->getCosineHalfAngle()), // Field of view
		1.0f,                                                             // Aspect ratio
		0.1f,                                                             // Near plane
		lightEntity.light->getCutoffRadius()                              // Far plane
	);
	const auto lProjectionViewMatrix = mProjectionMatrix * lightEntity.getViewMatrix();
	mat4f lProjectionViewModelMatrix;

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glViewport(0, 0, SHADOW_QUALITY, SHADOW_QUALITY);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	const auto& lProgram = ShadowShaderProgram::get();
	lProgram.activate();

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		lProjectionViewModelMatrix = lProjectionViewMatrix * lGeometryEntity->globalTransform();
		lProgram.setMatrixPVM(lProjectionViewModelMatrix);
		lGeometryEntity->mesh->draw();
	}
}

void SpotShadowBuffer::bindDepthTextures() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, DEPTH_TEXTURE_UNIT);
}

} // namespace gintonic