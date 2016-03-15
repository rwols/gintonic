#include "PointLight.hpp"

#include "../Math/SQT.hpp"

#include "PointShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iostream>

#include <iostream>

namespace gintonic {

PointLight::PointLight(const vec4f& intensity)
: Light(intensity)
{
	/* Empty on purpose. */
}

PointLight::PointLight(const vec4f& intensity, const vec4f& attenuation)
: Light(intensity)
{
	setAttenuation(attenuation);
}

void PointLight::setAttenuation(const vec4f& att) noexcept
{
	mAttenuation = att;
	calculateCutoffRadius();
}

vec4f PointLight::attenuation() const noexcept
{
	return mAttenuation;
}

float PointLight::cutoffPoint() const noexcept
{
	return mCutoffPoint;
}

void PointLight::shine(const Entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = mCutoffPoint;
	sphere_transform.translation = (e.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	Renderer::setModelMatrix(sphere_transform);

	const auto& nullshader = Renderer::get_null_shader();
	const auto& pointshader = Renderer::get_lp_point_shader();
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

	Renderer::begin_light_pass();

	pointshader.activate();
	pointshader.set_viewport_size(Renderer::viewportSize());
	pointshader.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	pointshader.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	pointshader.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	pointshader.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);
	pointshader.set_light_intensity(intensity);
	pointshader.set_light_position(vec3f(light_pos.data));
	pointshader.set_light_attenuation(mAttenuation);
	pointshader.set_matrix_PVM(Renderer::matrix_PVM());
	sphere->draw();

	glDisable(GL_CULL_FACE);
}

void PointLight::initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const
{
	lightEntity->shadowBuffer.reset(new PointShadowBuffer());
}

void PointLight::setBrightness(const float brightness)
{
	intensity.w = brightness;
	calculateCutoffRadius();
}

void PointLight::calculateCutoffRadius() noexcept
{

	// Let c be equal to intensity.w * max(intensity[0], intensity[1], intensity[2])
	// Let a be the attenuation
	// Let d be the cutoff radius
	// There are 256 possible values in an 8-bit color channel
	// Therefore,
	//
	//     a[0] + a[1] * d + a[2] * d^2 = 256 * c
	//
	// Solving for d, we find
	//
	// d = (-a[1] + sqrt(a[1]^2 - 4*a[2]*(a[0] - 256 * c))) / (2 * a[2])

	#define att mAttenuation
	#define in intensity

	const float c = in.w * std::max(in.x, std::max(in.x, in.y));

	mCutoffPoint = (-att.x + std::sqrt(att.x * att.x 
		- 4.0f * att.y * (att.x - 256.0f * c))) 
	/ (2 * att.y);

	#undef in
	#undef att

	// #ifdef ENABLE_DEBUG_TRACE
	// Renderer::cerr() << "Cutoff point was set to " << mCutoffPoint << '\n';
	// #endif
}

std::ostream& operator << (std::ostream& os, const PointLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& PointLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (PointLight) intensity: " << intensity
		<< ", attenuation: " << mAttenuation
		<< ", cutoffPoint: " << mCutoffPoint << " }";
}

} // namespace gintonic