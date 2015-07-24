#include "lights.hpp"
#include "shaders.hpp"
#include "renderer.hpp"

namespace gintonic {

light::light(const vec3f& intensity)
: intensity(intensity)
{
	/* Empty on purpose. */
}

directional_light::directional_light(std::shared_ptr<directional_light_pass_shader> prog)
: m_program(prog)
{
	/* Empty on purpose. */
}

directional_light::directional_light(
	std::shared_ptr<directional_light_pass_shader> prog,
	const vec3f& intensity)
: light(intensity)
, m_program(prog)
{
	/* Empty on purpose. */
}

void directional_light::shine(const sqt_transformf& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	m_program->activate();
	m_program->set_gbuffer_diffuse(1);
	m_program->set_gbuffer_normal(2);
	m_program->set_matrix_PVM(mat4f(1));
	m_program->set_light_intensity(intensity);
	m_program->set_light_direction(renderer::matrix_VM().upper_left_33() * t.rotation.direction());
}

point_light::point_light(std::shared_ptr<point_light_pass_shader> prog)
: m_program(prog)
{
	/* Empty on purpose. */
}

point_light::point_light(
	std::shared_ptr<point_light_pass_shader> prog,
	const vec3f& intensity)
: light(intensity)
, m_program(prog)
{
	/* Empty on purpose. */
}

point_light::point_light(
	std::shared_ptr<point_light_pass_shader> prog,
	const vec3f& intensity,
	const vec3f& attenuation)
: light(intensity)
, m_program(prog)
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
	m_program->activate();
	m_program->set_gbuffer_position(0);
	m_program->set_gbuffer_diffuse(1);
	m_program->set_gbuffer_normal(2);
	m_program->set_light_intensity(intensity);
	m_program->set_light_position(t.translation);
	m_program->set_light_attenuation(m_attenuation);
}

} // namespace gintonic