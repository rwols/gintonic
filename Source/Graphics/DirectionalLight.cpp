#include "DirectionalLight.hpp"

#include "../Foundation/exception.hpp"

#include "DirectionalShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
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
	const auto& s = Renderer::get_lp_directional_shader();
	s.activate();

	// These uniforms are always the same for every light shader.
	// Consider using uniform buffers ...
	s.set_viewport_size(Renderer::viewportSize());
	s.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	s.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	s.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	s.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);

	// These uniforms are different for each DirectionalLight.
	s.set_light_intensity(this->mIntensity);

	const auto lLightDirection = Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));
	
	s.set_light_direction(vec3f(lLightDirection.data));

	Renderer::getUnitQuad()->draw();
}

void DirectionalLight::initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const
{
	lightEntity->shadowBuffer.reset(new DirectionalShadowBuffer());
}

void DirectionalLight::beginShadowPass(const Entity& EntityWithLight)
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void DirectionalLight::renderShadow(const Entity& geometry) const noexcept
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();

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