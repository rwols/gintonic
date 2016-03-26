#include "DirectionalLight.hpp"

#include "../Foundation/exception.hpp"

#include "DirectionalShadowBuffer.hpp"
#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"
#include "../Camera.hpp"

#include <iostream>
#include <iomanip>

// Comment or uncomment this to see the bounding spheres
// Only works in a debug build
#define DEBUG_DIRECTIONAL_LIGHTS

namespace gintonic {

DirectionalLight::DirectionalLight(const vec4f& intensity)
: AmbientLight(intensity)
{
	/* Empty on purpose. */
}

void DirectionalLight::shine(const Entity& e) const noexcept
{

	const vec3f lLightDir = vec3f((Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f))).data).normalize();
	
	const auto& lProgram = DirectionalLightShaderProgram::get();

	lProgram.activate();
	lProgram.setViewportSize(Renderer::viewportSize());
	lProgram.setGeometryBufferPositionTexture(Renderer::GBUFFER_POSITION);
	lProgram.setGeometryBufferDiffuseTexture(Renderer::GBUFFER_DIFFUSE);
	lProgram.setGeometryBufferSpecularTexture(Renderer::GBUFFER_SPECULAR);
	lProgram.setGeometryBufferNormalTexture(Renderer::GBUFFER_NORMAL);
	lProgram.setLightShadowDepthTexture(DEPTH_TEXTURE_UNIT);
	lProgram.setLightIntensity(this->mIntensity);
	lProgram.setLightDirection(lLightDir);

	if (e.shadowBuffer)
	{
		e.shadowBuffer->bindDepthTextures();
		const auto lShadowMatrix = e.shadowBuffer->projectionMatrix() * e.getViewMatrix() * Renderer::getCameraEntity()->globalTransform();
		lProgram.setLightCastShadow(1);
		lProgram.setLightShadowMatrix(lShadowMatrix);
	}
	else
	{
		lProgram.setLightCastShadow(0);
	}

	#ifdef DEBUG_DIRECTIONAL_LIGHTS

	Renderer::cerr() 
		<< "Light name:           " << this->name << '\n'
		<< "lightIntensity:       " << std::fixed << std::setprecision(2) << mIntensity << '\n'
		<< "lightDirection:       " << lLightDir << '\n'
		<< "lightCastShadow:      " << (e.shadowBuffer ? "YES" : "NO") << "\n\n";

	#endif

	glCullFace(GL_BACK);
	Renderer::getUnitQuad()->draw();
}

void DirectionalLight::initializeShadowBuffer(Entity& lightEntity) const
{
	lightEntity.shadowBuffer.reset(new DirectionalShadowBuffer());
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