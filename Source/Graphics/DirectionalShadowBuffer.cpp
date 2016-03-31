#include "DirectionalShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

#include "../Math/vec4f.hpp"
#include "../Math/mat4f.hpp"

#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"
#include "../Camera.hpp"

#include <iomanip>

namespace gintonic {

DirectionalShadowBuffer::DirectionalShadowBuffer()
{
	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 
		SHADOW_QUALITY, SHADOW_QUALITY, 0, 
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	mFramebuffer.checkStatus();
}

void DirectionalShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	updateProjectionMatrix(lightEntity);

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glViewport(0, 0, SHADOW_QUALITY, SHADOW_QUALITY);
	glClear(GL_DEPTH_BUFFER_BIT);

	const auto lProjectionViewMatrix = mProjectionMatrix * lightEntity.getViewMatrix();
	const auto& lProgram = ShadowShaderProgram::get();
	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		lProgram.setMatrixPVM(lProjectionViewMatrix * lGeometryEntity->globalTransform());
		lGeometryEntity->mesh->draw();
	}
}

void DirectionalShadowBuffer::bindDepthTextures() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, DEPTH_TEXTURE_UNIT);
}

const mat4f& DirectionalShadowBuffer::projectionMatrix() const noexcept
{
	return mProjectionMatrix;
}

void DirectionalShadowBuffer::updateProjectionMatrix(const Entity& lightEntity) noexcept
{
	const auto lNearPlane   = Renderer::getCameraEntity()->camera->nearPlane();
	const auto lFarPlane    = Renderer::getCameraEntity()->camera->farPlane();
	const auto lAspectRatio = 1.0f / Renderer::aspectRatio();
	const auto lTan         = std::tan(0.5f * Renderer::getCameraEntity()->camera->fieldOfView());
	const auto lNearTan     = lNearPlane * lTan;
	const auto lFarTan      = lFarPlane  * lTan;

	// Right now this frustum is defined in the coordinates of the VIEW space.
	// The coordinates are found with simple trigonometry.
	vec4f lCameraFrustum[8] =
	{
		// NOTE: The minus sign in front of the lNearPlane and lFarPlane
		// values is because OpenGL is a left-handed coordinate system
		// while we are a right-handed coordinate system. DOH!
		{-lNearTan, -lNearTan * lAspectRatio, -lNearPlane, 1.0f},
		{ lNearTan, -lNearTan * lAspectRatio, -lNearPlane, 1.0f},
		{ lNearTan,  lNearTan * lAspectRatio, -lNearPlane, 1.0f},
		{-lNearTan,  lNearTan * lAspectRatio, -lNearPlane, 1.0f},
		{-lFarTan,  -lFarTan  * lAspectRatio, -lFarPlane,  1.0f},
		{ lFarTan,  -lFarTan  * lAspectRatio, -lFarPlane,  1.0f},
		{ lFarTan,   lFarTan  * lAspectRatio, -lFarPlane,  1.0f},
		{-lFarTan,   lFarTan  * lAspectRatio, -lFarPlane,  1.0f},
	};

	// TODO: Not all camera's are a perspective type camera.
	// It could also be an orthographic projection. We do not
	// account for this at the moment.

	// Matrix that goes from VIEW space to LIGHT space
	const auto lFromViewToLightSpace = lightEntity.getViewMatrix() * Renderer::getCameraEntity()->globalTransform();

	// Transform the points from VIEW space to LIGHT space
	// In LIGHT space, the light direction is [0,0,-1].
	// Therefore a tight bounding box around the transformed camera frustum is simple to compute
	box3f lCameraFrustumBoundingBox
	(
		vec3f // minimum corner
		(
			std::numeric_limits<float>::max(), 
			std::numeric_limits<float>::max(), 
			std::numeric_limits<float>::max()
		), 
		vec3f // maximum corner
		(
			std::numeric_limits<float>::min(), 
			std::numeric_limits<float>::min(), 
			std::numeric_limits<float>::min()
		)
	);

	for (const auto& lFrustumPointInViewSpace : lCameraFrustum)
	{
		lCameraFrustumBoundingBox.addPoint(lFromViewToLightSpace * lFrustumPointInViewSpace);
	}

	mProjectionMatrix.set_orthographic(
		lCameraFrustumBoundingBox.minCorner.x,  // left
		lCameraFrustumBoundingBox.maxCorner.x,  // right
		lCameraFrustumBoundingBox.maxCorner.y,  // top
		lCameraFrustumBoundingBox.minCorner.y,  // bottom
		lCameraFrustumBoundingBox.minCorner.z,  // near
		lCameraFrustumBoundingBox.maxCorner.z); // far
}

} // namespace gintonic