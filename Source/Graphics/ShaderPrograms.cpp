#include "ShaderPrograms.hpp"

namespace gintonic {

ShadowShaderProgram*           ShadowShaderProgram::sInstance           = nullptr;
MaterialShaderProgram*         MaterialShaderProgram::sInstance         = nullptr;
AmbientLightShaderProgram*     AmbientLightShaderProgram::sInstance     = nullptr;
DirectionalLightShaderProgram* DirectionalLightShaderProgram::sInstance = nullptr;
PointLightShaderProgram*       PointLightShaderProgram::sInstance       = nullptr;
SpotLightShaderProgram*        SpotLightShaderProgram::sInstance        = nullptr;
SkyboxShaderProgram*           SkyboxShaderProgram::sInstance           = nullptr;
FlatTextShaderProgram*         FlatTextShaderProgram::sInstance         = nullptr;

ShadowShaderProgram::ShadowShaderProgram()
: OpenGL::ShaderProgram("Shaders/Shadow.vert", "Shaders/Shadow.frag")
{
	mMatrixPVM = getUniformLocation("matrixPVM");
	mInstancedRendering = getUniformLocation("instancedRendering");
}

void ShadowShaderProgram::setMatrixPVM(const mat4f& value) const noexcept
{
	setUniform(mMatrixPVM, value);
}

void ShadowShaderProgram::setInstancedRendering(const GLint value) const noexcept
{
	setUniform(mInstancedRendering, value);
}

MaterialShaderProgram::MaterialShaderProgram()
: OpenGL::ShaderProgram("Shaders/Material.vert", "Shaders/Material.frag")
{
	mMatrixPVM = getUniformLocation("matrixPVM");
	mMatrixVM = getUniformLocation("matrixVM");
	mMatrixN = getUniformLocation("matrixN");
	mInstancedRendering = getUniformLocation("instancedRendering");
	mHasTangentsAndBitangents = getUniformLocation("hasTangentsAndBitangents");
	mMaterialDiffuseColor = getUniformLocation("materialDiffuseColor");
	mMaterialSpecularColor = getUniformLocation("materialSpecularColor");
	mMaterialDiffuseTexture = getUniformLocation("materialDiffuseTexture");
	mMaterialSpecularTexture = getUniformLocation("materialSpecularTexture");
	mMaterialNormalTexture = getUniformLocation("materialNormalTexture");
	mMaterialFlag = getUniformLocation("materialFlag");
}

void MaterialShaderProgram::setMatrixPVM(const mat4f& value) const noexcept
{
	setUniform(mMatrixPVM, value);
}

void MaterialShaderProgram::setMatrixVM(const mat4f& value) const noexcept
{
	setUniform(mMatrixVM, value);
}

void MaterialShaderProgram::setMatrixN(const mat3f& value) const noexcept
{
	setUniform(mMatrixN, value);
}

void MaterialShaderProgram::setInstancedRendering(const GLint value) const noexcept
{
	setUniform(mInstancedRendering, value);
}

void MaterialShaderProgram::setHasTangentsAndBitangents(const GLint value) const noexcept
{
	setUniform(mHasTangentsAndBitangents, value);
}

void MaterialShaderProgram::setMaterialDiffuseColor(const vec4f& value) const noexcept
{
	setUniform(mMaterialDiffuseColor, value);
}

void MaterialShaderProgram::setMaterialSpecularColor(const vec4f& value) const noexcept
{
	setUniform(mMaterialSpecularColor, value);
}

void MaterialShaderProgram::setMaterialDiffuseTexture(const GLint value) const noexcept
{
	setUniform(mMaterialDiffuseTexture, value);
}

void MaterialShaderProgram::setMaterialSpecularTexture(const GLint value) const noexcept
{
	setUniform(mMaterialSpecularTexture, value);
}

void MaterialShaderProgram::setMaterialNormalTexture(const GLint value) const noexcept
{
	setUniform(mMaterialNormalTexture, value);
}

void MaterialShaderProgram::setMaterialFlag(const GLint value) const noexcept
{
	setUniform(mMaterialFlag, value);
}

// /*****************************************************************************
//  * gintonic::Matrix_PVM_ShaderProgram                                        *
//  ****************************************************************************/

Matrix_PVM_ShaderProgram::Matrix_PVM_ShaderProgram()
: OpenGL::ShaderProgram("Shaders/LightPassPVM.vert", "Shaders/Null.frag")
{
	mMatrixPVM = getUniformLocation("matrix_PVM");
}

Matrix_PVM_ShaderProgram::Matrix_PVM_ShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	mMatrixPVM = getUniformLocation("matrix_PVM");
}

Matrix_PVM_ShaderProgram::Matrix_PVM_ShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	mMatrixPVM = getUniformLocation("matrix_PVM");
}

Matrix_PVM_ShaderProgram::~Matrix_PVM_ShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void Matrix_PVM_ShaderProgram::setMatrixPVM(const mat4f& m) const noexcept
{
	setUniform(mMatrixPVM, m);
}

// /*****************************************************************************
//  * gintonic::Matrix_PVM_VM_ShaderProgram                                     *
//  ****************************************************************************/

// Matrix_PVM_VM_ShaderProgram::Matrix_PVM_VM_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : Matrix_PVM_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mMatrixVM = getUniformLocation("matrix_VM");
// }

// Matrix_PVM_VM_ShaderProgram::Matrix_PVM_VM_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Matrix_PVM_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mMatrixVM = getUniformLocation("matrix_VM");
// }

// Matrix_PVM_VM_ShaderProgram::~Matrix_PVM_VM_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Matrix_PVM_VM_ShaderProgram::setMatrixVM(const mat4f& m) const noexcept
// {
// 	setUniform(mMatrixVM, m);
// }

// /*****************************************************************************
//  * gintonic::Matrix_PVM_V_N_ShaderProgram                                    *
//  ****************************************************************************/

// Matrix_PVM_V_N_ShaderProgram::Matrix_PVM_V_N_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : Matrix_PVM_VM_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mMatrixN = getUniformLocation("matrix_N");
// }

// Matrix_PVM_V_N_ShaderProgram::Matrix_PVM_V_N_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Matrix_PVM_VM_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mMatrixN = getUniformLocation("matrix_N");
// }

// Matrix_PVM_V_N_ShaderProgram::~Matrix_PVM_V_N_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Matrix_PVM_V_N_ShaderProgram::setMatrixN(const mat3f& m) const noexcept
// {
// 	setUniform(mMatrixN, m);
// }

// /*****************************************************************************
//  * gintonic::GeometryShaderProgram                                           *
//  ****************************************************************************/

// GeometryShaderProgram::GeometryShaderProgram()
// : Matrix_PVM_V_N_ShaderProgram("Shaders/Geometry.vert", "Shaders/Geometry.frag")
// {
// 	mDiffuseColor = getUniformLocation("material.diffuse_color");
// 	mSpecularColor = getUniformLocation("material.specular_color");
// }
// GeometryShaderProgram::~GeometryShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryShaderProgram::setDiffuseColor(const vec4f& color) const noexcept
// {
// 	setUniform(mDiffuseColor, color);
// }

// void GeometryShaderProgram::setSpecularColor(const vec4f& color) const noexcept
// {
// 	setUniform(mSpecularColor, color);
// }

// GeometryShaderProgram::GeometryShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : Matrix_PVM_V_N_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mDiffuseColor = getUniformLocation("material.diffuse_color");
// 	mSpecularColor = getUniformLocation("material.specular_color");
// }

// GeometryShaderProgram::GeometryShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Matrix_PVM_V_N_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mDiffuseColor = getUniformLocation("material.diffuse_color");
// 	mSpecularColor = getUniformLocation("material.specular_color");
// }

// /*****************************************************************************
//  * gintonic::Geometry_D_ShaderProgram                                        *
//  ****************************************************************************/

// Geometry_D_ShaderProgram::Geometry_D_ShaderProgram()
// : GeometryShaderProgram("Shaders/Geometry.vert", "Shaders/Geometry_D.frag")
// {
// 	mDiffuseTexture = getUniformLocation("material.diffuse_texture");
// }

// Geometry_D_ShaderProgram::~Geometry_D_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_D_ShaderProgram::setDiffuseTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mDiffuseTexture, texture_unit);
// }

// Geometry_D_ShaderProgram::Geometry_D_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mDiffuseTexture = getUniformLocation("material.diffuse_texture");
// }
// Geometry_D_ShaderProgram::Geometry_D_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mDiffuseTexture = getUniformLocation("material.diffuse_texture");
// }

// /*****************************************************************************
//  * gintonic::Geometry_S_ShaderProgram                                        *
//  ****************************************************************************/

// Geometry_S_ShaderProgram::Geometry_S_ShaderProgram()
// : GeometryShaderProgram("Shaders/Geometry.vert", "Shaders/Geometry_S.frag")
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// Geometry_S_ShaderProgram::~Geometry_S_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_S_ShaderProgram::setSpecularTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mSpecularTexture, texture_unit);
// }

// Geometry_S_ShaderProgram::Geometry_S_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// Geometry_S_ShaderProgram::Geometry_S_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// /*****************************************************************************
//  * gintonic::Geometry_N_ShaderProgram                                        *
//  ****************************************************************************/

// Geometry_N_ShaderProgram::Geometry_N_ShaderProgram()
// : GeometryShaderProgram("Shaders/Geometry_N.vert", "Shaders/Geometry_N.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_N_ShaderProgram::~Geometry_N_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_N_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// Geometry_N_ShaderProgram::Geometry_N_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_N_ShaderProgram::Geometry_N_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// /*****************************************************************************
//  * gintonic::Geometry_DS_ShaderProgram                                       *
//  ****************************************************************************/

// Geometry_DS_ShaderProgram::Geometry_DS_ShaderProgram()
// : Geometry_D_ShaderProgram("Shaders/Geometry.vert", "Shaders/Geometry_DS.frag")
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// Geometry_DS_ShaderProgram::~Geometry_DS_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_DS_ShaderProgram::setSpecularTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mSpecularTexture, texture_unit);
// }

// Geometry_DS_ShaderProgram::Geometry_DS_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : Geometry_D_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// Geometry_DS_ShaderProgram::Geometry_DS_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Geometry_D_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// /*****************************************************************************
//  * gintonic::Geometry_DN_ShaderProgram                                       *
//  ****************************************************************************/

// Geometry_DN_ShaderProgram::Geometry_DN_ShaderProgram()
// : Geometry_D_ShaderProgram("Shaders/Geometry_N.vert", "Shaders/Geometry_DN.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_DN_ShaderProgram::~Geometry_DN_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_DN_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// Geometry_DN_ShaderProgram::Geometry_DN_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : Geometry_D_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_DN_ShaderProgram::Geometry_DN_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Geometry_D_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// /*****************************************************************************
//  * gintonic::Geometry_SN_ShaderProgram                                       *
//  ****************************************************************************/

// Geometry_SN_ShaderProgram::Geometry_SN_ShaderProgram()
// : Geometry_S_ShaderProgram("Shaders/Geometry_N.vert", "Shaders/Geometry_SN.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_SN_ShaderProgram::~Geometry_SN_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_SN_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// Geometry_SN_ShaderProgram::Geometry_SN_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : Geometry_S_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_SN_ShaderProgram::Geometry_SN_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Geometry_S_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// ****************************************************************************
//  * gintonic::Geometry_DSN_ShaderProgram                                      *
//  ***************************************************************************

// Geometry_DSN_ShaderProgram::Geometry_DSN_ShaderProgram()
// : Geometry_DS_ShaderProgram("Shaders/Geometry_N.vert", "Shaders/Geometry_DSN.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_DSN_ShaderProgram::~Geometry_DSN_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void Geometry_DSN_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// Geometry_DSN_ShaderProgram::Geometry_DSN_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// Geometry_DSN_ShaderProgram::Geometry_DSN_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : Geometry_DS_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstancedShaderProgram                                  *
//  ****************************************************************************/

// GeometryInstancedShaderProgram::GeometryInstancedShaderProgram()
// : OpenGL::ShaderProgram("Shaders/GeometryInstanced.vert", "Shaders/Geometry.frag")
// {
// 	mDiffuseColor = getUniformLocation("material.diffuse_color");
// 	mSpecularColor = getUniformLocation("material.specular_color");
// }
// GeometryInstancedShaderProgram::~GeometryInstancedShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstancedShaderProgram::setDiffuseColor(const vec4f& color) const noexcept
// {
// 	setUniform(mDiffuseColor, color);
// }

// void GeometryInstancedShaderProgram::setSpecularColor(const vec4f& color) const noexcept
// {
// 	setUniform(mSpecularColor, color);
// }

// GeometryInstancedShaderProgram::GeometryInstancedShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mDiffuseColor = getUniformLocation("material.diffuse_color");
// 	mSpecularColor = getUniformLocation("material.specular_color");
// }

// GeometryInstancedShaderProgram::GeometryInstancedShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mDiffuseColor = getUniformLocation("material.diffuse_color");
// 	mSpecularColor = getUniformLocation("material.specular_color");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_D_ShaderProgram                               *
//  ****************************************************************************/

// GeometryInstanced_D_ShaderProgram::GeometryInstanced_D_ShaderProgram()
// : GeometryInstancedShaderProgram("Shaders/GeometryInstanced.vert", "Shaders/Geometry_D.frag")
// {
// 	mDiffuseTexture = getUniformLocation("material.diffuse_texture");
// }

// GeometryInstanced_D_ShaderProgram::~GeometryInstanced_D_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_D_ShaderProgram::setDiffuseTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mDiffuseTexture, texture_unit);
// }

// GeometryInstanced_D_ShaderProgram::GeometryInstanced_D_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryInstancedShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mDiffuseTexture = getUniformLocation("material.diffuse_texture");
// }
// GeometryInstanced_D_ShaderProgram::GeometryInstanced_D_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstancedShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mDiffuseTexture = getUniformLocation("material.diffuse_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_S_ShaderProgram                               *
//  ****************************************************************************/

// GeometryInstanced_S_ShaderProgram::GeometryInstanced_S_ShaderProgram()
// : GeometryInstancedShaderProgram("Shaders/GeometryInstanced.vert", "Shaders/Geometry_S.frag")
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// GeometryInstanced_S_ShaderProgram::~GeometryInstanced_S_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_S_ShaderProgram::setSpecularTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mSpecularTexture, texture_unit);
// }

// GeometryInstanced_S_ShaderProgram::GeometryInstanced_S_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryInstancedShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// GeometryInstanced_S_ShaderProgram::GeometryInstanced_S_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstancedShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_N_ShaderProgram                               *
//  ****************************************************************************/

// GeometryInstanced_N_ShaderProgram::GeometryInstanced_N_ShaderProgram()
// : GeometryInstancedShaderProgram("Shaders/GeometryInstanced_N.vert", "Shaders/Geometry_N.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_N_ShaderProgram::~GeometryInstanced_N_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_N_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// GeometryInstanced_N_ShaderProgram::GeometryInstanced_N_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryInstancedShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_N_ShaderProgram::GeometryInstanced_N_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstancedShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_DS_ShaderProgram                              *
//  ****************************************************************************/

// GeometryInstanced_DS_ShaderProgram::GeometryInstanced_DS_ShaderProgram()
// : GeometryInstanced_D_ShaderProgram("Shaders/GeometryInstanced.vert", "Shaders/Geometry_DS.frag")
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// GeometryInstanced_DS_ShaderProgram::~GeometryInstanced_DS_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_DS_ShaderProgram::setSpecularTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mSpecularTexture, texture_unit);
// }

// GeometryInstanced_DS_ShaderProgram::GeometryInstanced_DS_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_D_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// GeometryInstanced_DS_ShaderProgram::GeometryInstanced_DS_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_D_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mSpecularTexture = getUniformLocation("material.specular_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_DN_ShaderProgram                              *
//  ****************************************************************************/

// GeometryInstanced_DN_ShaderProgram::GeometryInstanced_DN_ShaderProgram()
// : GeometryInstanced_D_ShaderProgram("Shaders/GeometryInstanced_N.vert", "Shaders/Geometry_DN.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_DN_ShaderProgram::~GeometryInstanced_DN_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_DN_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// GeometryInstanced_DN_ShaderProgram::GeometryInstanced_DN_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_D_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_DN_ShaderProgram::GeometryInstanced_DN_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_D_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_SN_ShaderProgram                              *
//  ****************************************************************************/

// GeometryInstanced_SN_ShaderProgram::GeometryInstanced_SN_ShaderProgram()
// : GeometryInstanced_S_ShaderProgram("Shaders/GeometryInstanced_N.vert", "Shaders/Geometry_SN.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_SN_ShaderProgram::~GeometryInstanced_SN_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_SN_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// GeometryInstanced_SN_ShaderProgram::GeometryInstanced_SN_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_S_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_SN_ShaderProgram::GeometryInstanced_SN_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_S_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// /*****************************************************************************
//  * gintonic::GeometryInstanced_DSN_ShaderProgram                             *
//  ****************************************************************************/

// GeometryInstanced_DSN_ShaderProgram::GeometryInstanced_DSN_ShaderProgram()
// : GeometryInstanced_DS_ShaderProgram("Shaders/GeometryInstanced_N.vert", "Shaders/Geometry_DSN.frag")
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_DSN_ShaderProgram::~GeometryInstanced_DSN_ShaderProgram() noexcept
// {
// 	/* Empty on purpose. */
// }

// void GeometryInstanced_DSN_ShaderProgram::setNormalTexture(const GLint texture_unit) const noexcept
// {
// 	setUniform(mNormalTexture, texture_unit);
// }

// GeometryInstanced_DSN_ShaderProgram::GeometryInstanced_DSN_ShaderProgram(
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path fragment_shader)
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

// GeometryInstanced_DSN_ShaderProgram::GeometryInstanced_DSN_ShaderProgram( 
// 	boost::filesystem::path vertex_shader, 
// 	boost::filesystem::path geometry_shader,
// 	boost::filesystem::path fragment_shader)
// : GeometryInstanced_DS_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
// {
// 	mNormalTexture = getUniformLocation("material.normal_texture");
// }

/*****************************************************************************
 * gintonic::LightShaderProgram                                                  *
 ****************************************************************************/

LightShaderProgram::LightShaderProgram()
: OpenGL::ShaderProgram("Shaders/LightPass.vert", "Shaders/Null.frag")
{
	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
}

LightShaderProgram::~LightShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void LightShaderProgram::setGeometryBufferDiffuseTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferDiffuseTexture, texture_unit);
}

void LightShaderProgram::setViewportSize(const vec2f& size) const noexcept
{
	setUniform(mViewportSize, size);
}

LightShaderProgram::LightShaderProgram(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
}

LightShaderProgram::LightShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
}

/*****************************************************************************
 * gintonic::AmbientLightShaderProgram                                       *
 ****************************************************************************/

AmbientLightShaderProgram::AmbientLightShaderProgram()
: LightShaderProgram("Shaders/LightPass.vert", "Shaders/AmbientLight.frag")
{
	mLightIntensity = getUniformLocation("light.intensity");
}

AmbientLightShaderProgram::~AmbientLightShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void AmbientLightShaderProgram::setLightIntensity(const vec4f& intensity) const noexcept
{
	setUniform(mLightIntensity, intensity);
}

AmbientLightShaderProgram::AmbientLightShaderProgram(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: LightShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	mLightIntensity = getUniformLocation("light.intensity");
}

AmbientLightShaderProgram::AmbientLightShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: LightShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	mLightIntensity = getUniformLocation("light.intensity");
}

/*****************************************************************************
 * gintonic::DirectionalLightShaderProgram                                   *
 ****************************************************************************/

DirectionalLightShaderProgram::DirectionalLightShaderProgram()
: LightShaderProgram("Shaders/LightPass.vert", "Shaders/DirectionalLight.frag")
{
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightDirection = getUniformLocation("light.direction");
}

DirectionalLightShaderProgram::~DirectionalLightShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void DirectionalLightShaderProgram::setGeometryBufferPositionTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferPositionTexture, texture_unit);
}

void DirectionalLightShaderProgram::setGeometryBufferSpecularTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferSpecularTexture, texture_unit);
}

void DirectionalLightShaderProgram::setGeometryBufferNormalTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferNormalTexture, texture_unit);
}

void DirectionalLightShaderProgram::setLightIntensity(const vec4f& intensity) const noexcept
{
	setUniform(mLightIntensity, intensity);
}

void DirectionalLightShaderProgram::setLightDirection(const vec3f& direction) const noexcept
{
	setUniform(mLightDirection, direction);
}

DirectionalLightShaderProgram::DirectionalLightShaderProgram(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: LightShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightDirection = getUniformLocation("light.direction");
}

DirectionalLightShaderProgram::DirectionalLightShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: LightShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightDirection = getUniformLocation("light.direction");
}

/*****************************************************************************
 * gintonic::PointLightShaderProgram                                         *
 ****************************************************************************/

PointLightShaderProgram::PointLightShaderProgram()
: Matrix_PVM_ShaderProgram("Shaders/LightPassPVM.vert", "Shaders/PointLight.frag")
{
	#ifndef NDEBUG
	mDebugFlag = getUniformLocation("debugflag");
	#endif

	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightPosition = getUniformLocation("light.position");
	mLightAttenuation = getUniformLocation("light.attenuation");
}

PointLightShaderProgram::~PointLightShaderProgram() noexcept
{
	/* Empty on purpose. */
}

#ifndef NDEBUG
void PointLightShaderProgram::setDebugFlag(const GLint value) const noexcept
{
	setUniform(mDebugFlag, value);
}
#endif

void PointLightShaderProgram::setViewportSize(const vec2f& size) const noexcept
{
	setUniform(mViewportSize, size);
}

void PointLightShaderProgram::setGeometryBufferPositionTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferPositionTexture, texture_unit);
}

void PointLightShaderProgram::setGeometryBufferDiffuseTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferDiffuseTexture, texture_unit);
}

void PointLightShaderProgram::setGeometryBufferSpecularTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferSpecularTexture, texture_unit);
}

void PointLightShaderProgram::setGeometryBufferNormalTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferNormalTexture, texture_unit);
}

void PointLightShaderProgram::setLightIntensity(const vec4f& intensity) const noexcept
{
	setUniform(mLightIntensity, intensity);
}

void PointLightShaderProgram::setLightPosition(const vec3f& position) const noexcept
{
	setUniform(mLightPosition, position);
}

void PointLightShaderProgram::setLightAttenuation(const vec4f& attenuation) const noexcept
{
	setUniform(mLightAttenuation, attenuation);
}

PointLightShaderProgram::PointLightShaderProgram(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: Matrix_PVM_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	#ifndef NDEBUG
	mDebugFlag = getUniformLocation("debugflag");
	#endif

	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightPosition = getUniformLocation("light.position");
	mLightAttenuation = getUniformLocation("light.attenuation");
}

PointLightShaderProgram::PointLightShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: Matrix_PVM_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	#ifndef NDEBUG
	mDebugFlag = getUniformLocation("debugflag");
	#endif

	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightPosition = getUniformLocation("light.position");
	mLightAttenuation = getUniformLocation("light.attenuation");
}

/*****************************************************************************
 * gintonic::SpotLightShaderProgram                                          *
 ****************************************************************************/

SpotLightShaderProgram::SpotLightShaderProgram()
: Matrix_PVM_ShaderProgram("Shaders/LightPassPVM.vert", "Shaders/SpotLight.frag")
{
	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightPosition = getUniformLocation("light.position");
	mLightDirection = getUniformLocation("light.direction");
	mLightAttenuation = getUniformLocation("light.attenuation");
	mLightCosineHalfAngle = getUniformLocation("light.angle");

	#ifndef NDEBUG
	mDebugFlag = getUniformLocation("debugflag");
	#endif
}

SpotLightShaderProgram::~SpotLightShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void SpotLightShaderProgram::setViewportSize(const vec2f& size) const noexcept
{
	setUniform(mViewportSize, size);
}

void SpotLightShaderProgram::setGeometryBufferPositionTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferPositionTexture, texture_unit);
}

void SpotLightShaderProgram::setGeometryBufferDiffuseTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferDiffuseTexture, texture_unit);
}

void SpotLightShaderProgram::setGeometryBufferSpecularTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferSpecularTexture, texture_unit);
}

void SpotLightShaderProgram::setGeometryBufferNormalTexture(const GLint texture_unit) const noexcept
{
	setUniform(mGeometryBufferNormalTexture, texture_unit);
}

void SpotLightShaderProgram::setLightIntensity(const vec4f& intensity) const noexcept
{
	setUniform(mLightIntensity, intensity);
}

void SpotLightShaderProgram::setLightPosition(const vec3f& position) const noexcept
{
	setUniform(mLightPosition, position);
}

void SpotLightShaderProgram::setLightDirection(const vec3f& direction) const noexcept
{
	setUniform(mLightDirection, direction);
}

void SpotLightShaderProgram::setLightAttenuation(const vec4f& attenuation) const noexcept
{
	setUniform(mLightAttenuation, attenuation);
}

void SpotLightShaderProgram::setLightCosineHalfAngle(const GLfloat angle) const noexcept
{
	setUniform(mLightCosineHalfAngle, angle);
}

#ifndef NDEBUG
void SpotLightShaderProgram::setDebugFlag(const GLint value) const noexcept
{
	setUniform(mDebugFlag, value);
}
#endif

SpotLightShaderProgram::SpotLightShaderProgram(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: Matrix_PVM_ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightPosition = getUniformLocation("light.position");
	mLightDirection = getUniformLocation("light.direction");
	mLightAttenuation = getUniformLocation("light.attenuation");
	mLightCosineHalfAngle = getUniformLocation("light.angle");

	#ifndef NDEBUG
	mDebugFlag = getUniformLocation("debugflag");
	#endif
}

SpotLightShaderProgram::SpotLightShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: Matrix_PVM_ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	mViewportSize = getUniformLocation("viewport_size");
	mGeometryBufferPositionTexture = getUniformLocation("gbuffer.position");
	mGeometryBufferDiffuseTexture = getUniformLocation("gbuffer.diffuse");
	mGeometryBufferSpecularTexture = getUniformLocation("gbuffer.specular");
	mGeometryBufferNormalTexture = getUniformLocation("gbuffer.normal");
	mLightIntensity = getUniformLocation("light.intensity");
	mLightPosition = getUniformLocation("light.position");
	mLightDirection = getUniformLocation("light.direction");
	mLightAttenuation = getUniformLocation("light.attenuation");
	mLightCosineHalfAngle = getUniformLocation("light.angle");

	#ifndef NDEBUG
	mDebugFlag = getUniformLocation("debugflag");
	#endif
}

/*****************************************************************************
 * gintonic::sp_directional_shader                                           *
 ****************************************************************************/

// sp_directional_shader::sp_directional_shader()
// : Matrix_PVM_ShaderProgram("Shaders/LightPassPVM.vert", "Shaders/Null.frag")
// {
// 	/* Empty on purpose. */
// }
// sp_directional_shader::~sp_directional_shader() noexcept
// {
// 	/* Empty on purpose. */
// }

/*****************************************************************************
 * gintonic::SkyboxShaderProgram                                             *
 ****************************************************************************/

SkyboxShaderProgram::SkyboxShaderProgram()
: OpenGL::ShaderProgram("Shaders/Skybox.vert", "Shaders/Skybox.frag")
{
	mMatrixPV = getUniformLocation("matrix_PV");
	mDiffuseTexture = getUniformLocation("skybox.diffuse_texture");
}

SkyboxShaderProgram::~SkyboxShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void SkyboxShaderProgram::setMatrixPV(const mat4f& matrix_PV) const noexcept
{
	setUniform(mMatrixPV, matrix_PV);
}

void SkyboxShaderProgram::setDiffuseTexture(const GLint texture_unit) const noexcept
{
	setUniform(mDiffuseTexture, texture_unit);
}

SkyboxShaderProgram::SkyboxShaderProgram(
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(fragment_shader))
{
	mMatrixPV = getUniformLocation("matrix_PV");
	mDiffuseTexture = getUniformLocation("skybox.diffuse_texture");
}

SkyboxShaderProgram::SkyboxShaderProgram( 
	boost::filesystem::path vertex_shader, 
	boost::filesystem::path geometry_shader,
	boost::filesystem::path fragment_shader)
: OpenGL::ShaderProgram(std::move(vertex_shader), std::move(geometry_shader), std::move(fragment_shader))
{
	mMatrixPV = getUniformLocation("matrix_PV");
	mDiffuseTexture = getUniformLocation("skybox.diffuse_texture");
}

/*****************************************************************************
 * gintonic::FlatTextShaderProgram                                           *
 ****************************************************************************/

FlatTextShaderProgram::FlatTextShaderProgram()
: OpenGL::ShaderProgram("Shaders/FlatTextUniformColor.vert", "Shaders/FlatTextUniformColor.frag")
{
	mColor = getUniformLocation("color");
	mTexture = getUniformLocation("tex");
}

FlatTextShaderProgram::~FlatTextShaderProgram() noexcept
{
	/* Empty on purpose. */
}

void FlatTextShaderProgram::setColor(const vec3f& color) const noexcept
{
	setUniform(mColor, color);
}

void FlatTextShaderProgram::setTexture(const GLint tex) const noexcept
{
	setUniform(mTexture, tex);
}

} // namespace gintonic