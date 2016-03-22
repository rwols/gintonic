#include "DirectionalLight.hpp"

#include "../Foundation/exception.hpp"

#include "DirectionalShadowBuffer.hpp"
#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iostream>

#include <iostream>

namespace gintonic {

DirectionalLight::DirectionalLight(const vec4f& intensity)
: AmbientLight(intensity)
{
	/* Empty on purpose. */
}

void DirectionalLight::shine(const Entity& e) const noexcept
{
	const auto& lProgram = DirectionalLightShaderProgram::get();
	lProgram.activate();

	// These uniforms are always the same for every light shader.
	// Consider using uniform buffers ...
	lProgram.setViewportSize(Renderer::viewportSize());
	lProgram.setGeometryBufferPositionTexture(Renderer::GBUFFER_POSITION);
	lProgram.setGeometryBufferDiffuseTexture(Renderer::GBUFFER_DIFFUSE);
	lProgram.setGeometryBufferSpecularTexture(Renderer::GBUFFER_SPECULAR);
	lProgram.setGeometryBufferNormalTexture(Renderer::GBUFFER_NORMAL);

	// These uniforms are different for each DirectionalLight.
	lProgram.setLightIntensity(this->mIntensity);

	const auto lLightDirection = Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));
	
	lProgram.setLightDirection(vec3f(lLightDirection.data));

	Renderer::getUnitQuad()->draw();
}

void DirectionalLight::initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const
{
	lightEntity->shadowBuffer.reset(new DirectionalShadowBuffer());
}

std::ostream& operator << (std::ostream& os, const DirectionalLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& DirectionalLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (DirectionalLight) intensity: " << this->mIntensity << " }";
}


} // namespace gintonic