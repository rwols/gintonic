#include "DirectionalShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

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
	box3f lBoundingBox(vec3f(0.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 0.0f));

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		const auto lBBox = lGeometryEntity->globalBoundingBox();
		lBoundingBox.min_corner.x = std::min(lBoundingBox.min_corner.x, lBBox.min_corner.x);
		lBoundingBox.min_corner.y = std::min(lBoundingBox.min_corner.y, lBBox.min_corner.y);
		lBoundingBox.min_corner.z = std::min(lBoundingBox.min_corner.z, lBBox.min_corner.z);
		lBoundingBox.max_corner.x = std::max(lBoundingBox.max_corner.x, lBBox.max_corner.x);
		lBoundingBox.max_corner.y = std::max(lBoundingBox.max_corner.y, lBBox.max_corner.y);
		lBoundingBox.max_corner.z = std::max(lBoundingBox.max_corner.z, lBBox.max_corner.z);
	}

	const float lBias = 2.0f;

	mProjectionMatrix.set_orthographic(
		lBoundingBox.min_corner.x - lBias,  // left
		lBoundingBox.max_corner.x + lBias,  // right
		lBoundingBox.max_corner.y + lBias,  // top
		lBoundingBox.min_corner.y - lBias,  // bottom
		lBoundingBox.min_corner.z - lBias,  // near
		lBoundingBox.max_corner.z + lBias); // far

	const auto lProjectionViewMatrix = mProjectionMatrix * lightEntity.getViewMatrix();

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glViewport(0, 0, SHADOW_QUALITY, SHADOW_QUALITY);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		ShadowShaderProgram::get().setMatrixPVM(lProjectionViewMatrix * lGeometryEntity->globalTransform());
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

} // namespace gintonic