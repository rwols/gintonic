#include "DirectionalShadowBuffer.hpp"

#include "../Foundation/exception.hpp"

#include "../Math/mat4f.hpp"

#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iomanip>

#define DEPTH_TEXTURE_UNIT 4

#define SHADOW_QUALITY 512

namespace gintonic {

DirectionalShadowBuffer::DirectionalShadowBuffer()
{
	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_QUALITY, SHADOW_QUALITY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	mFramebuffer.checkStatus();
}


void DirectionalShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	const auto& lShadowProgram = ShadowShaderProgram::get();

	lightEntity.setTranslation(vec3f(0.0f, 0.0f, 0.0f));
	
	mat4f lProjectionViewModelMatrix;
	const auto lViewMatrix = lightEntity.getViewMatrix();

	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	mProjectionMatrix.set_orthographic(2.0f, 2.0f, -1.0f, 1.0f);
	const auto lProjectionViewMatrix = mProjectionMatrix * lViewMatrix;

	Renderer::cerr() << "Collecting depth buffer for " << lightEntity.name << '\n'
		<< "Projection matix: " << std::fixed << std::setprecision(4) << mProjectionMatrix << '\n'
		<< "ProjectionView:   " << std::fixed << std::setprecision(4) << lProjectionViewMatrix << '\n';

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		lProjectionViewModelMatrix = lProjectionViewMatrix * lGeometryEntity->globalTransform();
		Renderer::cerr() << "Rendering to depth buffer:  " << lGeometryEntity->name << '\n'
			<< "ProjectionViewModel matrix: " << std::fixed << std::setprecision(4) 
			<< lProjectionViewModelMatrix << "\n\n";
		lShadowProgram.setMatrixPVM(lProjectionViewModelMatrix);
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