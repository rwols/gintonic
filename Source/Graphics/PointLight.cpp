#include "PointLight.hpp"

#include "../Math/SQT.hpp"

#include "PointShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"
#include "../Camera.hpp"

#include <iostream>

#include <iostream>

// Comment or uncomment this to see the bounding spheres
// #define DEBUG_POINT_LIGHTS

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

	const vec3f lLightPos = (Renderer::matrix_V() * vec4f(lSphereTransform.translation, 1.0f)).data;

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

	lPointShader.activate();
	lPointShader.set_viewport_size(Renderer::viewportSize());
	lPointShader.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	lPointShader.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	lPointShader.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	lPointShader.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);
	lPointShader.set_light_intensity(intensity);
	lPointShader.set_light_position(lLightPos);
	lPointShader.set_light_attenuation(mAttenuation);
	lPointShader.set_matrix_PVM(Renderer::matrix_PVM());

	#define EPSILON 1.0f
	// Is the camera inside or outside the sphere?
	const auto lDist = gintonic::distance(Renderer::getCameraPosition(), lSphereTransform.translation);
	if (lDist < mCutoffPoint + EPSILON)
	{
		// Inside
		#ifdef DEBUG_POINT_LIGHTS
		lPointShader.set_debugflag(1);
		#endif
		glCullFace(GL_FRONT);
	}
	else
	{
		// Outside
		#ifdef DEBUG_POINT_LIGHTS
		lPointShader.set_debugflag(2);
		#endif
		glCullFace(GL_BACK);
	}
	#undef EPSILON

	// glCullFace(GL_FRONT);
	
	lSphereMesh->draw();
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

	if (att.z <= 0.0f)
	{
		// If att.z == 0.0f, solving the equation becomes a lot easier.
		// att.x + att.y * d = 256 * c
		// iff d = (256 * c - att.x) / att.y

		// Solving the equation
		// 1/256 = c / att.x for d is impossible when att.y == 0.0f
		// We just set the cutoff point to zero.
		mCutoffPoint = att.y <= 0.0f ? 0.0f : (256.0f * c - att.x) / att.y;
	}
	else
	{
		// Calculate the discriminant.
		const float discr = att.y * att.y - 4.0f * att.z * (att.x - 256.0f * c);

		// If the discriminant is less than zero,
		// we just set the cutoff point to zero.
		// Else we use the quadratic formula.
		mCutoffPoint = discr < 0.0f ? 0.0f : (-att.y + std::sqrt(discr)) / (2.0f * att.z);
	}

	const auto lFarplane = Renderer::getCameraEntity()->camera->farPlane();
	if (lFarplane / 2.0f < mCutoffPoint) mCutoffPoint = lFarplane / 2.0f;

	#ifdef DEBUG_POINT_LIGHTS
	PRINT_VAR(mCutoffPoint);
	#endif

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