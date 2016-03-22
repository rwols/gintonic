#include "PointShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

#include "../Math/mat4f.hpp"

#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"
#include "../Camera.hpp"

namespace gintonic {

PointShadowBuffer::PointShadowBuffer()
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void PointShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	const auto& lShadowProgram = ShadowShaderProgram::get();
	mat4f lProjectionMatrix, lViewMatrix, lModelMatrix;
	for (int i = 0; i < 6; ++i)
	{
		mFramebuffer[i].bind(GL_DRAW_FRAMEBUFFER);

		lProjectionMatrix = lightEntity.camera->projectionMatrix();

		lightEntity.postMultiplyRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));

		lightEntity.getViewMatrix(lViewMatrix);

		for (const auto lGeometryEntity : shadowCastingGeometryEntities)
		{
			lModelMatrix = lGeometryEntity->globalTransform();
			lShadowProgram.setMatrixPVM(lProjectionMatrix * lViewMatrix * lModelMatrix);
			lGeometryEntity->mesh->draw();
		}
	}
}

void PointShadowBuffer::bindDepthTextures() const noexcept
{
	for (int i = 0; i < 6; ++i)
	{
		mTexture[i].bind(GL_TEXTURE_2D, i);
	}
}

} // namespace gintonic