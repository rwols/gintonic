#include "DirectionalShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

#include "../Math/mat4f.hpp"

#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iomanip>

#define SHADOWBUFFER_QUALITY 512

namespace gintonic {

DirectionalShadowBuffer::DirectionalShadowBuffer()
{
	// THROW_NOT_IMPLEMENTED_EXCEPTION();

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWBUFFER_QUALITY, SHADOWBUFFER_QUALITY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture, 0);
	// glReadBuffer(GL_NONE);
	// glDrawBuffer(GL_NONE);
	mFramebuffer.checkStatus();
}


void DirectionalShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	Renderer::cerr() << "Collecting depth buffer for " << lightEntity.name << '\n';

	const auto& lShadowProgram = ShadowShaderProgram::get();
	mat4f lViewMatrix;
	mat4f lProjectionViewMatrix;
	mat4f lProjectionViewModelMatrix;

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	mProjectionMatrix.set_orthographic(1.0f, 1.0f, -1.0f, 1.0f);

	lightEntity.getViewMatrix(lViewMatrix);
	lProjectionViewMatrix = mProjectionMatrix * lViewMatrix;

	Renderer::cerr() << "Projection matix: " << std::fixed << std::setprecision(4) << mProjectionMatrix << '\n';
	Renderer::cerr() << "View matrix:      " << std::fixed << std::setprecision(4) << lViewMatrix << '\n';
	Renderer::cerr() << "ProjectionView:   " << std::fixed << std::setprecision(4) << lProjectionViewMatrix << '\n';

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		Renderer::cerr() << "Rendering to depth buffer: " << lGeometryEntity->name << '\n';
		lProjectionViewModelMatrix = lProjectionViewMatrix * lGeometryEntity->globalTransform();
		lShadowProgram.setMatrixPVM(lProjectionViewModelMatrix);
		// lShadowProgram.setInstancedRendering(0);
		lGeometryEntity->mesh->draw();
	}
}

void DirectionalShadowBuffer::bindDepthTextures() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, 4);
}

const mat4f& DirectionalShadowBuffer::projectionMatrix() const noexcept
{
	return mProjectionMatrix;
}

} // namespace gintonic