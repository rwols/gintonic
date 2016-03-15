/**
 * @file shaders.hpp
 * @brief Defines all the shader classes.
 * @author Raoul Wols
 */

#pragma once

#include "OpenGL/ShaderProgram.hpp"

namespace gintonic {

/**
 * @brief A shader that only houses a matrix_PVM uniform.
 * @details This shader should not be used directly but is really only here
 * to serve as a base class for inheritance.
 */
class matrix_PVM_shader : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	matrix_PVM_shader();

	/// Destructor.
	virtual ~matrix_PVM_shader() noexcept;
	
	/**
	 * @brief Set the matrix_PVM uniform variable in the shader.
	 * @param m The matrix to set the matrix_PVM uniform variable to.
	 */
	void set_matrix_PVM(const mat4f& m) const noexcept;

protected:

	/// Forwarding constructor.
	matrix_PVM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	matrix_PVM_shader( 
		boost::filesystem::path vertex_shader,
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_matrix_PVM;
};

/**
 * @brief A shader that manages a matrix_PVM and a matrix_VM uniform.
 * @details This shader should not be used directly but is really only here
 * to serve as a base class for inheritance.
 */
class matrix_PVM_VM_shader : public matrix_PVM_shader
{
public:

	/// Destructor.
	virtual ~matrix_PVM_VM_shader() noexcept;

	/**
	 * @brief Set the matrix_VM uniform variable in the shader.
	 * @param m The matrix to set the matrix_VM uniform variable to.
	 */
	void set_matrix_VM(const mat4f& m) const noexcept;

protected:

	/// Forwarding constructor.
	matrix_PVM_VM_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	matrix_PVM_VM_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_matrix_VM;
};

/**
 * @brief A shader that manages a matrix_PVM, a matrix_VM and a matrix_N
 * uniform.
 * @details This shader should not be used directly but is really only here
 * to serve as a base class for inheritance.
 */
class matrix_PVM_VM_N_shader : public matrix_PVM_VM_shader
{
public:

	/// Destructor.
	virtual ~matrix_PVM_VM_N_shader() noexcept;

	/**
	 * @brief Set the matrix_N uniform variable in the shader.
	 * @param m The matrix to set the matrix_N uniform variable to.
	 */
	void set_matrix_N(const mat3f& m) const noexcept;

protected:

	/// Forwarding constructor.
	matrix_PVM_VM_N_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	matrix_PVM_VM_N_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_matrix_N;
};

/**
 * @brief Geometry pass shader with only a base diffuse color and base
 * specular color.
 */
class gp_shader : public matrix_PVM_VM_N_shader
{
public:

	/// Default constructor.
	gp_shader();

	/// Destructor.
	virtual ~gp_shader() noexcept;

	/**
	 * @brief Set the diffuse base color of the material.
	 * 
	 * @details The uniform with the name "material.diffuse_color" will be
	 * set to the given value. So such a uniform variable name must be part
	 * of a struct with an instance variable name of "material". The struct
	 * name can be anything you like.
	 * 
	 * @param c The color value.
	 */
	void set_diffuse_color(const vec4f& c) const noexcept;

	/**
	 * @brief Set the specular base color of the material.
	 * 
	 * @details The uniform with the name "material.specular_color" will be
	 * set to the given value. So such a uniform variable name must be part
	 * of a struct with an instance variable name of "material". The struct
	 * name can be anything you like.
	 * 
	 * @param c The color value.
	 */
	void set_specular_color(const vec4f& c) const noexcept;

protected:

	/// Forwarding constructor.
	gp_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_diffuse_color;
	GLuint loc_specular_color;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color and
 * a diffuse texture.
 */
class gp_d_shader : public gp_shader
{
public:

	/// Default constructor.
	gp_d_shader();

	/// Destructor.
	virtual ~gp_d_shader() noexcept;

	/**
	 * @brief Set the diffuse texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding diffuse texture was bound.
	 * 
	 * @param i The value for the diffuse texture sampler in the shader.
	 */
	void set_diffuse_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gp_d_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_d_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_diffuse_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color and
 * a specular texture.
 */
class gp_s_shader : public gp_shader
{
public:

	/// Default constructor.
	gp_s_shader();

	/// Destructor.
	virtual ~gp_s_shader() noexcept;

	/**
	 * @brief Set the specular texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding specular texture was bound.
	 * 
	 * @param i The value for the specular texture sampler in the shader.
	 */
	void set_specular_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gp_s_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_s_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_specular_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color and
 * a normal texture.
 */
class gp_n_shader : public gp_shader
{
public:

	/// Default constructor.
	gp_n_shader();

	/// Destructor.
	virtual ~gp_n_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gp_n_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_n_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * diffuse texture and a specular texture.
 */
class gp_ds_shader : public gp_d_shader
{
public:

	/// Default constructor.
	gp_ds_shader();

	/// Destructor.
	virtual ~gp_ds_shader() noexcept;

	/**
	 * @brief Set the specular texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding specular texture was bound.
	 * 
	 * @param i The value for the specular texture sampler in the shader.
	 */
	void set_specular_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gp_ds_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_ds_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_specular_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * diffuse texture and a normal texture.
 */
class gp_dn_shader : public gp_d_shader
{
public:

	/// Default constructor.
	gp_dn_shader();

	/// Destructor.
	virtual ~gp_dn_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gp_dn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_dn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * specular texture and a normal texture.
 */
class gp_sn_shader : public gp_s_shader
{
public:

	/// Default constructor.
	gp_sn_shader();

	/// Destructor.
	virtual ~gp_sn_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gp_sn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_sn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * diffuse texture, specular texture and a normal texture.
 */
class gp_dsn_shader : public gp_ds_shader
{
public:

	/// Default constructor.
	gp_dsn_shader();

	/// Destructor.
	virtual ~gp_dsn_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;
protected:

	/// Forwarding constructor.
	gp_dsn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gp_dsn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);
private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with only a base diffuse color and base
 * specular color, instanced version.
 */
class gpi_shader : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	gpi_shader();

	/// Destructor.
	virtual ~gpi_shader() noexcept;

	/**
	 * @brief Set the diffuse base color of the material.
	 * 
	 * @details The uniform with the name "material.diffuse_color" will be
	 * set to the given value. So such a uniform variable name must be part
	 * of a struct with an instance variable name of "material". The struct
	 * name can be anything you like.
	 * 
	 * @param c The color value.
	 */
	void set_diffuse_color(const vec4f& c) const noexcept;

	/**
	 * @brief Set the specular base color of the material.
	 * 
	 * @details The uniform with the name "material.specular_color" will be
	 * set to the given value. So such a uniform variable name must be part
	 * of a struct with an instance variable name of "material". The struct
	 * name can be anything you like.
	 * 
	 * @param c The color value.
	 */
	void set_specular_color(const vec4f& c) const noexcept;

protected:

	/// Forwarding constructor.
	gpi_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_diffuse_color;
	GLuint loc_specular_color;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color and
 * a diffuse texture, instanced version.
 */
class gpi_d_shader : public gpi_shader
{
public:

	/// Default constructor.
	gpi_d_shader();

	/// Destructor.
	virtual ~gpi_d_shader() noexcept;

	/**
	 * @brief Set the diffuse texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding diffuse texture was bound.
	 * 
	 * @param i The value for the diffuse texture sampler in the shader.
	 */
	void set_diffuse_texture(const GLint i) const noexcept;
protected:

	/// Forwarding constructor.
	gpi_d_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_d_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_diffuse_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color and
 * a specular texture, instanced version.
 */
class gpi_s_shader : public gpi_shader
{
public:

	/// Default constructor.
	gpi_s_shader();

	/// Destructor.
	virtual ~gpi_s_shader() noexcept;

	/**
	 * @brief Set the specular texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding specular texture was bound.
	 * 
	 * @param i The value for the specular texture sampler in the shader.
	 */
	void set_specular_texture(const GLint i) const noexcept;
protected:

	/// Forwarding constructor.
	gpi_s_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_s_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_specular_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color and
 * a normal texture, instanced version.
 */
class gpi_n_shader : public gpi_shader
{
public:

	/// Default constructor.
	gpi_n_shader();

	/// Destructor.
	virtual ~gpi_n_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gpi_n_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_n_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * diffuse texture and a specular texture, instanced version.
 */
class gpi_ds_shader : public gpi_d_shader
{
public:

	/// Default constructor.
	gpi_ds_shader();

	/// Destructor.
	virtual ~gpi_ds_shader() noexcept;

	/**
	 * @brief Set the specular texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding specular texture was bound.
	 * 
	 * @param i The value for the specular texture sampler in the shader.
	 */
	void set_specular_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gpi_ds_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_ds_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_specular_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * diffuse texture and a normal texture, instanced version.
 */
class gpi_dn_shader : public gpi_d_shader
{
public:

	/// Default constructor.
	gpi_dn_shader();

	/// Destructor.
	virtual ~gpi_dn_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;
protected:

	/// Forwarding constructor.
	gpi_dn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_dn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * specular texture and a normal texture, instanced version.
 */
class gpi_sn_shader : public gpi_s_shader
{
public:

	/// Default constructor.
	gpi_sn_shader();

	/// Destructor.
	virtual ~gpi_sn_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gpi_sn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_sn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader with a diffuse color, a specular color, a
 * diffuse texture, specular texture and a normal texture, instanced version.
 */
class gpi_dsn_shader : public gpi_ds_shader
{
public:

	/// Default constructor.
	gpi_dsn_shader();

	/// Destructor.
	virtual ~gpi_dsn_shader() noexcept;

	/**
	 * @brief Set the normal texture sampler.
	 * 
	 * @details This value must be the same value that was given as the
	 * active texture unit when the corresponding normal texture was bound.
	 * 
	 * @param i The value for the normal texture sampler in the shader.
	 */
	void set_normal_texture(const GLint i) const noexcept;

protected:

	/// Forwarding constructor.
	gpi_dsn_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	gpi_dsn_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:
	GLuint loc_normal_texture;
};

/**
 * @brief Geometry pass shader that does nothing at all.
 */
class geometry_null_shader : public matrix_PVM_VM_N_shader
{
public:

	/// Default constructor.
	geometry_null_shader();

	/// Destructor.
	virtual ~geometry_null_shader() noexcept;
};

/**
 * @brief Base class for light pass shaders.
 */
class lp_null_shader : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	lp_null_shader();

	/// Destructor.
	virtual ~lp_null_shader() noexcept;

	/**
	 * @brief Set the geometry buffer diffuse sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_DIFFUSE.
	 * 
	 * @param texture_unit The value for the geometry buffer diffuse sampler.
	 * Must always be equal to Renderer::GBUFFER_DIFFUSE.
	 */
	void set_gbuffer_diffuse(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the viewport size uniform variable.
	 * @details You can query the viewport size with the static function
	 * Renderer::viewport_size.
	 * 
	 * @param size The size of the viewport. Should always use
	 * Renderer::viewport_size unless you have a specific reason not to.
	 */
	void set_viewport_size(const vec2f& size) const noexcept;

protected:

	/// Forwarding constructor.
	lp_null_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	lp_null_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_viewport_size;
	GLuint loc_gbuffer_diffuse;
};

/**
 * @brief Light pass shader for an ambient light.
 */
class lp_ambient_shader : public lp_null_shader
{
public:

	/// Default constructor.
	lp_ambient_shader();

	/// Destructor.
	virtual ~lp_ambient_shader() noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void set_light_intensity(const vec4f& intensity) const 
		noexcept;

protected:

	/// Forwarding constructor.
	lp_ambient_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	lp_ambient_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_light_intensity;
};

/**
 * @brief Light pass shader for a directional light.
 */
class lp_directional_shader : public lp_null_shader
{
public:

	/// Default constructor.
	lp_directional_shader();

	/// Destructor.
	virtual ~lp_directional_shader() noexcept;

	/**
	 * @brief Set the geometry buffer position sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_POSITION.
	 * 
	 * @param texture_unit The value for the geometry buffer position sampler.
	 * Must always be equal to Renderer::GBUFFER_POSITION.
	 */
	void set_gbuffer_position(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer specular sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_SPECULAR.
	 * 
	 * @param texture_unit The value for the geometry buffer specular sampler.
	 * Must always be equal to Renderer::GBUFFER_SPECULAR.
	 */
	void set_gbuffer_specular(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer normal sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_NORMAL.
	 * 
	 * @param texture_unit The value for the geometry buffer normal sampler.
	 * Must always be equal to Renderer::GBUFFER_NORMAL.
	 */
	void set_gbuffer_normal(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void set_light_intensity(const vec4f& intensity) const 
		noexcept;

	/**
	 * @brief Set the light direction uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.direction".
	 * 
	 * @param direction The direction value.
	 */
	void set_light_direction(const vec3f& direction) const 
		noexcept;

protected:

	/// Forwarding constructor.
	lp_directional_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
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

/**
 * @brief Light pass shader for a point light.
 */
class lp_point_shader : public matrix_PVM_shader
{
public:

	/// Default constructor.
	lp_point_shader();

	/// Destructor.
	virtual ~lp_point_shader() noexcept;

	/**
	 * @brief Set the viewport size uniform variable.
	 * @details You can query the viewport size with the static function
	 * Renderer::viewport_size.
	 * 
	 * @param size The size of the viewport. Should always use
	 * Renderer::viewport_size unless you have a specific reason not to.
	 */
	void set_viewport_size(const vec2f& size) const noexcept;

	/**
	 * @brief Set the geometry buffer position sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_POSITION.
	 * 
	 * @param texture_unit The value for the geometry buffer position sampler.
	 * Must always be equal to Renderer::GBUFFER_POSITION.
	 */
	void set_gbuffer_position(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer diffuse sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_DIFFUSE.
	 * 
	 * @param texture_unit The value for the geometry buffer diffuse sampler.
	 * Must always be equal to Renderer::GBUFFER_DIFFUSE.
	 */
	void set_gbuffer_diffuse(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer specular sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_SPECULAR.
	 * 
	 * @param texture_unit The value for the geometry buffer specular sampler.
	 * Must always be equal to Renderer::GBUFFER_SPECULAR.
	 */
	void set_gbuffer_specular(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer normal sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_NORMAL.
	 * 
	 * @param texture_unit The value for the geometry buffer normal sampler.
	 * Must always be equal to Renderer::GBUFFER_NORMAL.
	 */
	void set_gbuffer_normal(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void set_light_intensity(const vec4f& intensity) const 
		noexcept;

	/**
	 * @brief Set the light position uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.position".
	 * 
	 * @param position The position value.
	 */
	void set_light_position(const vec3f& position) const 
		noexcept;

	/**
	 * @brief Set the light attenuation uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.attenuation".
	 * 
	 * @param attenuation The attenuation value.
	 */
	void set_light_attenuation(const vec4f& attenuation) const 
		noexcept;
protected:

	/// Forwarding constructor.
	lp_point_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
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

/**
 * @brief Light pass shader for a spot light.
 */
class lp_spot_shader : public matrix_PVM_shader
{
public:

	/// Default constructor.
	lp_spot_shader();

	/// Destructor.
	virtual ~lp_spot_shader() noexcept;

	/**
	 * @brief Set the viewport size uniform variable.
	 * @details You can query the viewport size with the static function
	 * Renderer::viewport_size.
	 * 
	 * @param size The size of the viewport. Should always use
	 * Renderer::viewport_size unless you have a specific reason not to.
	 */
	void set_viewport_size(const vec2f& size) const noexcept;

	/**
	 * @brief Set the geometry buffer position sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_POSITION.
	 * 
	 * @param texture_unit The value for the geometry buffer position sampler.
	 * Must always be equal to Renderer::GBUFFER_POSITION.
	 */
	void set_gbuffer_position(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer diffuse sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_DIFFUSE.
	 * 
	 * @param texture_unit The value for the geometry buffer diffuse sampler.
	 * Must always be equal to Renderer::GBUFFER_DIFFUSE.
	 */
	void set_gbuffer_diffuse(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer specular sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_SPECULAR.
	 * 
	 * @param texture_unit The value for the geometry buffer specular sampler.
	 * Must always be equal to Renderer::GBUFFER_SPECULAR.
	 */
	void set_gbuffer_specular(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the geometry buffer normal sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_NORMAL.
	 * 
	 * @param texture_unit The value for the geometry buffer normal sampler.
	 * Must always be equal to Renderer::GBUFFER_NORMAL.
	 */
	void set_gbuffer_normal(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void set_light_intensity(const vec4f& intensity) const 
		noexcept;

	/**
	 * @brief Set the light position uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.position".
	 * 
	 * @param position The position value.
	 */
	void set_light_position(const vec3f& position) const 
		noexcept;

	/**
	 * @brief Set the light direction uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.direction".
	 * 
	 * @param direction The direction value.
	 */
	void set_light_direction(const vec3f& direction) const 
		noexcept;

	/**
	 * @brief Set the light attenuation uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.attenuation".
	 * 
	 * @param attenuation The attenuation value.
	 */
	void set_light_attenuation(const vec4f& attenuation) const 
		noexcept;

protected:

	/// Forwarding constructor.
	lp_spot_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
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

/**
 * @brief Shadow pass shader for a directional light.
 */
class sp_directional_shader : public matrix_PVM_shader
{
public:
	/// Default constructor.
	sp_directional_shader();

	/// Destructor.
	virtual ~sp_directional_shader() noexcept;
};

/**
 * @brief Shader for a skybox.
 */
class skybox_shader : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	skybox_shader();

	/// Destructor.
	virtual ~skybox_shader() noexcept;

	/**
	 * @brief Set the matrix_PV uniform variable in the shader.
	 * 
	 * @param matrix The matrix_PV to set the uniform to.
	 */
	void set_matrix_PV(const mat4f& matrix) const noexcept;

	/**
	 * @brief Set the skybox diffuse sampler.
	 * 
	 * @param texture_unit The value of the active texture that the skybox
	 * cube texture is bound for.
	 */
	void set_skybox_diffuse(const GLint texture_unit) const 
		noexcept;
protected:

	/// Forwarding constructor.
	skybox_shader(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	skybox_shader( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint loc_matrix_PV;
	GLuint loc_skybox_diffuse;
};

/**
 * @brief Simple shader for two-dimensional text.
 */
class text_shader : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	text_shader();

	/// Destructor.
	virtual ~text_shader() noexcept;

	/**
	 * @brief Set the text color uniform variable.
	 * 
	 * @param color The color of the text.
	 */
	void set_color(const vec3f& color) const noexcept;

	/**
	 * @brief Set the text texture.
	 * 
	 * @details You don't use this method manually, but instead you use the
	 * font mechanism (to be explained later).
	 * 
	 * @param texture_unit The same number as the active texture unit for the
	 * text.
	 */	
	void set_texture(const GLint texture_unit) const 
		noexcept;

private:
	
	GLuint loc_color;
	GLuint loc_texture;
};

} // namespace gintonic
