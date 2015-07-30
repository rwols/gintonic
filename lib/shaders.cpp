#include "shaders.hpp"

namespace gintonic {

matrix_PVM_shader::matrix_PVM_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: opengl::shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_PVM = get_uniform_location("matrix_PVM");
}

matrix_PVM_shader::matrix_PVM_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: opengl::shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_PVM = get_uniform_location("matrix_PVM");
}

void matrix_PVM_shader::set_matrix_PVM(const mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_matrix_PVM, m);
}

matrix_PVM_VM_shader::matrix_PVM_VM_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_VM = get_uniform_location("matrix_VM");
}

matrix_PVM_VM_shader::matrix_PVM_VM_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_VM = get_uniform_location("matrix_VM");
}

void matrix_PVM_VM_shader::set_matrix_VM(const mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_matrix_VM, m);
}

matrix_PVM_VM_N_shader::matrix_PVM_VM_N_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_N = get_uniform_location("matrix_N");
}

matrix_PVM_VM_N_shader::matrix_PVM_VM_N_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_N = get_uniform_location("matrix_N");
}

void matrix_PVM_VM_N_shader::set_matrix_N(const mat3f& m) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_matrix_N, m);
}

geometry_null_shader::geometry_null_shader()
: matrix_PVM_VM_N_shader("../s/gp_null.vs", "../s/gp_null.fs")
{
	
}

gp_c_shader::gp_c_shader()
: matrix_PVM_VM_N_shader("../s/gp_c.vs", "../s/gp_c.fs")
{
	loc_color = get_uniform_location("material.color");
}

gp_c_shader::gp_c_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_N_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_color = get_uniform_location("material.color");
}
gp_c_shader::gp_c_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_N_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_color = get_uniform_location("material.color");
}

void gp_c_shader::set_color(const vec4f& color) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_color, color);
}

gp_cd_shader::gp_cd_shader()
: gp_c_shader("../s/gp_cd.vs", "../s/gp_cd.fs")
{
	loc_diffuse = get_uniform_location("material.diffuse");
}

void gp_cd_shader::set_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_diffuse, texture_unit);
}

gp_cd_shader::gp_cd_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_c_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_diffuse = get_uniform_location("material.diffuse");
}

gp_cd_shader::gp_cd_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_c_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_diffuse = get_uniform_location("material.diffuse");
}

gp_cds_shader::gp_cds_shader()
: gp_cd_shader("../s/gp_cds.vs", "../s/gp_cds.fs")
{
	loc_specular = get_uniform_location("material.specular");
}

void gp_cds_shader::set_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_specular, texture_unit);
}

gp_cds_shader::gp_cds_shader(
	boost::filesystem::path vertex_shader,
	boost::filesystem::path fragment_shader)
: gp_cd_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_specular = get_uniform_location("material.specular");
}

gp_cds_shader::gp_cds_shader(
	boost::filesystem::path vertex_shader,
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_cd_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_specular = get_uniform_location("material.specular");
}


gp_cdn_shader::gp_cdn_shader()
: matrix_PVM_shader("../s/gp_cdn.vs", "../s/gp_cdn.fs")
{
	loc_color = get_uniform_location("material.color");
	loc_diffuse = get_uniform_location("material.diffuse");
	loc_normal = get_uniform_location("material.normal");
}

void gp_cdn_shader::set_color(const vec4f& color) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_color, color);
}

void gp_cdn_shader::set_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_diffuse, texture_unit);
}

void gp_cdn_shader::set_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_normal, texture_unit);
}

gp_cdn_shader::gp_cdn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_color = get_uniform_location("material.color");
	loc_diffuse = get_uniform_location("material.diffuse");
	loc_normal = get_uniform_location("material.normal");
}

gp_cdn_shader::gp_cdn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_color = get_uniform_location("material.color");
	loc_diffuse = get_uniform_location("material.diffuse");
	loc_normal = get_uniform_location("material.normal");
}

geometry_pass_shader::geometry_pass_shader()
: matrix_PVM_VM_N_shader("../s/geometry_pass.vs", "../s/geometry_pass.fs")
{
	loc_diffuse_color = get_uniform_location("material.diffuse_color");
	loc_diffuse = get_uniform_location("material.diffuse");
	loc_diffuse_factor = get_uniform_location("material.diffuse_factor");
}

void geometry_pass_shader::set_diffuse_color(const vec4f& diffuse_color) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_diffuse_color, diffuse_color);
}

void geometry_pass_shader::set_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_diffuse, texture_unit);
}

void geometry_pass_shader::set_diffuse_factor(const GLfloat factor) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_diffuse_factor, factor);
}

lp_null_shader::lp_null_shader()
: opengl::shader("../s/lp_null.vs", "../s/lp_null.fs")
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
}

void lp_null_shader::set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_diffuse, texture_unit);
}

void lp_null_shader::set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_viewport_size, size);
}

lp_null_shader::lp_null_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: opengl::shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
}

lp_null_shader::lp_null_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: opengl::shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
}

lp_directional_shader::lp_directional_shader()
: lp_null_shader("../s/lp_directional.vs", "../s/lp_directional.fs")
{
	// loc_gbuffer_position = get_uniform_location("gbuffer.position"); // NOTE: ONLY TEMPORARY
	// loc_gbuffer_specular = get_uniform_location("gbuffer.specular"); // NOTE: ONLY TEMPORARY
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
	loc_light_intensity = get_uniform_location("light.intensity");
	loc_light_direction = get_uniform_location("light.direction");
}

// void lp_directional_shader::set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	set_uniform(loc_gbuffer_position, texture_unit); // NOTE: ONLY TEMPORARY
// }

// void lp_directional_shader::set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	set_uniform(loc_gbuffer_specular, texture_unit); // NOTE: ONLY TEMPORARY
// }

void lp_directional_shader::set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_normal, texture_unit);
}

void lp_directional_shader::set_light_intensity(const vec3f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_intensity, intensity);
}

void lp_directional_shader::set_light_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_direction, direction);
}

lp_directional_shader::lp_directional_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: lp_null_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	// loc_gbuffer_position = get_uniform_location("gbuffer.position"); // NOTE: ONLY TEMPORARY
	// loc_gbuffer_specular = get_uniform_location("gbuffer.specular"); // NOTE: ONLY TEMPORARY
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
	loc_light_intensity = get_uniform_location("light.intensity");
	loc_light_direction = get_uniform_location("light.direction");
}

lp_directional_shader::lp_directional_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: lp_null_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	// loc_gbuffer_position = get_uniform_location("gbuffer.position"); // NOTE: ONLY TEMPORARY
	// loc_gbuffer_specular = get_uniform_location("gbuffer.specular"); // NOTE: ONLY TEMPORARY
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
	loc_light_intensity = get_uniform_location("light.intensity");
	loc_light_direction = get_uniform_location("light.direction");
}


lp_point_shader::lp_point_shader()
: matrix_PVM_shader("../s/lp_point.vs", "../s/lp_point.fs")
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_gbuffer_position = get_uniform_location("gbuffer.position");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
	// loc_gbuffer_specular = get_uniform_location("gbuffer.specular"); // NOTE: ONLY TEMPORARY
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
	loc_light_intensity = get_uniform_location("light.intensity");
	loc_light_position = get_uniform_location("light.position");
	loc_light_attenuation = get_uniform_location("light.attenuation");
}

void lp_point_shader::set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_viewport_size, size);
}

void lp_point_shader::set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_position, texture_unit);
}

void lp_point_shader::set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_diffuse, texture_unit);
}

// void lp_point_shader::set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	set_uniform(loc_gbuffer_specular, texture_unit);
// }

void lp_point_shader::set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_normal, texture_unit);
}

void lp_point_shader::set_light_intensity(const vec3f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_intensity, intensity);
}

void lp_point_shader::set_light_position(const vec3f& position) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_position, position);
}

void lp_point_shader::set_light_attenuation(const vec3f& attenuation) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_attenuation, attenuation);
}

lp_point_shader::lp_point_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_gbuffer_position = get_uniform_location("gbuffer.position");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
	// loc_gbuffer_specular = get_uniform_location("gbuffer.specular"); // NOTE: ONLY TEMPORARY
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
	loc_light_intensity = get_uniform_location("light.intensity");
	loc_light_position = get_uniform_location("light.position");
	loc_light_attenuation = get_uniform_location("light.attenuation");
}
lp_point_shader::lp_point_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_gbuffer_position = get_uniform_location("gbuffer.position");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
	// loc_gbuffer_specular = get_uniform_location("gbuffer.specular"); // NOTE: ONLY TEMPORARY
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
	loc_light_intensity = get_uniform_location("light.intensity");
	loc_light_position = get_uniform_location("light.position");
	loc_light_attenuation = get_uniform_location("light.attenuation");
}

light_pass_shader::light_pass_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_light_intensity = get_uniform_location("light.intensity");

}

light_pass_shader::light_pass_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_viewport_size = get_uniform_location("viewport_size");
	loc_light_intensity = get_uniform_location("light.intensity");
}

void light_pass_shader::set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_viewport_size, size);
}

void light_pass_shader::set_light_intensity(const vec3f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_intensity, intensity);
}

directional_light_pass_shader::directional_light_pass_shader()
: light_pass_shader("../s/light_pass.vs", "../s/directional_light_pass.fs")
{
	loc_light_direction = get_uniform_location("light.direction");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
}

void directional_light_pass_shader::set_light_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_direction, direction);
}

void directional_light_pass_shader::set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_diffuse, texture_unit);
}

void directional_light_pass_shader::set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_normal, texture_unit);
}

point_light_pass_shader::point_light_pass_shader()
: light_pass_shader("../s/light_pass.vs", "../s/point_light_pass.fs")
{
	loc_light_position = get_uniform_location("light.position");
	loc_light_attenuation = get_uniform_location("light.attenuation");
	loc_gbuffer_position = get_uniform_location("gbuffer.position");
	loc_gbuffer_diffuse = get_uniform_location("gbuffer.diffuse");
	loc_gbuffer_normal = get_uniform_location("gbuffer.normal");
}

void point_light_pass_shader::set_light_position(const vec3f& position) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_position, position);
}

void point_light_pass_shader::set_light_attenuation(const vec3f& attenuation) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_light_attenuation, attenuation);
}

void point_light_pass_shader::set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_position, texture_unit);
}

void point_light_pass_shader::set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_diffuse, texture_unit);
}

void point_light_pass_shader::set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_gbuffer_normal, texture_unit);
}

text_shader::text_shader()
: opengl::shader("../s/flat_text_uniform_color.vs", "../s/flat_text_uniform_color.fs")
{
	loc_color = get_uniform_location("color");
	loc_texture = get_uniform_location("tex");
}

void text_shader::set_color(const vec3f& color) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_color, color);
}

void text_shader::set_texture(const GLint tex) const BOOST_NOEXCEPT_OR_NOTHROW
{
	set_uniform(loc_texture, tex);
}

} // namespace gintonic