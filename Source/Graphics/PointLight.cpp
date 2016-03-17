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

	SQT lSphereTransform;

	lSphereTransform.scale = mCutoffPoint;
	lSphereTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);
	lSphereTransform.translation = (e.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	Renderer::setModelMatrix(lSphereTransform);

	// const auto& lNullShader = Renderer::get_null_shader();
	const auto& lPointShader = Renderer::get_lp_point_shader();
	const auto lSphereMesh = Renderer::getUnitSphere();

	// We first do a null-pass that only fills the renderer's stencil buffer.
	// The stencil value is increased when a front-facing triangle is seen,
	// and is decreased when a back-facing triangle is seen. This way, the
	// values that are non-zero should be shaded.

	// glDrawBuffer(GL_NONE);
	// glEnable(GL_DEPTH_TEST);
	// glDisable(GL_CULL_FACE);
	// glClear(GL_STENCIL_BUFFER_BIT);
	// glStencilFunc(GL_ALWAYS, 0, 0);
	// glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	// glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	// lNullShader.activate();
	// lNullShader.set_matrix_PVM(Renderer::matrix_PVM());
	// lSphereMesh->draw();

	// Here we use the information collected in the stencil buffer to only
	// shade pixels that really need it.
	
	// glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);
	// glDrawBuffer(GL_BACK);
	// glDisable(GL_DEPTH_TEST);
	// glEnable(GL_BLEND);
	// glBlendEquation(GL_FUNC_ADD);
	// glBlendFunc(GL_ONE, GL_ONE);

	// Is the camera inside the sphere?
	// const auto lCameraPos = vec3f(Renderer::getCameraEntity()->globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f).data);
	// if ((lCameraPos - lSphereTransform.translation).length2() < mCutoffPoint * mCutoffPoint)
	// {
	// 	glCullFace(GL_FRONT);
	// }
	// else
	// {
	// 	glCullFace(GL_BACK);
	// }

	// glDisable(GL_CULL_FACE);

	glCullFace(GL_FRONT);

	lPointShader.activate();
	lPointShader.set_viewport_size(Renderer::viewportSize());
	lPointShader.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	lPointShader.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	lPointShader.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	lPointShader.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);
	lPointShader.set_light_intensity(intensity);

	// The position of the light was in WORLD coordinates.
	// We now transform it further to VIEW coordinates.
	const vec3f lLightPos = (Renderer::matrix_V() * vec4f(lSphereTransform.translation, 1.0f)).data;

	// And then pass it to the shader.
	lPointShader.set_light_position(lLightPos);
	lPointShader.set_light_attenuation(mAttenuation);
	lPointShader.set_matrix_PVM(Renderer::matrix_PVM());
	lPointShader.set_debugflag(0);
	lSphereMesh->draw();

	// glDisable(GL_CULL_FACE);
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
	// Let c = in.w * max(in.x, in.y, in.z)
	// Assume a color value is stored with 8 bits.
	// Solve the equation: 1/256 = c / (att.x + att.y * d + att.z * d),
	// where d denotes the cutoff point.
	// Finally we actually halve this cutoff radius,
	// because in practise its a gigantic sphere radius
	// and the far plane clipping kicks in waaay too early
	// for big spheres.

	#define att mAttenuation
	#define in intensity

	const float c = in.w * std::max(in.x, std::max(in.x, in.y));
	mCutoffPoint = (-att.y + std::sqrt(att.y * att.y - 4.0f * att.z * (att.x - 256.0f * c))) / (2.0f * att.z);
	mCutoffPoint *= 0.5f;

	PRINT_VAR(mCutoffPoint);

	#undef in
	#undef att
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