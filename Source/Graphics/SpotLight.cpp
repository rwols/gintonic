#include "SpotLight.hpp"

#include "../Math/SQT.hpp"

#include "SpotShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iostream>

namespace gintonic {


SpotLight::SpotLight(const vec4f& intensity)
: PointLight(intensity)
{
	/* Empty on purpose. */
}

SpotLight::SpotLight(const vec4f& intensity, const vec4f& attenuation)
: PointLight(intensity, attenuation)
{
	/* Empty on purpose. */
}

void SpotLight::shine(const Entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = cutoffPoint();
	sphere_transform.translation = (e.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	Renderer::setModelMatrix(sphere_transform);

	const auto& nullshader = Renderer::get_null_shader();
	const auto& spotshader = Renderer::get_lp_spot_shader();
	const auto sphere = Renderer::getUnitSphere();

	// We first do a null-pass that only fills the renderer's stencil buffer.
	// The stencil value is increased when a front-facing triangle is seen,
	// and is decreased when a back-facing triangle is seen. This way, the
	// values that are non-zero should be shaded.

	Renderer::begin_stencil_pass();
	nullshader.activate();
	nullshader.set_matrix_PVM(Renderer::matrix_PVM());
	sphere->draw();

	// Here we use the information collected in the stencil buffer to only
	// shade pixels that really need it.
	
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	const auto light_pos = Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	const auto light_dir = Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));

	Renderer::begin_light_pass();

	spotshader.activate();
	spotshader.set_viewport_size(Renderer::viewportSize());
	spotshader.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	spotshader.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	spotshader.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	spotshader.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);
	spotshader.set_light_intensity(intensity);
	spotshader.set_light_position(vec3f(light_pos.data));
	spotshader.set_light_direction(vec3f(light_dir.data));
	spotshader.set_light_attenuation(attenuation());
	spotshader.set_matrix_PVM(Renderer::matrix_PVM());
	sphere->draw();

	glDisable(GL_CULL_FACE);
}

void SpotLight::initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const
{
	lightEntity->shadowBuffer.reset(new SpotShadowBuffer());
}


std::ostream& operator << (std::ostream& os, const SpotLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& SpotLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (SpotLight) intensity: " << intensity
		<< ", attenuation: " << attenuation()
		<< ", cutoffPoint: " << cutoffPoint() << " }";
}


} // namespace gintonic