#include "DirectionalLight.hpp"

#include "../Foundation/exception.hpp"

#include "DirectionalShadowBuffer.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include "../Entity.hpp"

#include <iostream>

#include <iostream>

namespace gintonic {

DirectionalLight::DirectionalLight(const vec4f& intensity)
: AmbientLight(intensity)
{
	/* Empty on purpose. */
}

#define SHADOW_QUALITY 1024

// void DirectionalLight::attach(Entity& e)
// {
// 	// Don't forget to call the base method.
// 	Light::attach(e);

// 	std::shared_ptr<OpenGL::Framebuffer> framebuf(new OpenGL::Framebuffer());
// 	std::shared_ptr<OpenGL::TextureObject> texture(new OpenGL::TextureObject());

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

// 	std::cerr << "Directional light " << this << " attached to Entity " << &e << '\n';
// }

// void DirectionalLight::detach(Entity& e)
// {
// 	const auto r = m_shadow_maps.find(&e);
// 	assert(r != m_shadow_maps.end());
// 	m_shadow_maps.erase(r);

// 	// Don't forget to call the base method.
// 	Light::detach(e);

// 	std::cerr << "Directional light " << this << " detached from Entity " << &e << '\n';
// }

void DirectionalLight::shine(const Entity& e) const noexcept
{
	const auto& s = Renderer::get_lp_directional_shader();
	s.activate();

	// These uniforms are always the same for every light shader.
	// Consider using uniform buffers ...
	s.set_viewport_size(Renderer::viewportSize());
	s.set_gbuffer_position(Renderer::GBUFFER_POSITION);
	s.set_gbuffer_diffuse(Renderer::GBUFFER_DIFFUSE);
	s.set_gbuffer_specular(Renderer::GBUFFER_SPECULAR);
	s.set_gbuffer_normal(Renderer::GBUFFER_NORMAL);

	// These uniforms are different for each DirectionalLight.
	s.set_light_intensity(intensity);

	const auto lLightDirection = Renderer::matrix_V() * (e.globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));
	
	s.set_light_direction(vec3f(lLightDirection.data));

	Renderer::getUnitQuad()->draw();
}

void DirectionalLight::initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const
{
	lightEntity->shadowBuffer.reset(new DirectionalShadowBuffer());
}

void DirectionalLight::beginShadowPass(const Entity& EntityWithLight)
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
	// const auto r = m_shadow_maps.find(&EntityWithLight);
	// if (r == m_shadow_maps.end())
	// {
	// 	// This light component is not attached to the given Entity.
	// 	// We cannot fix the problem by attaching it at this point,
	// 	// because we have a const reference. We just throw an exception.
	// 	throw std::runtime_error("Light component is not attached to given Entity.");
	// }
	// const auto* framebuf = r->second.first.get();
	// // const auto* texture = r->second.second.get();
	// glBindFramebuffer(GL_FRAMEBUFFER, *framebuf);
	// // glActiveTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	// // glBindTexture(GL_TEXTURE_2D, *texture);
	// Renderer::get_sp_directional_shader().activate();
	// mat4f world_to_light_space;
	// EntityWithLight.get_view_matrix(world_to_light_space);
	// m_current_matrix_PV = EntityWithLight.camera->projectionMatrix() * world_to_light_space;
}

void DirectionalLight::renderShadow(const Entity& geometry) const noexcept
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
	// const auto matrix_PVM = m_current_matrix_PV * geometry.globalTransform();
	// Renderer::get_sp_directional_shader().set_matrix_PVM(matrix_PVM);
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


} // namespace gintonic