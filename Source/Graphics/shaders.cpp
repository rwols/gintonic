#include "shaders.hpp"

namespace gintonic {

/*****************************************************************************
 * gintonic::matrix_PVM_shader                                               *
 ****************************************************************************/

matrix_PVM_shader::matrix_PVM_shader()
: OpenGL::ShaderProgram("../Shaders/lp_pvm.vs", "../Shaders/null.fs")
{
	loc_matrix_PVM = getUniformLocation("matrix_PVM");
}

matrix_PVM_shader::matrix_PVM_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_PVM = getUniformLocation("matrix_PVM");
}

matrix_PVM_shader::matrix_PVM_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_PVM = getUniformLocation("matrix_PVM");
}

matrix_PVM_shader::~matrix_PVM_shader() noexcept
{
	/* Empty on purpose. */
}

void matrix_PVM_shader::set_matrix_PVM(const mat4f& m) const noexcept
{
	setUniform(loc_matrix_PVM, m);
}

/*****************************************************************************
 * gintonic::matrix_PVM_VM_shader                                            *
 ****************************************************************************/

matrix_PVM_VM_shader::matrix_PVM_VM_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_VM = getUniformLocation("matrix_VM");
}

matrix_PVM_VM_shader::matrix_PVM_VM_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_VM = getUniformLocation("matrix_VM");
}

matrix_PVM_VM_shader::~matrix_PVM_VM_shader() noexcept
{
	/* Empty on purpose. */
}

void matrix_PVM_VM_shader::set_matrix_VM(const mat4f& m) const noexcept
{
	setUniform(loc_matrix_VM, m);
}

/*****************************************************************************
 * gintonic::matrix_PVM_VM_N_shader                                          *
 ****************************************************************************/

matrix_PVM_VM_N_shader::matrix_PVM_VM_N_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_N = getUniformLocation("matrix_N");
}

matrix_PVM_VM_N_shader::matrix_PVM_VM_N_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_N = getUniformLocation("matrix_N");
}

matrix_PVM_VM_N_shader::~matrix_PVM_VM_N_shader() noexcept
{
	/* Empty on purpose. */
}

void matrix_PVM_VM_N_shader::set_matrix_N(const mat3f& m) const noexcept
{
	setUniform(loc_matrix_N, m);
}

/*****************************************************************************
 * gintonic::gp_shader                                                       *
 ****************************************************************************/

gp_shader::gp_shader()
: matrix_PVM_VM_N_shader("../Shaders/gp.vs", "../Shaders/gp.fs")
{
	loc_diffuse_color = getUniformLocation("material.diffuse_color");
	loc_specular_color = getUniformLocation("material.specular_color");
}
gp_shader::~gp_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_shader::set_diffuse_color(const vec4f& color) const noexcept
{
	setUniform(loc_diffuse_color, color);
}

void gp_shader::set_specular_color(const vec4f& color) const noexcept
{
	setUniform(loc_specular_color, color);
}

gp_shader::gp_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_N_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_diffuse_color = getUniformLocation("material.diffuse_color");
	loc_specular_color = getUniformLocation("material.specular_color");
}

gp_shader::gp_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_VM_N_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_diffuse_color = getUniformLocation("material.diffuse_color");
	loc_specular_color = getUniformLocation("material.specular_color");
}

/*****************************************************************************
 * gintonic::gp_d_shader                                                     *
 ****************************************************************************/

gp_d_shader::gp_d_shader()
: gp_shader("../Shaders/gp.vs", "../Shaders/gp_d.fs")
{
	loc_diffuse_texture = getUniformLocation("material.diffuse_texture");
}

gp_d_shader::~gp_d_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_d_shader::set_diffuse_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_diffuse_texture, texture_unit);
}

gp_d_shader::gp_d_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_diffuse_texture = getUniformLocation("material.diffuse_texture");
}
gp_d_shader::gp_d_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_diffuse_texture = getUniformLocation("material.diffuse_texture");
}

/*****************************************************************************
 * gintonic::gp_s_shader                                                     *
 ****************************************************************************/

gp_s_shader::gp_s_shader()
: gp_shader("../Shaders/gp.vs", "../Shaders/gp_s.fs")
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gp_s_shader::~gp_s_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_s_shader::set_specular_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_specular_texture, texture_unit);
}

gp_s_shader::gp_s_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gp_s_shader::gp_s_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

/*****************************************************************************
 * gintonic::gp_n_shader                                                     *
 ****************************************************************************/

gp_n_shader::gp_n_shader()
: gp_shader("../Shaders/gp_n.vs", "../Shaders/gp_n.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_n_shader::~gp_n_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_n_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gp_n_shader::gp_n_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_n_shader::gp_n_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gp_ds_shader                                                    *
 ****************************************************************************/

gp_ds_shader::gp_ds_shader()
: gp_d_shader("../Shaders/gp.vs", "../Shaders/gp_ds.fs")
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gp_ds_shader::~gp_ds_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_ds_shader::set_specular_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_specular_texture, texture_unit);
}

gp_ds_shader::gp_ds_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_d_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gp_ds_shader::gp_ds_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_d_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

/*****************************************************************************
 * gintonic::gp_dn_shader                                                    *
 ****************************************************************************/

gp_dn_shader::gp_dn_shader()
: gp_d_shader("../Shaders/gp_n.vs", "../Shaders/gp_dn.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_dn_shader::~gp_dn_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_dn_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gp_dn_shader::gp_dn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_d_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_dn_shader::gp_dn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_d_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gp_sn_shader                                                    *
 ****************************************************************************/

gp_sn_shader::gp_sn_shader()
: gp_s_shader("../Shaders/gp_n.vs", "../Shaders/gp_sn.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_sn_shader::~gp_sn_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_sn_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gp_sn_shader::gp_sn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gp_s_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_sn_shader::gp_sn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_s_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gp_dsn_shader                                                   *
 ****************************************************************************/

gp_dsn_shader::gp_dsn_shader()
: gp_ds_shader("../Shaders/gp_n.vs", "../Shaders/gp_dsn.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_dsn_shader::~gp_dsn_shader() noexcept
{
	/* Empty on purpose. */
}

void gp_dsn_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gp_dsn_shader::gp_dsn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gp_dsn_shader::gp_dsn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gp_ds_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gpi_shader                                                      *
 ****************************************************************************/

gpi_shader::gpi_shader()
: OpenGL::ShaderProgram("../Shaders/gpi.vs", "../Shaders/gp.fs")
{
	loc_diffuse_color = getUniformLocation("material.diffuse_color");
	loc_specular_color = getUniformLocation("material.specular_color");
}
gpi_shader::~gpi_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_shader::set_diffuse_color(const vec4f& color) const noexcept
{
	setUniform(loc_diffuse_color, color);
}

void gpi_shader::set_specular_color(const vec4f& color) const noexcept
{
	setUniform(loc_specular_color, color);
}

gpi_shader::gpi_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_diffuse_color = getUniformLocation("material.diffuse_color");
	loc_specular_color = getUniformLocation("material.specular_color");
}

gpi_shader::gpi_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_diffuse_color = getUniformLocation("material.diffuse_color");
	loc_specular_color = getUniformLocation("material.specular_color");
}

/*****************************************************************************
 * gintonic::gpi_d_shader                                                    *
 ****************************************************************************/

gpi_d_shader::gpi_d_shader()
: gpi_shader("../Shaders/gpi.vs", "../Shaders/gp_d.fs")
{
	loc_diffuse_texture = getUniformLocation("material.diffuse_texture");
}

gpi_d_shader::~gpi_d_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_d_shader::set_diffuse_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_diffuse_texture, texture_unit);
}

gpi_d_shader::gpi_d_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gpi_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_diffuse_texture = getUniformLocation("material.diffuse_texture");
}
gpi_d_shader::gpi_d_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_diffuse_texture = getUniformLocation("material.diffuse_texture");
}

/*****************************************************************************
 * gintonic::gpi_s_shader                                                    *
 ****************************************************************************/

gpi_s_shader::gpi_s_shader()
: gpi_shader("../Shaders/gpi.vs", "../Shaders/gp_s.fs")
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gpi_s_shader::~gpi_s_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_s_shader::set_specular_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_specular_texture, texture_unit);
}

gpi_s_shader::gpi_s_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gpi_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gpi_s_shader::gpi_s_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

/*****************************************************************************
 * gintonic::gpi_n_shader                                                    *
 ****************************************************************************/

gpi_n_shader::gpi_n_shader()
: gpi_shader("../Shaders/gpi_n.vs", "../Shaders/gp_n.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_n_shader::~gpi_n_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_n_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gpi_n_shader::gpi_n_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gpi_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_n_shader::gpi_n_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gpi_ds_shader                                                   *
 ****************************************************************************/

gpi_ds_shader::gpi_ds_shader()
: gpi_d_shader("../Shaders/gpi.vs", "../Shaders/gp_ds.fs")
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gpi_ds_shader::~gpi_ds_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_ds_shader::set_specular_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_specular_texture, texture_unit);
}

gpi_ds_shader::gpi_ds_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gpi_d_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

gpi_ds_shader::gpi_ds_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_d_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_specular_texture = getUniformLocation("material.specular_texture");
}

/*****************************************************************************
 * gintonic::gpi_dn_shader                                                   *
 ****************************************************************************/

gpi_dn_shader::gpi_dn_shader()
: gpi_d_shader("../Shaders/gpi_n.vs", "../Shaders/gp_dn.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_dn_shader::~gpi_dn_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_dn_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gpi_dn_shader::gpi_dn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gpi_d_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_dn_shader::gpi_dn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_d_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gpi_sn_shader                                                   *
 ****************************************************************************/

gpi_sn_shader::gpi_sn_shader()
: gpi_s_shader("../Shaders/gpi_n.vs", "../Shaders/gp_sn.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_sn_shader::~gpi_sn_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_sn_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gpi_sn_shader::gpi_sn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: gpi_s_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_sn_shader::gpi_sn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_s_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::gpi_dsn_shader                                                  *
 ****************************************************************************/

gpi_dsn_shader::gpi_dsn_shader()
: gpi_ds_shader("../Shaders/gpi_n.vs", "../Shaders/gp_dsn.fs")
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_dsn_shader::~gpi_dsn_shader() noexcept
{
	/* Empty on purpose. */
}

void gpi_dsn_shader::set_normal_texture(const GLint texture_unit) const noexcept
{
	setUniform(loc_normal_texture, texture_unit);
}

gpi_dsn_shader::gpi_dsn_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

gpi_dsn_shader::gpi_dsn_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: gpi_ds_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_normal_texture = getUniformLocation("material.normal_texture");
}

/*****************************************************************************
 * gintonic::lp_null_shader                                                  *
 ****************************************************************************/

lp_null_shader::lp_null_shader()
: OpenGL::ShaderProgram("../Shaders/lp_null.vs", "../Shaders/lp_null.fs")
{
	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
}

lp_null_shader::~lp_null_shader() noexcept
{
	/* Empty on purpose. */
}

void lp_null_shader::set_gbuffer_diffuse(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_diffuse, texture_unit);
}

void lp_null_shader::set_viewport_size(const vec2f& size) const noexcept
{
	setUniform(loc_viewport_size, size);
}

lp_null_shader::lp_null_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
}

lp_null_shader::lp_null_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
}

/*****************************************************************************
 * gintonic::lp_ambient_shader                                               *
 ****************************************************************************/

lp_ambient_shader::lp_ambient_shader()
: lp_null_shader("../Shaders/lp.vs", "../Shaders/lp_ambient.fs")
{
	loc_light_intensity = getUniformLocation("light.intensity");
}

lp_ambient_shader::~lp_ambient_shader() noexcept
{
	/* Empty on purpose. */
}

void lp_ambient_shader::set_light_intensity(const vec4f& intensity) const noexcept
{
	setUniform(loc_light_intensity, intensity);
}

lp_ambient_shader::lp_ambient_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: lp_null_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_light_intensity = getUniformLocation("light.intensity");
}

lp_ambient_shader::lp_ambient_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: lp_null_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_light_intensity = getUniformLocation("light.intensity");
}

/*****************************************************************************
 * gintonic::lp_directional_shader                                           *
 ****************************************************************************/

lp_directional_shader::lp_directional_shader()
: lp_null_shader("../Shaders/lp.vs", "../Shaders/lp_directional.fs")
{
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_direction = getUniformLocation("light.direction");
}

lp_directional_shader::~lp_directional_shader() noexcept
{
	/* Empty on purpose. */
}

void lp_directional_shader::set_gbuffer_position(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_position, texture_unit);
}

void lp_directional_shader::set_gbuffer_specular(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_specular, texture_unit);
}

void lp_directional_shader::set_gbuffer_normal(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_normal, texture_unit);
}

void lp_directional_shader::set_light_intensity(const vec4f& intensity) const noexcept
{
	setUniform(loc_light_intensity, intensity);
}

void lp_directional_shader::set_light_direction(const vec3f& direction) const noexcept
{
	setUniform(loc_light_direction, direction);
}

lp_directional_shader::lp_directional_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: lp_null_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_direction = getUniformLocation("light.direction");
}

lp_directional_shader::lp_directional_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: lp_null_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_direction = getUniformLocation("light.direction");
}

/*****************************************************************************
 * gintonic::lp_point_shader                                                 *
 ****************************************************************************/

lp_point_shader::lp_point_shader()
: matrix_PVM_shader("../Shaders/lp_pvm.vs", "../Shaders/lp_point.fs")
{
	#ifndef NDEBUG
	loc_debugflag = getUniformLocation("debugflag");
	#endif

	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_position = getUniformLocation("light.position");
	loc_light_attenuation = getUniformLocation("light.attenuation");
}

lp_point_shader::~lp_point_shader() noexcept
{
	/* Empty on purpose. */
}

#ifndef NDEBUG
void lp_point_shader::set_debugflag(const GLint value) const noexcept
{
	setUniform(loc_debugflag, value);
}
#endif

void lp_point_shader::set_viewport_size(const vec2f& size) const noexcept
{
	setUniform(loc_viewport_size, size);
}

void lp_point_shader::set_gbuffer_position(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_position, texture_unit);
}

void lp_point_shader::set_gbuffer_diffuse(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_diffuse, texture_unit);
}

void lp_point_shader::set_gbuffer_specular(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_specular, texture_unit);
}

void lp_point_shader::set_gbuffer_normal(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_normal, texture_unit);
}

void lp_point_shader::set_light_intensity(const vec4f& intensity) const noexcept
{
	setUniform(loc_light_intensity, intensity);
}

void lp_point_shader::set_light_position(const vec3f& position) const noexcept
{
	setUniform(loc_light_position, position);
}

void lp_point_shader::set_light_attenuation(const vec4f& attenuation) const noexcept
{
	setUniform(loc_light_attenuation, attenuation);
}

lp_point_shader::lp_point_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	#ifndef NDEBUG
	loc_debugflag = getUniformLocation("debugflag");
	#endif

	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_position = getUniformLocation("light.position");
	loc_light_attenuation = getUniformLocation("light.attenuation");
}

lp_point_shader::lp_point_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	#ifndef NDEBUG
	loc_debugflag = getUniformLocation("debugflag");
	#endif

	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_position = getUniformLocation("light.position");
	loc_light_attenuation = getUniformLocation("light.attenuation");
}

/*****************************************************************************
 * gintonic::lp_spot_shader                                                 *
 ****************************************************************************/

lp_spot_shader::lp_spot_shader()
: matrix_PVM_shader("../Shaders/lp_pvm.vs", "../Shaders/lp_spot.fs")
{
	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_position = getUniformLocation("light.position");
	loc_light_direction = getUniformLocation("light.direction");
	loc_light_attenuation = getUniformLocation("light.attenuation");
	loc_light_angle = getUniformLocation("light.angle");

	#ifndef NDEBUG
	loc_debugflag = getUniformLocation("debugflag");
	#endif
}

lp_spot_shader::~lp_spot_shader() noexcept
{
	/* Empty on purpose. */
}

void lp_spot_shader::set_viewport_size(const vec2f& size) const noexcept
{
	setUniform(loc_viewport_size, size);
}

void lp_spot_shader::set_gbuffer_position(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_position, texture_unit);
}

void lp_spot_shader::set_gbuffer_diffuse(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_diffuse, texture_unit);
}

void lp_spot_shader::set_gbuffer_specular(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_specular, texture_unit);
}

void lp_spot_shader::set_gbuffer_normal(const GLint texture_unit) const noexcept
{
	setUniform(loc_gbuffer_normal, texture_unit);
}

void lp_spot_shader::set_light_intensity(const vec4f& intensity) const noexcept
{
	setUniform(loc_light_intensity, intensity);
}

void lp_spot_shader::set_light_position(const vec3f& position) const noexcept
{
	setUniform(loc_light_position, position);
}

void lp_spot_shader::set_light_direction(const vec3f& direction) const noexcept
{
	setUniform(loc_light_direction, direction);
}

void lp_spot_shader::set_light_attenuation(const vec4f& attenuation) const noexcept
{
	setUniform(loc_light_attenuation, attenuation);
}

void lp_spot_shader::set_light_angle(const GLfloat angle) const noexcept
{
	setUniform(loc_light_angle, angle);
}

#ifndef NDEBUG
void lp_spot_shader::set_debugflag(const GLint value) const noexcept
{
	setUniform(loc_debugflag, value);
}
#endif

lp_spot_shader::lp_spot_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_position = getUniformLocation("light.position");
	loc_light_direction = getUniformLocation("light.direction");
	loc_light_attenuation = getUniformLocation("light.attenuation");
	loc_light_angle = getUniformLocation("light.angle");

	#ifndef NDEBUG
	loc_debugflag = getUniformLocation("debugflag");
	#endif
}

lp_spot_shader::lp_spot_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: matrix_PVM_shader(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_viewport_size = getUniformLocation("viewport_size");
	loc_gbuffer_position = getUniformLocation("gbuffer.position");
	loc_gbuffer_diffuse = getUniformLocation("gbuffer.diffuse");
	loc_gbuffer_specular = getUniformLocation("gbuffer.specular");
	loc_gbuffer_normal = getUniformLocation("gbuffer.normal");
	loc_light_intensity = getUniformLocation("light.intensity");
	loc_light_position = getUniformLocation("light.position");
	loc_light_direction = getUniformLocation("light.direction");
	loc_light_attenuation = getUniformLocation("light.attenuation");
	loc_light_angle = getUniformLocation("light.angle");

	#ifndef NDEBUG
	loc_debugflag = getUniformLocation("debugflag");
	#endif
}

/*****************************************************************************
 * gintonic::sp_directional_shader                                           *
 ****************************************************************************/

sp_directional_shader::sp_directional_shader()
: matrix_PVM_shader("../Shaders/lp_pvm.vs", "../Shaders/null.fs")
{
	/* Empty on purpose. */
}
sp_directional_shader::~sp_directional_shader() noexcept
{
	/* Empty on purpose. */
}

/*****************************************************************************
 * gintonic::skybox_shader                                                   *
 ****************************************************************************/

skybox_shader::skybox_shader()
: OpenGL::ShaderProgram("../Shaders/skybox.vs", "../Shaders/skybox.fs")
{
	loc_matrix_PV = getUniformLocation("matrix_PV");
	loc_skybox_diffuse = getUniformLocation("skybox.diffuse_texture");
}

skybox_shader::~skybox_shader() noexcept
{
	/* Empty on purpose. */
}

void skybox_shader::set_matrix_PV(const mat4f& matrix_PV) const noexcept
{
	setUniform(loc_matrix_PV, matrix_PV);
}

void skybox_shader::set_skybox_diffuse(const GLint texture_unit) const noexcept
{
	setUniform(loc_skybox_diffuse, texture_unit);
}

skybox_shader::skybox_shader(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	loc_matrix_PV = getUniformLocation("matrix_PV");
	loc_skybox_diffuse = getUniformLocation("skybox.diffuse_texture");
}

skybox_shader::skybox_shader( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	loc_matrix_PV = getUniformLocation("matrix_PV");
	loc_skybox_diffuse = getUniformLocation("skybox.diffuse_texture");
}

/*****************************************************************************
 * gintonic::text_shader                                                     *
 ****************************************************************************/

text_shader::text_shader()
: OpenGL::ShaderProgram("../Shaders/flat_text_uniform_color.vs", "../Shaders/flat_text_uniform_color.fs")
{
	loc_color = getUniformLocation("color");
	loc_texture = getUniformLocation("tex");
}

text_shader::~text_shader() noexcept
{
	/* Empty on purpose. */
}

void text_shader::set_color(const vec3f& color) const noexcept
{
	setUniform(loc_color, color);
}

void text_shader::set_texture(const GLint tex) const noexcept
{
	setUniform(loc_texture, tex);
}

} // namespace gintonic