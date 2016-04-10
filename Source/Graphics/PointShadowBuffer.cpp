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
	/* Empty on purpose. */
}

void PointShadowBuffer::collect(
	Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	const vec3f lLightPos = (Renderer::matrix_V() * lightEntity.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	const auto& lProgram = SilhouetteShaderProgram::get();

	lProgram.activate();
	lProgram.setColor(vec3f(1.0f, 0.0f, 0.0f));
	lProgram.setLightPosition(lLightPos);

	glLineWidth(2.0f); // remove me
	glCullFace(GL_BACK);

	for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	{
		Renderer::setModelMatrix(lGeometryEntity->globalTransform());
		lProgram.setMatrixPVM(Renderer::matrix_PVM());
		lProgram.setMatrixVM(Renderer::matrix_VM());
		
		lGeometryEntity->mesh->drawAdjacent();
	}
}

void PointShadowBuffer::bindDepthTextures() const noexcept
{
	/* Empty on purpose. */
}

} // namespace gintonic