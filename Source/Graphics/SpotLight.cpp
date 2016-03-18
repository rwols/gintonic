#include "SpotLight.hpp"

#include "../Math/SQT.hpp"

#include "SpotShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iostream>

// #define DEBUG_SPOT_LIGHTS

namespace gintonic {

SpotLight::SpotLight(const vec4f& intensity)
: PointLight(intensity)
, mAngle(1.0f)
{
	// The cosine of pi/2 is 1, so that's why we set mAngle to 1.
}

SpotLight::SpotLight(const vec4f& intensity, const vec4f& attenuation)
: PointLight(intensity, attenuation)
, mAngle(1.0f)
{
	// The cosine of pi/2 is 1, so that's why we set mAngle to 1.
}

SpotLight::SpotLight(const vec4f& intensity, const vec4f& attenuation, const float angle)
: PointLight(intensity, attenuation)
, mAngle(std::cos(angle))
{
	/* Empty on purpose. */
}

void SpotLight::shine(const Entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT lSphereTransform;

	lSphereTransform.scale = cutoffPoint();
	lSphereTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);
	lSphereTransform.translation = (e.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	const vec3f lLightPos = (Renderer::matrix_V() * vec4f(lSphereTransform.translation, 1.0f)).data;
	const vec3f lLightDir = (Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f))).data;

	Renderer::setModelMatrix(lSphereTransform);

	// const auto& lNullShader = Renderer::get_null_shader();
	const auto& lSpotShader = Renderer::get_lp_spot_shader();
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

	lSpotShader.activate();
	lSpotShader.set_viewport_size(Renderer::viewportSize());
	lSpotShader.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	lSpotShader.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	lSpotShader.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	lSpotShader.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);
	lSpotShader.set_light_intensity(intensity);
	lSpotShader.set_light_position(lLightPos);
	lSpotShader.set_light_direction(lLightDir);
	lSpotShader.set_light_attenuation(getAttenuation());
	lSpotShader.set_light_angle(mAngle);
	lSpotShader.set_matrix_PVM(Renderer::matrix_PVM());

	#define EPSILON 1.0f
	// Is the camera inside or outside the sphere?
	const auto lDist = gintonic::distance(Renderer::getCameraPosition(), lSphereTransform.translation);
	if (lDist < cutoffPoint() + EPSILON)
	{
		// Inside
		#ifdef DEBUG_SPOT_LIGHTS
		lSpotShader.set_debugflag(1);
		#endif
		glCullFace(GL_FRONT);
	}
	else
	{
		// Outside
		#ifdef DEBUG_SPOT_LIGHTS
		lSpotShader.set_debugflag(2);
		#endif
		glCullFace(GL_BACK);
	}
	#undef EPSILON

	// glCullFace(GL_FRONT);
	
	lSphereMesh->draw();
}

void SpotLight::setAngle(const float angle)
{
	mAngle = std::cos(angle);
}

float SpotLight::getAngle() const noexcept
{
	return std::acos(mAngle);
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
		<< ", attenuation: " << getAttenuation()
		<< ", cutoffPoint: " << cutoffPoint() << " }";
}


} // namespace gintonic