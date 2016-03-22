#include "DirectionalShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

#include "../Math/mat4f.hpp"

#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

namespace gintonic {

DirectionalShadowBuffer::DirectionalShadowBuffer()
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}


void DirectionalShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	const auto& lShadowProgram = ShadowShaderProgram::get();
	mat4f lViewMatrix, lProjectionMatrix, lModelMatrix;

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);

	lProjectionMatrix.set_orthographic(Renderer::width(), Renderer::height(), -100.0f, 100.0f);
	lightEntity.getViewMatrix(lViewMatrix);

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		lModelMatrix = lGeometryEntity->globalTransform();
		lShadowProgram.setMatrixPVM(lProjectionMatrix * lViewMatrix * lModelMatrix);
		lGeometryEntity->mesh->draw();
	}
}

void DirectionalShadowBuffer::bindDepthTextures() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, 0);
}

} // namespace gintonic