/**
 * @file ShaderPrograms.hpp
 * @brief Defines all the shader classes.
 * @author Raoul Wols
 */

#pragma once

#include "OpenGL/ShaderProgram.hpp"

namespace gintonic {

class ShadowShaderProgram : public OpenGL::ShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new ShadowShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const ShadowShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	ShadowShaderProgram();

	virtual ~ShadowShaderProgram() noexcept = default;

	void setMatrixPVM(const mat4f& value) const noexcept;
	void setInstancedRendering(const GLint value) const noexcept;

private:

	static ShadowShaderProgram* sInstance;

	GLuint mMatrixPVM;
	GLuint mInstancedRendering;

};

class MaterialShaderProgram : public OpenGL::ShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new MaterialShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const MaterialShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	MaterialShaderProgram();

	virtual ~MaterialShaderProgram() noexcept = default;

	void setMatrixPVM(const mat4f& value) const noexcept;
	void setMatrixVM(const mat4f& value) const noexcept;
	void setMatrixN(const mat3f& value) const noexcept;
	void setInstancedRendering(const GLint value) const noexcept;
	void setHasTangentsAndBitangents(const GLint value) const noexcept;
	void setMaterialDiffuseColor(const vec4f& value) const noexcept;
	void setMaterialSpecularColor(const vec4f& value) const noexcept;
	void setMaterialDiffuseTexture(const GLint value) const noexcept;
	void setMaterialSpecularTexture(const GLint value) const noexcept;
	void setMaterialNormalTexture(const GLint value) const noexcept;
	void setMaterialFlag(const GLint value) const noexcept;

private:

	static MaterialShaderProgram* sInstance;

	GLuint mMatrixPVM;
	GLuint mMatrixVM;
	GLuint mMatrixN;
	GLuint mInstancedRendering;
	GLuint mHasTangentsAndBitangents;
	GLuint mMaterialDiffuseColor;
	GLuint mMaterialSpecularColor;
	GLuint mMaterialDiffuseTexture;
	GLuint mMaterialSpecularTexture;
	GLuint mMaterialNormalTexture;
	GLuint mMaterialFlag;

};

// /**
//  * @brief A shader that only houses a matrix_PVM uniform.
//  * @details This shader should not be used directly but is really only here
//  * to serve as a base class for inheritance.
//  */
class Matrix_PVM_ShaderProgram : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	Matrix_PVM_ShaderProgram();

	/// Destructor.
	virtual ~Matrix_PVM_ShaderProgram() noexcept;
	
	/**
	 * @brief Set the matrix_PVM uniform variable in the shader.
	 * @param m The matrix to set the matrix_PVM uniform variable to.
	 */
	void setMatrixPVM(const mat4f& m) const noexcept;

protected:

	/// Forwarding constructor.
	Matrix_PVM_ShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	Matrix_PVM_ShaderProgram( 
		boost::filesystem::path vertex_shader,
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint mMatrixPVM;
};

// /**
//  * @brief A shader that manages a matrix_PVM and a matrix_VM uniform.
//  * @details This shader should not be used directly but is really only here
//  * to serve as a base class for inheritance.
//  */
// class Matrix_PVM_VM_ShaderProgram : public Matrix_PVM_ShaderProgram
// {
// public:

// 	/// Destructor.
// 	virtual ~Matrix_PVM_VM_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the matrix_VM uniform variable in the shader.
// 	 * @param m The matrix to set the matrix_VM uniform variable to.
// 	 */
// 	void setMatrixVM(const mat4f& m) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Matrix_PVM_VM_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Matrix_PVM_VM_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mMatrixVM;
// };

// /**
//  * @brief A shader that manages a matrix_PVM, a matrix_VM and a matrix_N
//  * uniform.
//  * @details This shader should not be used directly but is really only here
//  * to serve as a base class for inheritance.
//  */
// class Matrix_PVM_VM_N_ShaderProgram : public Matrix_PVM_VM_ShaderProgram
// {
// public:

// 	/// Destructor.
// 	virtual ~Matrix_PVM_VM_N_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the matrix_N uniform variable in the shader.
// 	 * @param m The matrix to set the matrix_N uniform variable to.
// 	 */
// 	void setMatrixN(const mat3f& m) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Matrix_PVM_VM_N_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Matrix_PVM_VM_N_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mMatrixN;
// };

// /**
//  * @brief Geometry pass shader with only a base diffuse color and base
//  * specular color.
//  */
// class GeometryShaderProgram : public Matrix_PVM_VM_N_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the diffuse base color of the material.
// 	 * 
// 	 * @details The uniform with the name "material.diffuse_color" will be
// 	 * set to the given value. So such a uniform variable name must be part
// 	 * of a struct with an instance variable name of "material". The struct
// 	 * name can be anything you like.
// 	 * 
// 	 * @param c The color value.
// 	 */
// 	void setDiffuseColor(const vec4f& c) const noexcept;

// 	/**
// 	 * @brief Set the specular base color of the material.
// 	 * 
// 	 * @details The uniform with the name "material.specular_color" will be
// 	 * set to the given value. So such a uniform variable name must be part
// 	 * of a struct with an instance variable name of "material". The struct
// 	 * name can be anything you like.
// 	 * 
// 	 * @param c The color value.
// 	 */
// 	void setSpecularColor(const vec4f& c) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	GeometryShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mDiffuseColor;
// 	GLuint mSpecularColor;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color and
//  * a diffuse texture.
//  */
// class Geometry_D_ShaderProgram : public GeometryShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_D_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_D_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the diffuse texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding diffuse texture was bound.
// 	 * 
// 	 * @param i The value for the diffuse texture sampler in the shader.
// 	 */
// 	void setDiffuseTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_D_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_D_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mDiffuseTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color and
//  * a specular texture.
//  */
// class Geometry_S_ShaderProgram : public GeometryShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_S_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_S_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the specular texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding specular texture was bound.
// 	 * 
// 	 * @param i The value for the specular texture sampler in the shader.
// 	 */
// 	void setSpecularTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_S_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_S_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mSpecularTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color and
//  * a normal texture.
//  */
// class Geometry_N_ShaderProgram : public GeometryShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_N_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_N_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_N_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_N_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * diffuse texture and a specular texture.
//  */
// class Geometry_DS_ShaderProgram : public Geometry_D_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_DS_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_DS_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the specular texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding specular texture was bound.
// 	 * 
// 	 * @param i The value for the specular texture sampler in the shader.
// 	 */
// 	void setSpecularTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_DS_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_DS_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mSpecularTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * diffuse texture and a normal texture.
//  */
// class Geometry_DN_ShaderProgram : public Geometry_D_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_DN_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_DN_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_DN_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_DN_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * specular texture and a normal texture.
//  */
// class Geometry_SN_ShaderProgram : public Geometry_S_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_SN_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_SN_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_SN_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_SN_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// *
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * diffuse texture, specular texture and a normal texture.
 
// class Geometry_DSN_ShaderProgram : public Geometry_DS_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	Geometry_DSN_ShaderProgram();

// 	/// Destructor.
// 	virtual ~Geometry_DSN_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	Geometry_DSN_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	Geometry_DSN_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader with only a base diffuse color and base
//  * specular color, instanced version.
//  */
// class GeometryInstancedShaderProgram : public OpenGL::ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstancedShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstancedShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the diffuse base color of the material.
// 	 * 
// 	 * @details The uniform with the name "material.diffuse_color" will be
// 	 * set to the given value. So such a uniform variable name must be part
// 	 * of a struct with an instance variable name of "material". The struct
// 	 * name can be anything you like.
// 	 * 
// 	 * @param c The color value.
// 	 */
// 	void setDiffuseColor(const vec4f& c) const noexcept;

// 	/**
// 	 * @brief Set the specular base color of the material.
// 	 * 
// 	 * @details The uniform with the name "material.specular_color" will be
// 	 * set to the given value. So such a uniform variable name must be part
// 	 * of a struct with an instance variable name of "material". The struct
// 	 * name can be anything you like.
// 	 * 
// 	 * @param c The color value.
// 	 */
// 	void setSpecularColor(const vec4f& c) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	GeometryInstancedShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstancedShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mDiffuseColor;
// 	GLuint mSpecularColor;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color and
//  * a diffuse texture, instanced version.
//  */
// class GeometryInstanced_D_ShaderProgram : public GeometryInstancedShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_D_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_D_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the diffuse texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding diffuse texture was bound.
// 	 * 
// 	 * @param i The value for the diffuse texture sampler in the shader.
// 	 */
// 	void setDiffuseTexture(const GLint i) const noexcept;
// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_D_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_D_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mDiffuseTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color and
//  * a specular texture, instanced version.
//  */
// class GeometryInstanced_S_ShaderProgram : public GeometryInstancedShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_S_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_S_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the specular texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding specular texture was bound.
// 	 * 
// 	 * @param i The value for the specular texture sampler in the shader.
// 	 */
// 	void setSpecularTexture(const GLint i) const noexcept;
// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_S_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_S_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mSpecularTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color and
//  * a normal texture, instanced version.
//  */
// class GeometryInstanced_N_ShaderProgram : public GeometryInstancedShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_N_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_N_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_N_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_N_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * diffuse texture and a specular texture, instanced version.
//  */
// class GeometryInstanced_DS_ShaderProgram : public GeometryInstanced_D_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_DS_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_DS_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the specular texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding specular texture was bound.
// 	 * 
// 	 * @param i The value for the specular texture sampler in the shader.
// 	 */
// 	void setSpecularTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_DS_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_DS_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mSpecularTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * diffuse texture and a normal texture, instanced version.
//  */
// class GeometryInstanced_DN_ShaderProgram : public GeometryInstanced_D_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_DN_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_DN_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;
// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_DN_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_DN_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * specular texture and a normal texture, instanced version.
//  */
// class GeometryInstanced_SN_ShaderProgram : public GeometryInstanced_S_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_SN_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_SN_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_SN_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_SN_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:

// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader with a diffuse color, a specular color, a
//  * diffuse texture, specular texture and a normal texture, instanced version.
//  */
// class GeometryInstanced_DSN_ShaderProgram : public GeometryInstanced_DS_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryInstanced_DSN_ShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryInstanced_DSN_ShaderProgram() noexcept;

// 	/**
// 	 * @brief Set the normal texture sampler.
// 	 * 
// 	 * @details This value must be the same value that was given as the
// 	 * active texture unit when the corresponding normal texture was bound.
// 	 * 
// 	 * @param i The value for the normal texture sampler in the shader.
// 	 */
// 	void setNormalTexture(const GLint i) const noexcept;

// protected:

// 	/// Forwarding constructor.
// 	GeometryInstanced_DSN_ShaderProgram(
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path fragment_shader);

// 	/// Forwarding constructor.
// 	GeometryInstanced_DSN_ShaderProgram( 
// 		boost::filesystem::path vertex_shader, 
// 		boost::filesystem::path geometry_shader,
// 		boost::filesystem::path fragment_shader);

// private:
// 	GLuint mNormalTexture;
// };

// /**
//  * @brief Geometry pass shader that does nothing at all.
//  */
// class GeometryNullShaderProgram : public Matrix_PVM_VM_N_ShaderProgram
// {
// public:

// 	/// Default constructor.
// 	GeometryNullShaderProgram();

// 	/// Destructor.
// 	virtual ~GeometryNullShaderProgram() noexcept;
// };

/**
 * @brief Base class for light pass shaders.
 */
class LightShaderProgram : public OpenGL::ShaderProgram
{
public:

	/// Default constructor.
	LightShaderProgram();

	/// Destructor.
	virtual ~LightShaderProgram() noexcept;

	/**
	 * @brief Set the geometry buffer diffuse sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_DIFFUSE.
	 * 
	 * @param texture_unit The value for the geometry buffer diffuse sampler.
	 * Must always be equal to Renderer::GBUFFER_DIFFUSE.
	 */
	void setGeometryBufferDiffuseTexture(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the viewport size uniform variable.
	 * @details You can query the viewport size with the static function
	 * Renderer::viewport_size.
	 * 
	 * @param size The size of the viewport. Should always use
	 * Renderer::viewport_size unless you have a specific reason not to.
	 */
	void setViewportSize(const vec2f& size) const noexcept;

protected:

	/// Forwarding constructor.
	LightShaderProgram(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	LightShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	GLuint mViewportSize;
	GLuint mGeometryBufferDiffuseTexture;
};

/**
 * @brief Light pass shader for an ambient light.
 */
class AmbientLightShaderProgram : public LightShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new AmbientLightShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const AmbientLightShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	/// Default constructor.
	AmbientLightShaderProgram();

	/// Destructor.
	virtual ~AmbientLightShaderProgram() noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void setLightIntensity(const vec4f& intensity) const 
		noexcept;

protected:

	/// Forwarding constructor.
	AmbientLightShaderProgram(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	AmbientLightShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	static AmbientLightShaderProgram* sInstance;

	GLuint mLightIntensity;
};

/**
 * @brief Light pass shader for a directional light.
 */
class DirectionalLightShaderProgram : public LightShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new DirectionalLightShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const DirectionalLightShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	/// Default constructor.
	DirectionalLightShaderProgram();

	/// Destructor.
	virtual ~DirectionalLightShaderProgram() noexcept;

	/**
	 * @brief Set the geometry buffer position sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_POSITION.
	 * 
	 * @param texture_unit The value for the geometry buffer position sampler.
	 * Must always be equal to Renderer::GBUFFER_POSITION.
	 */
	void setGeometryBufferPositionTexture(const GLint texture_unit) const 
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
	void setGeometryBufferSpecularTexture(const GLint texture_unit) const 
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
	void setGeometryBufferNormalTexture(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void setLightIntensity(const vec4f& intensity) const 
		noexcept;

	/**
	 * @brief Set the light direction uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.direction".
	 * 
	 * @param direction The direction value.
	 */
	void setLightDirection(const vec3f& direction) const 
		noexcept;

protected:

	/// Forwarding constructor.
	DirectionalLightShaderProgram(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	DirectionalLightShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	static DirectionalLightShaderProgram* sInstance;

	GLuint mGeometryBufferPositionTexture;
	GLuint mGeometryBufferSpecularTexture;
	GLuint mGeometryBufferNormalTexture;
	GLuint mLightIntensity;
	GLuint mLightDirection;
};

/**
 * @brief Light pass shader for a point light.
 */
class PointLightShaderProgram : public Matrix_PVM_ShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new PointLightShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const PointLightShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	/// Default constructor.
	PointLightShaderProgram();

	/// Destructor.
	virtual ~PointLightShaderProgram() noexcept;

	#ifndef NDEBUG
	void setDebugFlag(const GLint value) const noexcept;
	#endif

	/**
	 * @brief Set the viewport size uniform variable.
	 * @details You can query the viewport size with the static function
	 * Renderer::viewport_size.
	 * 
	 * @param size The size of the viewport. Should always use
	 * Renderer::viewport_size unless you have a specific reason not to.
	 */
	void setViewportSize(const vec2f& size) const noexcept;

	/**
	 * @brief Set the geometry buffer position sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_POSITION.
	 * 
	 * @param texture_unit The value for the geometry buffer position sampler.
	 * Must always be equal to Renderer::GBUFFER_POSITION.
	 */
	void setGeometryBufferPositionTexture(const GLint texture_unit) const 
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
	void setGeometryBufferDiffuseTexture(const GLint texture_unit) const 
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
	void setGeometryBufferSpecularTexture(const GLint texture_unit) const 
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
	void setGeometryBufferNormalTexture(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void setLightIntensity(const vec4f& intensity) const 
		noexcept;

	/**
	 * @brief Set the light position uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.position".
	 * 
	 * @param position The position value.
	 */
	void setLightPosition(const vec3f& position) const 
		noexcept;

	/**
	 * @brief Set the light attenuation uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.attenuation".
	 * 
	 * @param attenuation The attenuation value.
	 */
	void setLightAttenuation(const vec4f& attenuation) const 
		noexcept;
protected:

	/// Forwarding constructor.
	PointLightShaderProgram(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	PointLightShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	static PointLightShaderProgram* sInstance;

	#ifndef NDEBUG
	GLuint mDebugFlag;
	#endif
	
	GLuint mViewportSize;
	GLuint mGeometryBufferPositionTexture;
	GLuint mGeometryBufferDiffuseTexture;
	GLuint mGeometryBufferSpecularTexture;
	GLuint mGeometryBufferNormalTexture;
	GLuint mLightIntensity;
	GLuint mLightPosition;
	GLuint mLightAttenuation;
};

/**
 * @brief Light pass shader for a spot light.
 */
class SpotLightShaderProgram : public Matrix_PVM_ShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new SpotLightShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const SpotLightShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	/// Default constructor.
	SpotLightShaderProgram();

	/// Destructor.
	virtual ~SpotLightShaderProgram() noexcept;

	/**
	 * @brief Set the viewport size uniform variable.
	 * @details You can query the viewport size with the static function
	 * Renderer::viewport_size.
	 * 
	 * @param size The size of the viewport. Should always use
	 * Renderer::viewport_size unless you have a specific reason not to.
	 */
	void setViewportSize(const vec2f& size) const noexcept;

	/**
	 * @brief Set the geometry buffer position sampler.
	 * 
	 * @details This value must always be equal to 
	 * Renderer::GBUFFER_POSITION.
	 * 
	 * @param texture_unit The value for the geometry buffer position sampler.
	 * Must always be equal to Renderer::GBUFFER_POSITION.
	 */
	void setGeometryBufferPositionTexture(const GLint texture_unit) const 
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
	void setGeometryBufferDiffuseTexture(const GLint texture_unit) const 
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
	void setGeometryBufferSpecularTexture(const GLint texture_unit) const 
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
	void setGeometryBufferNormalTexture(const GLint texture_unit) const 
		noexcept;

	/**
	 * @brief Set the light intensity uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.intensity".
	 * 
	 * @param intensity The intensity value.
	 */
	void setLightIntensity(const vec4f& intensity) const 
		noexcept;

	/**
	 * @brief Set the light position uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.position".
	 * 
	 * @param position The position value.
	 */
	void setLightPosition(const vec3f& position) const 
		noexcept;

	/**
	 * @brief Set the light direction uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.direction".
	 * 
	 * @param direction The direction value.
	 */
	void setLightDirection(const vec3f& direction) const noexcept;

	/**
	 * @brief Set the light attenuation uniform variable.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.attenuation".
	 * 
	 * @param attenuation The attenuation value.
	 */
	void setLightAttenuation(const vec4f& attenuation) const noexcept;

	/**
	 * @brief Set the spot light's cosine of the half angle.
	 * @details The name of the uniform in the shader is assumed to be equal
	 * to "light.cosineHalfAngle".
	 * 
	 * @param angle The spot light's cosine half angle.
	 */
	void setLightCosineHalfAngle(const GLfloat cosineHalfAngle) const noexcept;

	#ifndef NDEBUG
	/**
	 * @brief Set the spot light's debug flag.
	 * @details When set to 1, the shader adds a red hue. When set to 2,
	 * the shader adds a green hue. When any other value is supplied,
	 * nothing special is done in the shader. This method is only available
	 * in a debug build.
	 * 
	 * @param value The value for the debug flag. Either 1 or 2. All other
	 * values do nothing special.
	 */
	void setDebugFlag(const GLint value) const noexcept;
	#endif

protected:

	/// Forwarding constructor.
	SpotLightShaderProgram(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	SpotLightShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	static SpotLightShaderProgram* sInstance;

	GLuint mViewportSize;
	GLuint mGeometryBufferPositionTexture;
	GLuint mGeometryBufferDiffuseTexture;
	GLuint mGeometryBufferSpecularTexture;
	GLuint mGeometryBufferNormalTexture;
	GLuint mLightIntensity;
	GLuint mLightPosition;
	GLuint mLightDirection;
	GLuint mLightAttenuation;
	GLuint mLightCosineHalfAngle;

	#ifndef NDEBUG
	GLuint mDebugFlag;
	#endif
};

// /**
//  * @brief Shadow pass shader for a directional light.
//  */
// class sp_directional_shader : public Matrix_PVM_ShaderProgram
// {
// public:
// 	/// Default constructor.
// 	sp_directional_shader();

// 	/// Destructor.
// 	virtual ~sp_directional_shader() noexcept;
// };

/**
 * @brief Shader for a skybox.
 */
class SkyboxShaderProgram : public OpenGL::ShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new SkyboxShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const SkyboxShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	/// Default constructor.
	SkyboxShaderProgram();

	/// Destructor.
	virtual ~SkyboxShaderProgram() noexcept;

	/**
	 * @brief Set the matrix_PV uniform variable in the shader.
	 * 
	 * @param matrix The matrix_PV to set the uniform to.
	 */
	void setMatrixPV(const mat4f& matrix) const noexcept;

	/**
	 * @brief Set the skybox diffuse sampler.
	 * 
	 * @param texture_unit The value of the active texture that the skybox
	 * cube texture is bound for.
	 */
	void setDiffuseTexture(const GLint texture_unit) const noexcept;

protected:

	/// Forwarding constructor.
	SkyboxShaderProgram(
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path fragment_shader);

	/// Forwarding constructor.
	SkyboxShaderProgram( 
		boost::filesystem::path vertex_shader, 
		boost::filesystem::path geometry_shader,
		boost::filesystem::path fragment_shader);

private:

	static SkyboxShaderProgram* sInstance;

	GLuint mMatrixPV;
	GLuint mDiffuseTexture;
};

/**
 * @brief Simple shader for two-dimensional text.
 */
class FlatTextShaderProgram : public OpenGL::ShaderProgram
{
public:

	inline static void initialize()
	{
		delete sInstance;
		sInstance = new FlatTextShaderProgram();
	}

	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	inline static const FlatTextShaderProgram& get() noexcept
	{
		return *sInstance;
	}

	/// Default constructor.
	FlatTextShaderProgram();

	/// Destructor.
	virtual ~FlatTextShaderProgram() noexcept;

	/**
	 * @brief Set the text color uniform variable.
	 * 
	 * @param color The color of the text.
	 */
	void setColor(const vec3f& color) const noexcept;

	/**
	 * @brief Set the text texture.
	 * 
	 * @details You don't use this method manually, but instead you use the
	 * font mechanism (to be explained later).
	 * 
	 * @param texture_unit The same number as the active texture unit for the
	 * text.
	 */	
	void setTexture(const GLint texture_unit) const 
		noexcept;

private:

	static FlatTextShaderProgram* sInstance;
	
	GLuint mColor;
	GLuint mTexture;
};

} // namespace gintonic
