#include "PointLight.hpp"

#include "../Math/SQT.hpp"

#include "PointShadowBuffer.hpp"
#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"
#include "../Camera.hpp"

#include <iostream>
#include <iomanip>

#define EPSILON 0.1f // Used in PointLight::shine

// Comment or uncomment this to see the bounding spheres
// Only works in a debug build
#define DEBUG_POINT_LIGHTS

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

void PointLight::setAttenuation(const vec4f& att)
{
	mAttenuation = att;
	calculateCutoffRadius();
}

vec4f PointLight::getAttenuation() const noexcept
{
	return mAttenuation;
}

void PointLight::shine(const Entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT lSphereTransform;

	lSphereTransform.scale = mCutoffRadius;
	lSphereTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);
	lSphereTransform.translation = (e.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	const vec3f lLightPos = (Renderer::matrix_V() * vec4f(lSphereTransform.translation, 1.0f)).data;

	Renderer::setModelMatrix(lSphereTransform);

	const auto& lPointShader = PointLightShaderProgram::get();

	lPointShader.activate();
	lPointShader.setViewportSize(Renderer::viewportSize());
	lPointShader.setGeometryBufferPositionTexture(Renderer::GBUFFER_POSITION);
	lPointShader.setGeometryBufferDiffuseTexture(Renderer::GBUFFER_DIFFUSE);
	lPointShader.setGeometryBufferSpecularTexture(Renderer::GBUFFER_SPECULAR);
	lPointShader.setGeometryBufferNormalTexture(Renderer::GBUFFER_NORMAL);
	lPointShader.setLightIntensity(this->mIntensity);
	lPointShader.setLightPosition(lLightPos);
	lPointShader.setLightAttenuation(mAttenuation);
	lPointShader.setMatrixPVM(Renderer::matrix_PVM());

	// Is the camera inside or outside the sphere?
	const auto lDist = gintonic::distance(Renderer::getCameraPosition(), lSphereTransform.translation);

	// EPSILON is defined at the top of the file.
	const auto lCutoffWithEpsilon = mCutoffRadius + EPSILON * mCutoffRadius;

	if (lDist < lCutoffWithEpsilon)
	{
		// Inside
		#ifdef DEBUG_POINT_LIGHTS
		lPointShader.setDebugFlag(1);
		#endif
		glCullFace(GL_FRONT);
	}
	else
	{
		// Outside
		#ifdef DEBUG_POINT_LIGHTS
		lPointShader.setDebugFlag(2);
		#endif
		glCullFace(GL_BACK);
	}
	#undef EPSILON

	#ifdef DEBUG_POINT_LIGHTS

	lPointShader.setDebugFlag(0);

	Renderer::cerr() 
		<< "Light name:           " << this->name << '\n'
		<< "lightIntensity:       " << std::fixed << std::setprecision(2) << mIntensity << '\n'
		<< "lightPosition:        " << lLightPos << '\n'
		<< "lightAttenuation:     " << getAttenuation() << '\n'
		<< "lightCastShadow:      " << (e.shadowBuffer ? "YES" : "NO") << "\n\n";

	#endif
	
	Renderer::getUnitSphere()->draw();
}

void PointLight::initializeShadowBuffer(Entity& lightEntity) const
{
	lightEntity.shadowBuffer.reset(new PointShadowBuffer());
}

void PointLight::setBrightness(const float value)
{
	Light::setBrightness(value);
	calculateCutoffRadius();	
}

void PointLight::setIntensity(const vec4f& intensity)
{
	Light::setIntensity(intensity);
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
	#define in mIntensity

	const float c = in.w * std::max(in.x, std::max(in.x, in.y));

	if (att.z <= 0.0f)
	{
		// If att.z == 0.0f, solving the equation becomes a lot easier.
		// att.x + att.y * d = 256 * c
		// iff d = (256 * c - att.x) / att.y

		// Solving the equation
		// 1/256 = c / att.x for d is impossible when att.y == 0.0f
		// We just set the cutoff point to zero.
		mCutoffRadius = att.y <= 0.0f ? 0.0f : (256.0f * c - att.x) / att.y;
	}
	else
	{
		// Calculate the discriminant.
		const float discr = att.y * att.y - 4.0f * att.z * (att.x - 256.0f * c);

		// If the discriminant is less than zero,
		// we just set the cutoff point to zero.
		// Else we use the quadratic formula.
		mCutoffRadius = discr < 0.0f ? 0.0f : (-att.y + std::sqrt(discr)) / (2.0f * att.z);
	}

	const auto lFarplane = Renderer::getCameraEntity()->camera->farPlane();
	if (lFarplane / 2.0f < mCutoffRadius) mCutoffRadius = lFarplane / 2.0f;

	// #ifdef DEBUG_POINT_LIGHTS
	// PRINT_VAR(mCutoffRadius);
	// #endif

	#undef in
	#undef att
}

std::ostream& operator << (std::ostream& os, const PointLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& PointLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (PointLight) intensity: " << mIntensity
		<< ", attenuation: " << mAttenuation
		<< ", cutoffPoint: " << mCutoffRadius << " }";
}

} // namespace gintonic