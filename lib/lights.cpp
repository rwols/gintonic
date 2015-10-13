#include "lights.hpp"
#include "shaders.hpp"
#include "basic_shapes.hpp"
#include "entity.hpp"
#include "renderer.hpp"

#ifdef ENABLE_DEBUG_TRACE
	#include "fonts.hpp"
#endif

namespace gintonic {

void light::attach(entity& e)
{
	if (e.m_light_component == this) return;
	else if (e.m_light_component) e.m_light_component->detach(e);
	e.m_light_component = this;
	m_ents.push_back(&e);
}

void light::detach(entity& e)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (*i == &e)
		{
			e.m_light_component = nullptr;
			m_ents.erase(i);
			return;
		}
	}
}

light::light(const vec4f& intensity)
: intensity(intensity)
{
	/* Empty on purpose. */
}

light::~light() noexcept
{
	for (auto* e : m_ents) e->m_light_component = nullptr;
}

void light::set_brightness(const float brightness)
{
	intensity.w = brightness;
}

float light::brightness() const noexcept
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

std::ostream& light::pretty_print(std::ostream& os) const noexcept
{
	return os << "{ (light) intensity: " << intensity << " }";
}

ambient_light::ambient_light(const vec4f& intensity)
: light(intensity)
{
	/* Empty on purpose. */
}

ambient_light::~ambient_light() noexcept
{
	/* Empty on purpose. */
}

void ambient_light::shine(const entity& e) const noexcept
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

std::ostream& ambient_light::pretty_print(std::ostream& os) const noexcept
{
	return os << "{ (ambient_light) intensity: " << intensity << " }";
}

directional_light::directional_light(const vec4f& intensity)
: ambient_light(intensity)
{
	/* Empty on purpose. */
}

directional_light::~directional_light() noexcept
{
	/* Empty on purpose. */
}

void directional_light::shine(const entity& e) const noexcept
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

	const auto light_dir = renderer::matrix_V() * (e.global_transform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f));
	
	s.set_light_direction(vec3f(light_dir.data));

	renderer::get_unit_quad_P().draw();
}

std::ostream& operator << (std::ostream& os, const directional_light& l)
{
	return l.pretty_print(os);
}

std::ostream& directional_light::pretty_print(std::ostream& os) const noexcept
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

point_light::~point_light() noexcept
{
	/* Empty on purpose. */
}

void point_light::set_attenuation(const vec4f& att) noexcept
{
	m_attenuation = att;
	calculate_cutoff_radius();
}

vec4f point_light::attenuation() const noexcept
{
	return m_attenuation;
}

float point_light::cutoff_point() const noexcept
{
	return m_cutoff_point;
}

void point_light::shine(const entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = m_cutoff_point;
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

void point_light::calculate_cutoff_radius() noexcept
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

std::ostream& point_light::pretty_print(std::ostream& os) const noexcept
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

spot_light::~spot_light() noexcept
{
	 /* Empty on purpose. */
}

void spot_light::shine(const entity& e) const noexcept
{
	// The transformation data is delivered in WORLD coordinates.

	SQT sphere_transform;
	sphere_transform.scale = cutoff_point();
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

std::ostream& operator << (std::ostream& os, const spot_light& l)
{
	return l.pretty_print(os);
}

std::ostream& spot_light::pretty_print(std::ostream& os) const noexcept
{
	return os << "{ (spot_light) intensity: " << intensity
		<< ", attenuation: " << attenuation()
		<< ", cutoff_point: " << cutoff_point() << " }";
}

} // namespace gintonic