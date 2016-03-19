#include "SpotLight.hpp"

#include "../Math/SQT.hpp"

#include "SpotShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iostream>

#define EPSILON 0.1f // Used in SpotLight::shine

// Comment or uncomment this to see the bounding spheres
// Only works in a debug build
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

	const auto& lSpotShader = Renderer::get_lp_spot_shader();

	lSpotShader.activate();
	lSpotShader.set_viewport_size(Renderer::viewportSize());
	lSpotShader.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	lSpotShader.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	lSpotShader.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	lSpotShader.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);
	lSpotShader.set_light_intensity(this->mIntensity);
	lSpotShader.set_light_position(lLightPos);
	lSpotShader.set_light_direction(lLightDir);
	lSpotShader.set_light_attenuation(getAttenuation());
	lSpotShader.set_light_angle(mAngle);
	lSpotShader.set_matrix_PVM(Renderer::matrix_PVM());

	// Is the camera inside or outside the sphere?
	const auto lDist = gintonic::distance(Renderer::getCameraPosition(), lSphereTransform.translation);
	const auto lCutoffWithEpsilon = cutoffPoint() + EPSILON * cutoffPoint();
	if (lDist < lCutoffWithEpsilon)
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
	
	// Draw a sphere to drive the shader.
	Renderer::getUnitSphere()->draw();
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
	return os << "{ (SpotLight) intensity: " << this->mIntensity
		<< ", attenuation: " << getAttenuation()
		<< ", cutoffPoint: " << cutoffPoint() << " }";
}


} // namespace gintonic