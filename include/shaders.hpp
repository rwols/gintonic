#ifndef gintonic_shaders_hpp
#define gintonic_shaders_hpp

#include "opengl.hpp"

namespace gintonic {

class matrix_PVM_shader : public opengl::shader
{
public:
	matrix_PVM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	matrix_PVM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
	virtual ~matrix_PVM_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_matrix_PVM(const mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_matrix_PVM;
};

class matrix_PVM_VM_shader : public matrix_PVM_shader
{
public:
	matrix_PVM_VM_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	matrix_PVM_VM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
	virtual ~matrix_PVM_VM_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_matrix_VM(const mat4f& m) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_matrix_VM;
};

class matrix_PVM_VM_N_shader : public matrix_PVM_VM_shader
{
public:
	matrix_PVM_VM_N_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);
	matrix_PVM_VM_N_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

	virtual ~matrix_PVM_VM_N_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_matrix_N(const mat3f& m) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	GLuint loc_matrix_N;
};

class geometry_null_shader : public matrix_PVM_VM_N_shader
{
public:
	geometry_null_shader();
	virtual ~geometry_null_shader() BOOST_NOEXCEPT_OR_NOTHROW;
};

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

class lp_directional_shader : public lp_null_shader
{
public:
	lp_directional_shader();
	virtual ~lp_directional_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	// void set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW; // NOTE: ONLY TEMPORARY
	// void set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW; // NOTE: ONLY TEMPORARY
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_intensity(const vec3f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW;
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
	// GLuint loc_gbuffer_position; // NOTE: ONLY TEMPORARY
	// GLuint loc_gbuffer_specular; // NOTE: ONLY TEMPORARY
	GLuint loc_gbuffer_normal;
	GLuint loc_light_intensity;
	GLuint loc_light_direction;
};

class lp_point_shader : public matrix_PVM_shader
{
public:
	lp_point_shader();
	virtual ~lp_point_shader() BOOST_NOEXCEPT_OR_NOTHROW;
	void set_viewport_size(const vec2f& size) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_position(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_gbuffer_diffuse(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	// void set_gbuffer_specular(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW; // NOTE: ONLY TEMPORARY
	void set_gbuffer_normal(const GLint texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_intensity(const vec3f& intensity) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_position(const vec3f& position) const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_light_attenuation(const vec3f& attenuation) const BOOST_NOEXCEPT_OR_NOTHROW;
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
	// GLuint loc_gbuffer_specular; // NOTE: ONLY TEMPORARY
	GLuint loc_gbuffer_normal;
	GLuint loc_light_intensity;
	GLuint loc_light_position;
	GLuint loc_light_attenuation;
};

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