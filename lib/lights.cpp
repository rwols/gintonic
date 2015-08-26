#include "lights.hpp"
#include "shaders.hpp"
#include "basic_shapes.hpp"
#include "entity.hpp"
#include "renderer.hpp"

#ifdef ENABLE_DEBUG_TRACE
	#include "fonts.hpp"
#endif

namespace gintonic {

light::light(const vec4f& intensity)
: intensity(intensity)
{
	/* Empty on purpose. */
}

light::~light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void light::set_brightness(const float brightness)
{
	intensity.w = brightness;
}

float light::brightness() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return intensity.w;
}

std::ostream& operator << (std::ostream& os, const light* l)
{
	return l->pretty_print(os);
}

std::ostream& operator << (std::ostream& os, const std::unique_ptr<light>& l)
{
	return l->pretty_print(os);
}

std::ostream& operator << (std::ostream& os, const std::shared_ptr<light>& l)
{
	return l->pretty_print(os);
}

std::ostream& operator << (std::ostream& os, const light& l)
{
	return l.pretty_print(os);
}

std::ostream& light::pretty_print(std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << "{ (light) intensity: " << intensity << " }";
}

ambient_light::ambient_light(const vec4f& intensity)
: light(intensity)
{
	/* Empty on purpose. */
}

ambient_light::~ambient_light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void ambient_light::shine(const SQT& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_lp_ambient_shader();
	s.activate();
	s.set_viewport_size(renderer::viewport_size());
	s.set_light_intensity(intensity);
	renderer::get_unit_quad_P().draw();
}

std::ostream& operator << (std::ostream& os, const ambient_light& l)
{
	return l.pretty_print(os);
}

std::ostream& ambient_light::pretty_print(std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << "{ (ambient_light) intensity: " << intensity << " }";
}

directional_light::directional_light(const vec4f& intensity)
: ambient_light(intensity)
{
	/* Empty on purpose. */
}

directional_light::~directional_light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void directional_light::shine(const SQT& t) const BOOST_NOEXCEPT_OR_NOTHROW
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

	// These uniforms are different for each directional_light.
	s.set_light_intensity(intensity);

	// const auto rot = renderer::camera().global_transform().apply_to_direction(t.rotation.direction());
	const auto rot = renderer::matrix_V() * vec4f(t.rotation.direction(), 0.0f);
	
	s.set_light_direction(vec3f(rot.x, rot.y, rot.z));

	renderer::get_unit_quad_P().draw();
}

std::ostream& operator << (std::ostream& os, const directional_light& l)
{
	return l.pretty_print(os);
}

std::ostream& directional_light::pretty_print(std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << "{ (directional_light) intensity: " << intensity << " }";
}

point_light::point_light(const vec4f& intensity)
: light(intensity)
{
	/* Empty on purpose. */
}

point_light::point_light(const vec4f& intensity, const vec4f& attenuation)
: light(intensity)
{
	set_attenuation(attenuation);
}

point_light::~point_light() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void point_light::set_attenuation(const vec4f& att) BOOST_NOEXCEPT_OR_NOTHROW
{
	m_attenuation = att;
	calculate_cutoff_radius();
}

vec4f point_light::attenuation() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return m_attenuation;
}

float point_light::cutoff_point() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return m_cutoff_point;
}

void point_light::shine(const SQT& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = m_cutoff_point;
	sphere_transform.translation = t.translation;

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

	const auto light_pos = renderer::camera().global_transform().apply_to_point(t.translation);

	renderer::begin_light_pass();

	pointshader.activate();
	pointshader.set_viewport_size(renderer::viewport_size());
	pointshader.set_gbuffer_position(renderer::GBUFFER_POSITION);
	pointshader.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	pointshader.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	pointshader.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	pointshader.set_light_intensity(intensity);
	pointshader.set_light_position(light_pos);
	pointshader.set_light_attenuation(m_attenuation);
	pointshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	glDisable(GL_CULL_FACE);
}

void point_light::set_brightness(const float brightness)
{
	intensity.w = brightness;
	calculate_cutoff_radius();
}

void point_light::calculate_cutoff_radius() BOOST_NOEXCEPT_OR_NOTHROW
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

	#define att m_attenuation
	#define in intensity

	const float c = in.w * std::max(in.x, std::max(in.x, in.y));

	m_cutoff_point = (-att.x + std::sqrt(att.x * att.x 
		- 4.0f * att.y * (att.x - 256.0f * c))) 
	/ (2 * att.y);

	#undef in
	#undef att

	// #ifdef ENABLE_DEBUG_TRACE
	// renderer::cerr() << "Cutoff point was set to " << m_cutoff_point << '\n';
	// #endif
}

std::ostream& operator << (std::ostream& os, const point_light& l)
{
	return l.pretty_print(os);
}

std::ostream& point_light::pretty_print(std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << "{ (point_light) intensity: " << intensity
		<< ", attenuation: " << m_attenuation
		<< ", cutoff_point: " << m_cutoff_point << " }";
}

spot_light::spot_light(const vec4f& intensity)
: point_light(intensity)
{
	/* Empty on purpose. */
}

spot_light::spot_light(const vec4f& intensity, const vec4f& attenuation)
: point_light(intensity, attenuation)
{
	/* Empty on purpose. */
}

spot_light::~spot_light() BOOST_NOEXCEPT_OR_NOTHROW
{
	 /* Empty on purpose. */
}

void spot_light::shine(const SQT& t) const BOOST_NOEXCEPT_OR_NOTHROW
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = cutoff_point();
	// sphere_transform.rotation = t.rotation;
	sphere_transform.translation = t.translation;

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

	const auto light_pos = renderer::camera().global_transform().apply_to_point(t.translation);
	const auto light_dir = (renderer::camera().global_transform().rotation * t.rotation).direction();

	renderer::begin_light_pass();

	spotshader.activate();
	spotshader.set_viewport_size(renderer::viewport_size());
	spotshader.set_gbuffer_position(renderer::GBUFFER_POSITION);
	spotshader.set_gbuffer_diffuse(renderer::GBUFFER_DIFFUSE);
	spotshader.set_gbuffer_specular(renderer::GBUFFER_SPECULAR);
	spotshader.set_gbuffer_normal(renderer::GBUFFER_NORMAL);
	spotshader.set_light_intensity(intensity);
	spotshader.set_light_position(light_pos);
	spotshader.set_light_direction(light_dir);
	spotshader.set_light_attenuation(attenuation());
	spotshader.set_matrix_PVM(renderer::matrix_PVM());
	sphere.draw();

	glDisable(GL_CULL_FACE);
}

std::ostream& operator << (std::ostream& os, const spot_light& l)
{
	return l.pretty_print(os);
}

std::ostream& spot_light::pretty_print(std::ostream& os) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return os << "{ (spot_light) intensity: " << intensity
		<< ", attenuation: " << attenuation()
		<< ", cutoff_point: " << cutoff_point() << " }";
}

} // namespace gintonic