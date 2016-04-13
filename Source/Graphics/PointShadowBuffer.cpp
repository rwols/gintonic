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
	const Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept
{
	// const vec3f lLightPos = lightEntity.globalTransform().data[0];

	// // const auto& lProgram = SilhouetteShaderProgram::get();
	// const auto& lProgram = ShadowVolumeShaderProgram::get();

	// lProgram.activate();
	// // lProgram.setColor(vec3f(1.0f, 1.0f, 1.0f));
	// lProgram.setLightPosition(lLightPos); // in WORLD coordinates

	// glLineWidth(2.0f); // remove me
	// glCullFace(GL_BACK);

	// Renderer::cerr() << "Light position: " << lLightPos << '\n';

	// for (const auto lGeometryEntity : shadowCastingGeometryEntities)
	// {
	// 	Renderer::setModelMatrix(lGeometryEntity->globalTransform());
	// 	lProgram.setMatrixPVM(Renderer::matrix_PVM());
	// 	// lProgram.setMatrixVM(Renderer::matrix_VM());
	// 	// lProgram.setMatrixPVM(Renderer::matrix_P() * Renderer::matrix_V() * lGeometryEntity->globalTransform());
	// 	// lProgram.setMatrixVM(Renderer::matrix_V() * lGeometryEntity->globalTransform());
		
	// 	lGeometryEntity->mesh->drawAdjacent();
	// }
}

void PointShadowBuffer::bindDepthTextures() const noexcept
{
	/* Empty on purpose. */
}

} // namespace gintonic