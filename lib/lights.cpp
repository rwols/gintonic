#include "lights.hpp"
#include "shaders.hpp"
#include "basic_shapes.hpp"
#include "entity.hpp"
#include "renderer.hpp"
#include "camera.hpp"
#include "proj_info.hpp"
#include "exception.hpp"
#include "DirectionalShadowBuffer.hpp"
#include "PointShadowBuffer.hpp"
#include "SpotShadowBuffer.hpp"

#ifdef ENABLE_DEBUG_TRACE
	#include "fonts.hpp"
#endif

namespace gintonic {

std::shared_ptr<Light> Light::create(const FbxLight* pFbxLight)
{
	const auto lFbxIntensity = static_cast<float>(pFbxLight->Intensity.Get());
	const auto lFbxColor = pFbxLight->Color.Get();
	const vec4f lIntensity(static_cast<float>(lFbxColor[0]), 
		static_cast<float>(lFbxColor[1]), 
		static_cast<float>(lFbxColor[2]), 
		lFbxIntensity);
	vec4f lAttenuation;
	std::shared_ptr<Light> lLight;
	
	switch (pFbxLight->DecayType.Get())
	{
		case FbxLight::eNone:
		{
			lAttenuation.x = 1.0f;
			break;
		}
		case FbxLight::eLinear:
		{
			lAttenuation.y = 1.0f;
			break;
		}
		case FbxLight::eQuadratic:
		{
			lAttenuation.z = 1.0f;
			break;
		}
		case FbxLight::eCubic:
		{
			throw std::runtime_error("There's not support for cubic falloff.");
		}
		default:
		{
			lAttenuation.z = 1.0f; // quadratic is default
		}
	}
	switch (pFbxLight->LightType.Get())
	{
		case FbxLight::ePoint:
		{
			std::cerr << "\tPoint light\n";
			std::cerr << "\tIntensity: " << lIntensity << '\n';
			std::cerr << "\tAttenuation: " << lAttenuation << '\n';
			lLight = std::make_shared<PointLight>(lIntensity, lAttenuation);
			break;
		}
		case FbxLight::eDirectional:
		{
			std::cerr << "\tDirectional light\n";
			lLight = std::make_shared<DirectionalLight>(lIntensity);
			break;
		}
		case FbxLight::eSpot:
		{
			std::cerr << "\tSpot light\n";
			lLight = std::make_shared<SpotLight>(lIntensity, lAttenuation);
		}
		case FbxLight::eArea:
		{
			std::cerr << "\tArea light is not supported.\n";
			break;
		}
		case FbxLight::eVolume:
		{
			std::cerr << "\tVolume light is not supported.\n";
			break;
		}
		default:
		{
			std::cerr << "\tUnknown light!\n";
			break;
		}
	}
	auto lGlobalName = boost::filesystem::path(pFbxLight->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
	if (std::strcmp(pFbxLight->GetName(), "") == 0)
	{
		lLight->setName(std::move(lGlobalName), pFbxLight->GetName());
	}
	else
	{
		lLight->setName(std::move(lGlobalName), pFbxLight->GetNode()->GetName());
	}
	return lLight;
}

Light::Light(const vec4f& intensity)
: intensity(intensity)
{
	/* Empty on purpose. */
}

void Light::setBrightness(const float brightness)
{
	intensity.w = brightness;
}

float Light::brightness() const noexcept
{
	return intensity.w;
}

std::ostream& operator << (std::ostream& os, const Light* l)
{
	return l->prettyPrint(os);
}

std::ostream& operator << (std::ostream& os, const std::unique_ptr<Light>& l)
{
	return l->prettyPrint(os);
}

std::ostream& operator << (std::ostream& os, const std::shared_ptr<Light>& l)
{
	return l->prettyPrint(os);
}

std::ostream& operator << (std::ostream& os, const Light& l)
{
	return l.prettyPrint(os);
}

std::ostream& Light::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (Light) intensity: " << intensity << " }";
}

AmbientLight::AmbientLight(const vec4f& intensity)
: Light(intensity)
{
	/* Empty on purpose. */
}

void AmbientLight::shine(const entity& e) const noexcept
{
	const auto& s = renderer::get_lp_ambient_shader();
	s.activate();
	s.set_viewport_size(renderer::viewport_size());
	s.set_light_intensity(intensity);
	renderer::get_unit_quad_P().draw();
}

std::ostream& operator << (std::ostream& os, const AmbientLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& AmbientLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (AmbientLight) intensity: " << intensity << " }";
}

DirectionalLight::DirectionalLight(const vec4f& intensity)
: AmbientLight(intensity)
{
	/* Empty on purpose. */
}

#define SHADOW_QUALITY 1024

// void DirectionalLight::attach(entity& e)
// {
// 	// Don't forget to call the base method.
// 	Light::attach(e);

// 	std::shared_ptr<opengl::framebuffer> framebuf(new opengl::framebuffer());
// 	std::shared_ptr<opengl::texture_object> texture(new opengl::texture_object());

// 	glBindTexture(GL_TEXTURE_2D, *texture);
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_QUALITY, SHADOW_QUALITY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
// 	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

// 	glBindFramebuffer(GL_FRAMEBUFFER, *framebuf);
// 	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texture, 0);

// 	// Disable writes to the color buffer
// 	glDrawBuffer(GL_NONE);
// 	glReadBuffer(GL_NONE);

// 	framebuf->check_status();

// 	// Everything went okay at this point. Add it to our map of shadow maps.
// 	m_shadow_maps.emplace(&e, std::make_pair(framebuf, texture));

// 	std::cerr << "Directional light " << this << " attached to entity " << &e << '\n';
// }

// void DirectionalLight::detach(entity& e)
// {
// 	const auto r = m_shadow_maps.find(&e);
// 	assert(r != m_shadow_maps.end());
// 	m_shadow_maps.erase(r);

// 	// Don't forget to call the base method.
// 	Light::detach(e);

// 	std::cerr << "Directional light " << this << " detached from entity " << &e << '\n';
// }

void DirectionalLight::shine(const entity& e) const noexcept
{
	const auto& s = renderer::get_lp_directional_shader();
	s.activate();

	// These uniforms are always the same for every light shader.
	// Consider using uniform buffers ...
	s.set_viewport_size(renderer::viewport_size());
	s.set_gbuffer_position(renderer::GBUFFER_POSITION);
	s.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	s.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	s.set_gbuffer_normal(renderer::GBUFFER_NORMAL);

	// These uniforms are different for each DirectionalLight.
	s.set_light_intensity(intensity);

	const auto lLightDirection = renderer::matrix_V() * (e.global_transform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));
	
	s.set_light_direction(vec3f(lLightDirection.data));

	renderer::get_unit_quad_P().draw();
}

void DirectionalLight::initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const
{
	lightEntity->shadowBuffer.reset(new DirectionalShadowBuffer());
}

void DirectionalLight::beginShadowPass(const entity& entityWithLight)
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
	// const auto r = m_shadow_maps.find(&entityWithLight);
	// if (r == m_shadow_maps.end())
	// {
	// 	// This light component is not attached to the given entity.
	// 	// We cannot fix the problem by attaching it at this point,
	// 	// because we have a const reference. We just throw an exception.
	// 	throw std::runtime_error("Light component is not attached to given entity.");
	// }
	// const auto* framebuf = r->second.first.get();
	// // const auto* texture = r->second.second.get();
	// glBindFramebuffer(GL_FRAMEBUFFER, *framebuf);
	// // glActiveTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	// // glBindTexture(GL_TEXTURE_2D, *texture);
	// renderer::get_sp_directional_shader().activate();
	// mat4f world_to_light_space;
	// entityWithLight.get_view_matrix(world_to_light_space);
	// m_current_matrix_PV = entityWithLight.camera->projectionMatrix() * world_to_light_space;
}

void DirectionalLight::renderShadow(const entity& geometry) const noexcept
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
	// const auto matrix_PVM = m_current_matrix_PV * geometry.global_transform();
	// renderer::get_sp_directional_shader().set_matrix_PVM(matrix_PVM);
	// geometry.mesh->draw();
}

std::ostream& operator << (std::ostream& os, const DirectionalLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& DirectionalLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (DirectionalLight) intensity: " << intensity << " }";
}

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

void PointLight::shine(const entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = mCutoffPoint;
	sphere_transform.translation = (e.global_transform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	renderer::set_model_matrix(sphere_transform);

	const auto& nullshader = renderer::get_null_shader();
	const auto& pointshader = renderer::get_lp_point_shader();
	const auto& sphere = renderer::get_unit_sphere_P();

	// We first do a null-pass that only fills the renderer's stencil buffer.
	// The stencil value is increased when a front-facing triangle is seen,
	// and is decreased when a back-facing triangle is seen. This way, the
	// values that are non-zero should be shaded.

	renderer::begin_stencil_pass();
	nullshader.activate();
	nullshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	// Here we use the information collected in the stencil buffer to only
	// shade pixels that really need it.
	
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	const auto light_pos = renderer::matrix_V() * (e.global_transform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f));

	renderer::begin_light_pass();

	pointshader.activate();
	pointshader.set_viewport_size(renderer::viewport_size());
	pointshader.set_gbuffer_position(renderer::GBUFFER_POSITION);
	pointshader.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	pointshader.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	pointshader.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	pointshader.set_light_intensity(intensity);
	pointshader.set_light_position(vec3f(light_pos.data));
	pointshader.set_light_attenuation(mAttenuation);
	pointshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	glDisable(GL_CULL_FACE);
}

void PointLight::initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const
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
	// renderer::cerr() << "Cutoff point was set to " << mCutoffPoint << '\n';
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

void SpotLight::shine(const entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = cutoffPoint();
	sphere_transform.translation = (e.global_transform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data;

	renderer::set_model_matrix(sphere_transform);

	const auto& nullshader = renderer::get_null_shader();
	const auto& spotshader = renderer::get_lp_spot_shader();
	const auto& sphere = renderer::get_unit_sphere_P();

	// We first do a null-pass that only fills the renderer's stencil buffer.
	// The stencil value is increased when a front-facing triangle is seen,
	// and is decreased when a back-facing triangle is seen. This way, the
	// values that are non-zero should be shaded.

	renderer::begin_stencil_pass();
	nullshader.activate();
	nullshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	// Here we use the information collected in the stencil buffer to only
	// shade pixels that really need it.
	
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	const auto light_pos = renderer::matrix_V() * (e.global_transform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	const auto light_dir = renderer::matrix_V() * (e.global_transform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));

	renderer::begin_light_pass();

	spotshader.activate();
	spotshader.set_viewport_size(renderer::viewport_size());
	spotshader.set_gbuffer_position(renderer::GBUFFER_POSITION);
	spotshader.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	spotshader.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	spotshader.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	spotshader.set_light_intensity(intensity);
	spotshader.set_light_position(vec3f(light_pos.data));
	spotshader.set_light_direction(vec3f(light_dir.data));
	spotshader.set_light_attenuation(attenuation());
	spotshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	glDisable(GL_CULL_FACE);
}

void SpotLight::initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const
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