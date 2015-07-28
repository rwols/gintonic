#include "lights.hpp"
#include "shaders.hpp"
#include "renderer.hpp"

namespace gintonic {

light::light(const vec3f& intensity)
: intensity(intensity)
{
	/* Empty on purpose. */
}

void light::shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_lp_null_shader();
	s.activate();
	s.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	s.set_viewport_size(vec2f(renderer::width(), renderer::height()));
	renderer::get_unit_quad_P().draw();
}

directional_light::directional_light(const vec3f& intensity)
: light(intensity)
{
	/* Empty on purpose. */
}

void directional_light::shine(const sqt_transformf& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_lp_directional_shader();
	s.activate();
	s.set_gbuffer_position(renderer::GBUFFER_POSITION);
	s.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	s.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	s.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	s.set_light_intensity(intensity);
	s.set_light_direction(renderer::matrix_VM().upper_left_33() * t.rotation.direction());
	renderer::get_unit_quad_P().draw();

	// m_program->activate();
	// m_program->set_gbuffer_diffuse(1);
	// m_program->set_gbuffer_normal(2);
	// m_program->set_matrix_PVM(mat4f(1));
	// m_program->set_light_intensity(intensity);
	// m_program->set_light_direction(renderer::matrix_VM().upper_left_33() * t.rotation.direction());
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

void point_light::set_attenuation(const vec3f& att) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_attenuation = att;
	m_cutoff_point = (-att[1] + std::sqrt(att[1] * att[1] - 256.0f * att[0] * att[2])) / (2 * att[2]);
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
	const auto& s = renderer::get_lp_point_shader();
	s.activate();
	s.set_gbuffer_position(renderer::GBUFFER_POSITION);
	s.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	s.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	s.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	s.set_light_intensity(intensity);
	s.set_light_position(t.translation);
	s.set_light_attenuation(m_attenuation);

	sqt_transformf sphere_transform;
	sphere_transform.scale = m_cutoff_point;
	sphere_transform.translation = t.translation;

	renderer::set_model_matrix(sphere_transform.get_matrix());

	s.set_matrix_PVM(renderer::matrix_PVM());

	renderer::get_unit_sphere_P().draw();
}

} // namespace gintonic