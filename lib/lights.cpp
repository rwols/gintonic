#include "lights.hpp"
#include "shaders.hpp"
#include "renderer.hpp"
#include "basic_shapes.hpp"

namespace gintonic {

light::light(const vec3f& intensity)
: intensity(intensity)
{
	/* Empty on purpose. */
}

light::~light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void light::shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_lp_null_shader();
	s.activate();
	s.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	s.set_viewport_size(vec2f(static_cast<float>(renderer::width()), static_cast<float>(renderer::height())));
	renderer::get_unit_quad_P().draw();
}

directional_light::directional_light(const vec3f& intensity)
: light(intensity)
{
	/* Empty on purpose. */
}

directional_light::~directional_light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}


void directional_light::shine(const sqt_transformf& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_lp_directional_shader();
	s.activate();
	s.set_viewport_size(vec2f(static_cast<float>(renderer::width()), static_cast<float>(renderer::height())));
	// s.set_gbuffer_position(renderer::GBUFFER_POSITION);
	s.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	// s.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	s.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	s.set_light_intensity(intensity);
	// s.set_light_direction(renderer::camera().matrix_V().upper_left_33() * vec3f(0, -1, 0));
	s.set_light_direction(renderer::camera().matrix_V().upper_left_33() * t.rotation.direction());
	renderer::get_unit_quad_P().draw();
}

point_light::point_light(const vec3f& intensity)
: light(intensity)
{
	/* Empty on purpose. */
}

point_light::point_light(const vec3f& intensity, const vec3f& attenuation)
: light(intensity)
{
	set_attenuation(attenuation);
}

point_light::~point_light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void point_light::set_attenuation(const vec3f& att) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_attenuation = att;

	// Let c be equal to max(intensity[0], intensity[1], intensity[2])
	// Let a be the attenuation
	// Let d be the cutoff point
	// There are 256 possible values in an 8-bit color channel
	//
	// We assume that a[0] + a[1] * d + a[2] * d^2 = 256 * c
	//
	// Solving for d, we find
	//
	// d = (-a[1] + sqrt(a[1]^2 - 4*a[2]*(a[0] - 256 * c))) / (2 * a[2])

	const float c = std::max(intensity[0], std::max(intensity[1], intensity[2]));

	std::cout << "Max intensity: " << c << '\n';

	m_cutoff_point = (-att[1] + std::sqrt(att[1] * att[1] 
		- 4.0f * att[2] * (att[0] - 256.0f * c))) 
	/ (2 * att[2]);

	std::cout << "Cutoff point was set to: " << m_cutoff_point << '\n';
}

vec3f point_light::attenuation() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return m_attenuation;
}

float point_light::cutoff_point() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return m_cutoff_point;
}

void point_light::shine(const sqt_transformf& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	sqt_transformf sphere_transform;
	sphere_transform.scale = 4.0f;
	sphere_transform.translation = t.translation;

	renderer::set_model_matrix(sphere_transform.get_matrix());

	const auto& nullshader = renderer::get_null_shader();
	const auto& pointshader = renderer::get_lp_point_shader();
	const auto& sphere = renderer::get_unit_sphere_P();

    glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	nullshader.activate();
	nullshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	// renderer::bind_for_light_pass();

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	pointshader.activate();
	pointshader.set_viewport_size(vec2f(static_cast<float>(renderer::width()), static_cast<float>(renderer::height())));
	pointshader.set_gbuffer_position(renderer::GBUFFER_POSITION);
	pointshader.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	// pointshader.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	pointshader.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	pointshader.set_light_intensity(intensity);
	pointshader.set_light_position(t.translation);
	pointshader.set_light_attenuation(m_attenuation);
	pointshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();
}

} // namespace gintonic