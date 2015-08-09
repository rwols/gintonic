#ifndef gintonic_shaders_hpp
#define gintonic_shaders_hpp

#include "opengl.hpp"

namespace gintonic {

/*****************************************************************************
 * gintonic::matrix_PVM_shader                                               *
 ****************************************************************************/

class matrix_PVM_shader : public opengl::shader
{
public:
	matrix_PVM_shader();
	virtual ~matrix_PVM_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_matrix_PVM(const mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	matrix_PVM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	matrix_PVM_shader( 
		boost::filesystem::path vertex_shader,
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_matrix_PVM;
};

/*****************************************************************************
 * gintonic::matrix_PVM_VM_shader                                            *
 ****************************************************************************/

class matrix_PVM_VM_shader : public matrix_PVM_shader
{
public:
	virtual ~matrix_PVM_VM_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_matrix_VM(const mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	matrix_PVM_VM_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	matrix_PVM_VM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_matrix_VM;
};

/*****************************************************************************
 * gintonic::matrix_PVM_VM_N_shader                                          *
 ****************************************************************************/

class matrix_PVM_VM_N_shader : public matrix_PVM_VM_shader
{
public:
	virtual ~matrix_PVM_VM_N_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_matrix_N(const mat3f& m) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	matrix_PVM_VM_N_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	matrix_PVM_VM_N_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_matrix_N;
};

/*****************************************************************************
 * gintonic::gp_shader                                                       *
 ****************************************************************************/

class gp_shader : public matrix_PVM_VM_N_shader
{
public:
	gp_shader();
	virtual ~gp_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_color(const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_specular_color(const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_diffuse_color;
	GLuint loc_specular_color;
};

/*****************************************************************************
 * gintonic::gp_d_shader                                                     *
 ****************************************************************************/

class gp_d_shader : public gp_shader
{
public:
	gp_d_shader();
	virtual ~gp_d_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_d_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_d_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_diffuse_texture;
};

/*****************************************************************************
 * gintonic::gp_s_shader                                                     *
 ****************************************************************************/

class gp_s_shader : public gp_shader
{
public:
	gp_s_shader();
	virtual ~gp_s_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_specular_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_s_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_s_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_specular_texture;
};

/*****************************************************************************
 * gintonic::gp_n_shader                                                     *
 ****************************************************************************/

class gp_n_shader : public gp_shader
{
public:
	gp_n_shader();
	virtual ~gp_n_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_n_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_n_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_normal_texture;
};

/*****************************************************************************
 * gintonic::gp_ds_shader                                                    *
 ****************************************************************************/

class gp_ds_shader : public gp_d_shader
{
public:
	gp_ds_shader();
	virtual ~gp_ds_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_specular_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_ds_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_ds_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_specular_texture;
};

/*****************************************************************************
 * gintonic::gp_dn_shader                                                    *
 ****************************************************************************/

class gp_dn_shader : public gp_d_shader
{
public:
	gp_dn_shader();
	virtual ~gp_dn_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_dn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_dn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_normal_texture;
};

/*****************************************************************************
 * gintonic::gp_sn_shader                                                    *
 ****************************************************************************/

class gp_sn_shader : public gp_s_shader
{
public:
	gp_sn_shader();
	virtual ~gp_sn_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_sn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_sn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_normal_texture;
};

/*****************************************************************************
 * gintonic::gp_dsn_shader                                                   *
 ****************************************************************************/

class gp_dsn_shader : public gp_ds_shader
{
public:
	gp_dsn_shader();
	virtual ~gp_dsn_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal_texture(const GLint) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_dsn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_dsn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_normal_texture;
};

/*****************************************************************************
 * gintonic::geometry_null_shader                                            *
 ****************************************************************************/

class geometry_null_shader : public matrix_PVM_VM_N_shader
{
public:
	geometry_null_shader();
	virtual ~geometry_null_shader() BOOST_NOEXCEPT_OR_NOTHROW;
};

/*****************************************************************************
 * gintonic::gp_dc_shader                                                    *
 ****************************************************************************/

class gp_dc_shader : public virtual matrix_PVM_VM_N_shader
{
public:
	gp_dc_shader();
	virtual ~gp_dc_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_color(const vec4f& color) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_dc_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_dc_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_diffuse_color;
};

/*****************************************************************************
 * gintonic::gp_dcsc_shader                                                  *
 ****************************************************************************/

 class gp_dcsc_shader : public virtual gp_dc_shader
 {
 public:
 	gp_dcsc_shader();
 	virtual ~gp_dcsc_shader() BOOST_NOEXCEPT_OR_NOTHROW;
 	void set_specular_color(const vec4f& color) const BOOST_NOEXCEPT_OR_NOTHROW;
 protected:
	gp_dcsc_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_dcsc_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_specular_color;
 };

/*****************************************************************************
 * gintonic::gp_dt_shader                                                    *
 ****************************************************************************/

class gp_dt_shader : public virtual matrix_PVM_VM_N_shader
{
public:
	gp_dt_shader();
	virtual ~gp_dt_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_texture(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_dt_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_dt_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_diffuse_texture;
};

/*****************************************************************************
 * gintonic::gp_dcdt_shader                                                  *
 ****************************************************************************/

class gp_dcdt_shader 
: public gp_dc_shader
, public gp_dt_shader
{
public:
	gp_dcdt_shader();
	virtual ~gp_dcdt_shader() BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_dcdt_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_dcdt_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
};

/*****************************************************************************
 * gintonic::gp_c_shader                                                     *
 ****************************************************************************/

class gp_c_shader : public matrix_PVM_VM_N_shader
{
public:
	gp_c_shader();
	virtual ~gp_c_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_color(const vec4f& color) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_c_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_c_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_color;
};

/*****************************************************************************
 * gintonic::gp_cd_shader                                                    *
 ****************************************************************************/

class gp_cd_shader : public gp_c_shader
{
public:
	gp_cd_shader();
	virtual ~gp_cd_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_cd_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_cd_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_diffuse;
};

/*****************************************************************************
 * gintonic::gp_cds_shader                                                   *
 ****************************************************************************/

class gp_cds_shader : public gp_cd_shader
{
public:
	gp_cds_shader();
	virtual ~gp_cds_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	virtual void set_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_cds_shader(
		boost::filesystem::path vertex_shader,
		boost::filesystem::path fragment_shader);
	gp_cds_shader(
		boost::filesystem::path vertex_shader,
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_specular;
};

/*****************************************************************************
 * gintonic::gp_cdn_shader                                                   *
 ****************************************************************************/

class gp_cdn_shader : public matrix_PVM_shader
{
public:
	gp_cdn_shader();
	virtual ~gp_cdn_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_color(const vec4f& color) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	gp_cdn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	gp_cdn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_color;
	GLuint loc_diffuse;
	GLuint loc_normal;
};

/*****************************************************************************
 * gintonic::geometry_pass_shader                                            *
 ****************************************************************************/

class geometry_pass_shader : public matrix_PVM_VM_N_shader
{
public:
	geometry_pass_shader();
	virtual ~geometry_pass_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_color(const vec4f& diffuse_color) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse_factor(const GLfloat factor) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_diffuse_color;
	GLuint loc_diffuse;
	GLuint loc_diffuse_factor;
};

/*****************************************************************************
 * gintonic::lp_null_shader                                                  *
 ****************************************************************************/

class lp_null_shader : public opengl::shader
{
public:
	lp_null_shader();
	virtual ~lp_null_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	lp_null_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	lp_null_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_viewport_size;
	GLuint loc_gbuffer_diffuse;
};

/*****************************************************************************
 * gintonic::lp_directional_shader                                           *
 ****************************************************************************/

class lp_directional_shader : public lp_null_shader
{
public:
	lp_directional_shader();
	virtual ~lp_directional_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_intensity(const vec4f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	lp_directional_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	lp_directional_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_gbuffer_position;
	GLuint loc_gbuffer_specular;
	GLuint loc_gbuffer_normal;
	GLuint loc_light_intensity;
	GLuint loc_light_direction;
};

/*****************************************************************************
 * gintonic::lp_point_shader                                                 *
 ****************************************************************************/

class lp_point_shader : public matrix_PVM_shader
{
public:
	lp_point_shader();
	virtual ~lp_point_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_intensity(const vec4f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_position(const vec3f& position) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_attenuation(const vec4f& attenuation) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	lp_point_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	lp_point_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_viewport_size;
	GLuint loc_gbuffer_position;
	GLuint loc_gbuffer_diffuse;
	GLuint loc_gbuffer_specular;
	GLuint loc_gbuffer_normal;
	GLuint loc_light_intensity;
	GLuint loc_light_position;
	GLuint loc_light_attenuation;
};

/*****************************************************************************
 * gintonic::lp_spot_shader                                                  *
 ****************************************************************************/

class lp_spot_shader : public matrix_PVM_shader
{
public:
	lp_spot_shader();
	virtual ~lp_spot_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_intensity(const vec4f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_position(const vec3f& position) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_attenuation(const vec4f& attenuation) const BOOST_NOEXCEPT_OR_NOTHROW;
protected:
	lp_spot_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	lp_spot_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:
	GLuint loc_viewport_size;
	GLuint loc_gbuffer_position;
	GLuint loc_gbuffer_diffuse;
	GLuint loc_gbuffer_specular;
	GLuint loc_gbuffer_normal;
	GLuint loc_light_intensity;
	GLuint loc_light_position;
	GLuint loc_light_direction;
	GLuint loc_light_attenuation;
};

/*****************************************************************************
 * gintonic::light_pass_shader                                               *
 ****************************************************************************/

class light_pass_shader : public matrix_PVM_shader
{
public:
	light_pass_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	light_pass_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
	virtual ~light_pass_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_intensity(const vec3f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW;

private:
	GLuint loc_viewport_size;
	GLuint loc_light_intensity;

};

/*****************************************************************************
 * gintonic::directional_light_pass_shader                                   *
 ****************************************************************************/

class directional_light_pass_shader : public light_pass_shader
{
public:
	directional_light_pass_shader();
	virtual ~directional_light_pass_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_light_direction;
	GLuint loc_gbuffer_diffuse;
	GLuint loc_gbuffer_normal;
};

/*****************************************************************************
 * gintonic::point_light_pass_shader                                         *
 ****************************************************************************/

class point_light_pass_shader : public light_pass_shader
{
public:
	point_light_pass_shader();
	virtual ~point_light_pass_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_position(const vec3f& position) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_attenuation(const vec3f& attenuation) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_light_position;
	GLuint loc_light_attenuation;
	GLuint loc_gbuffer_position;
	GLuint loc_gbuffer_diffuse;
	GLuint loc_gbuffer_normal;
};

/*****************************************************************************
 * gintonic::text_shader                                                     *
 ****************************************************************************/

class text_shader : public opengl::shader
{
public:
	text_shader();
	virtual ~text_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_color(const vec3f& color) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_texture(const GLint tex) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_color;
	GLuint loc_texture;
};

} // namespace gintonic

#endif