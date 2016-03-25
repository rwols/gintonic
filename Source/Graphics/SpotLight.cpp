#include "SpotLight.hpp"

#include "../Math/SQT.hpp"

#include "SpotShadowBuffer.hpp"
#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
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
, mCosineHalfAngle(1.0f)
{
	// The cosine of pi/2 is 1, so that's why we set mCosineHalfAngle to 1.
}

SpotLight::SpotLight(const vec4f& intensity, const vec4f& attenuation)
: PointLight(intensity, attenuation)
, mCosineHalfAngle(1.0f)
{
	// The cosine of pi/2 is 1, so that's why we set mCosineHalfAngle to 1.
}

SpotLight::SpotLight(const vec4f& intensity, const vec4f& attenuation, const float angle)
: PointLight(intensity, attenuation)
, mCosineHalfAngle(std::cos(angle))
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

	const auto& lProgram = SpotLightShaderProgram::get();

	lProgram.activate();
	lProgram.setViewportSize(Renderer::viewportSize());
	lProgram.setGeometryBufferPositionTexture(Renderer::GBUFFER_POSITION);
	lProgram.setGeometryBufferDiffuseTexture(Renderer::GBUFFER_DIFFUSE);
	lProgram.setGeometryBufferSpecularTexture(Renderer::GBUFFER_SPECULAR);
	lProgram.setGeometryBufferNormalTexture(Renderer::GBUFFER_NORMAL);
	lProgram.setLightIntensity(this->mIntensity);
	lProgram.setLightPosition(lLightPos);
	lProgram.setLightDirection(lLightDir);
	lProgram.setLightAttenuation(getAttenuation());
	lProgram.setLightCosineHalfAngle(mCosineHalfAngle);
	lProgram.setMatrixPVM(Renderer::matrix_PVM());

	// Is the camera inside or outside the sphere?
	const auto lDist = gintonic::distance(Renderer::getCameraPosition(), lSphereTransform.translation);
	const auto lCutoffWithEpsilon = cutoffPoint() + EPSILON * cutoffPoint();
	if (lDist < lCutoffWithEpsilon)
	{
		// Inside
		#ifdef DEBUG_SPOT_LIGHTS
		lProgram.setDebugFlag(1);
		#endif
		glCullFace(GL_FRONT);
	}
	else
	{
		// Outside
		#ifdef DEBUG_SPOT_LIGHTS
		lProgram.setDebugFlag(2);
		#endif
		glCullFace(GL_BACK);
	}
	
	// Draw a sphere to drive the shader.
	Renderer::getUnitSphere()->draw();
}

void SpotLight::setCosineHalfAngle(const float cosineHalfAngle)
{
	mCosineHalfAngle = cosineHalfAngle;
}

float SpotLight::getCosineHalfAngle() const noexcept
{
	return mCosineHalfAngle;
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