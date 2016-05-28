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
// #define DEBUG_POINT_LIGHTS

namespace gintonic {

PointLight::PointLight(const vec4f& intensity)
: Super(intensity)
{
	/* Empty on purpose. */
}

PointLight::PointLight(const vec4f& intensity, const vec4f& attenuation)
: Super(intensity)
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

void PointLight::shine(
	const Entity& lightEntity, 
	const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) const noexcept
{
	const auto& lShadowVolumeProgram = ShadowVolumeShaderProgram::get();
	const auto& lPointLightProgram   = PointLightShaderProgram::get();

	vec3f lLightPos;

	if (lightEntity.castShadow)
	{
		// Go to world space.
		lLightPos = (lightEntity.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

		// Polygon offset is needed because otherwise the shadow volume will
		// Z-fight with the original geometry. We need to draw the shadow volume
		// just a tiny bit "away" from the original geometry. It's possible to do this
		// in the geometry shader, but I find this a more elegant solution.
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		Renderer::beginStencilPass();
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
		glClear(GL_STENCIL_BUFFER_BIT); // Clear the stencil buffer.
	
		lShadowVolumeProgram.activate();

		vec3f lLightPosInLocalCoordinates;
		const auto lMatrixPV = Renderer::getCameraEntity()->camera->projectionMatrix() * Renderer::matrix_V();
		for (const auto lGeometryEntity : shadowCastingGeometryEntities)
		{
			// Go from world space to the local space of the mesh.
			lLightPosInLocalCoordinates = (lGeometryEntity->getViewMatrix() * vec4f(lLightPos, 1.0f)).data;
			lShadowVolumeProgram.setLightPosition(lLightPosInLocalCoordinates);
			lShadowVolumeProgram.setMatrixPVM( lMatrixPV * lGeometryEntity->globalTransform() );
			lGeometryEntity->mesh->drawAdjacent();
		}

		glStencilFunc(GL_EQUAL, 0x0, 0xff); // Draw only if the corresponding stencil value is zero.
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Prevent update to the stencil buffer.
		Renderer::endStencilPass();

		// Restore original state. Here we implicitly assume that the default
		// state is to apply no polygon offset.
		glDisable(GL_POLYGON_OFFSET_FILL);
		
		// Move the light from world space to view space.
		lLightPos = (Renderer::matrix_V() * vec4f(lLightPos, 1.0f)).data;
	}
	else
	{
		// Move the light directly all the way to view space.
		lLightPos = (Renderer::matrix_V() * lightEntity.globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;
	}

	lPointLightProgram.activate();
	lPointLightProgram.setViewportSize(Renderer::viewportSize());
	lPointLightProgram.setGeometryBufferPositionTexture(Renderer::GBUFFER_POSITION);
	lPointLightProgram.setGeometryBufferDiffuseTexture(Renderer::GBUFFER_DIFFUSE);
	lPointLightProgram.setGeometryBufferSpecularTexture(Renderer::GBUFFER_SPECULAR);
	lPointLightProgram.setGeometryBufferNormalTexture(Renderer::GBUFFER_NORMAL);
	lPointLightProgram.setLightIntensity(this->mIntensity);
	lPointLightProgram.setLightPosition(lLightPos);
	lPointLightProgram.setLightAttenuation(mAttenuation);
	lPointLightProgram.setMatrixPVM(mat4f(1.0f));

	#ifdef DEBUG_POINT_LIGHTS

	lPointLightProgram.setDebugFlag(0);

	Renderer::cerr() 
		<< "Light name:           " << this->name << '\n'
		<< "lightIntensity:       " << std::fixed << std::setprecision(2) << mIntensity << '\n'
		<< "lightPosition:        " << lLightPosition << '\n'
		<< "lightAttenuation:     " << getAttenuation() << '\n'
		<< "lightCastShadow:      " << (lightEntity.shadowBuffer ? "YES" : "NO") << "\n\n";

	#endif
	
	Renderer::getUnitQuad()->draw();
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
