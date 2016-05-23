#include "Material.hpp"

#include "../Foundation/exception.hpp"
#include "../Foundation/portable_iarchive.hpp"
#include "../Foundation/portable_oarchive.hpp"

#include "../Math/vec3f.hpp"

// #include "ShaderPrograms.hpp"
// #include "Renderer.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>

#include <fbxsdk.h>

namespace { // anonymous namespace

std::size_t textureCount(
	const FbxSurfaceMaterial* mat, 
	const char* property)
{
	const auto prop = mat->FindProperty(property);
	return prop.GetSrcObjectCount<FbxFileTexture>();
}

FbxTexture* getFbxTexture(
	const FbxSurfaceMaterial* pFbxMaterial,
	const char* property,
	const std::size_t index)
{
	const auto prop = pFbxMaterial->FindProperty(property);
	return FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(index));
}

float getTextureFactor(
	const FbxSurfaceMaterial* mat,
	const char* property)
{
	const auto prop = mat->FindProperty(property);
	return static_cast<float>(prop.Get<FbxDouble>());
}

gintonic::vec3f getMaterialColor(
	const FbxSurfaceMaterial* mat,
	const char* property)
{
	const auto prop = mat->FindProperty(property);
	const auto fbx_color = prop.Get<FbxDouble3>();
	return gintonic::vec3f(static_cast<float>(fbx_color[0]), static_cast<float>(fbx_color[1]), static_cast<float>(fbx_color[2]));
}

} // anonymous namespace

namespace gintonic {

Material::Material(
	const vec4f& diffuseColor)
: diffuseColor(diffuseColor)
, specularColor(0.0f, 0.0f, 0.0f, 0.0f)
{
	/* Empty on purpose. */
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	/* Empty on purpose. */
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor,
	const boost::filesystem::path& diffuse_texture)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	diffuseTexture = Texture2D::create(diffuse_texture);
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor,
	const boost::filesystem::path& diffuse_texture,
	const boost::filesystem::path& specular_texture)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	diffuseTexture = Texture2D::create(diffuse_texture);
	specularTexture = Texture2D::create(specular_texture);
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor,
	const boost::filesystem::path& diffuse_texture,
	const boost::filesystem::path& specular_texture,
	const boost::filesystem::path& normal_texture)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	diffuseTexture = Texture2D::create(diffuse_texture);
	specularTexture = Texture2D::create(specular_texture);
	normalTexture = Texture2D::create(normal_texture);
}

Material::Material(
	const vec4f& diffuseColor,
	const vec4f& specularColor,
	std::shared_ptr<Texture2D> diffuse_texture,
	std::shared_ptr<Texture2D> specular_texture,
	std::shared_ptr<Texture2D> normal_texture)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
, diffuseTexture(std::move(diffuse_texture))
, specularTexture(std::move(specular_texture))
, normalTexture(std::move(normal_texture))
{
	/* Empty on purpose. */
}

Material::Material(const FbxSurfaceMaterial* pFbxMaterial)
: Object<Material, std::string>(pFbxMaterial)
{
	std::cerr << "\tFound Material: " << pFbxMaterial->GetName() << '\n';

	const auto lNumDiffuseTextures  = textureCount(pFbxMaterial, FbxSurfaceMaterial::sDiffuse);
	const auto lNumSpecularTextures = textureCount(pFbxMaterial, FbxSurfaceMaterial::sSpecular);
	const auto lNumNormalTextures   = textureCount(pFbxMaterial, FbxSurfaceMaterial::sNormalMap);

	const auto lDiffuseFactor  = getTextureFactor(pFbxMaterial, FbxSurfaceMaterial::sDiffuseFactor);
	const auto lSpecularFactor = getTextureFactor(pFbxMaterial, FbxSurfaceMaterial::sSpecularFactor);
	const auto lShininess      = getTextureFactor(pFbxMaterial, FbxSurfaceMaterial::sShininess);

	diffuseColor = vec4f(getMaterialColor(pFbxMaterial, FbxSurfaceMaterial::sDiffuse), lDiffuseFactor);
	specularColor = vec4f(getMaterialColor(pFbxMaterial, FbxSurfaceMaterial::sSpecular), 1.0f);

	specularColor *= lSpecularFactor;
	specularColor.w = lShininess;

	std::cerr << "\tDiffuse color: " << diffuseColor << '\n';
	std::cerr << "\tSpecular color: " << specularColor << '\n';

	if (lNumDiffuseTextures)
	{
		auto pFbxTexture = getFbxTexture(pFbxMaterial, FbxSurfaceMaterial::sDiffuse, 0);
		diffuseTexture = Texture2D::create(pFbxTexture);
		std::cerr << "\tDiffuse texture: " << diffuseTexture->name << '\n';
	}
	else
	{
		std::cerr << "\tNo diffuse texture present.\n";
	}
	if (lNumSpecularTextures)
	{
		auto pFbxTexture = getFbxTexture(pFbxMaterial, FbxSurfaceMaterial::sSpecular, 0);
		specularTexture = Texture2D::create(pFbxTexture);
		std::cerr << "\tSpecular texture: " << specularTexture->name << '\n';
	}
	else
	{
		std::cerr << "\tNo specular texture present.\n";
	}
	if (lNumNormalTextures)
	{
		auto pFbxTexture = getFbxTexture(pFbxMaterial, FbxSurfaceMaterial::sNormalMap, 0);
		normalTexture = Texture2D::create(pFbxTexture);
		std::cerr << "\tNormal texture: " << normalTexture->name << '\n';
	}
	else
	{
		std::cerr << "\tNo normal map present.\n";
	}
}

// void Material::bind(const bool instanced) const noexcept
// {
// 	#define HAS_DIFFUSE_TEXTURE 1
// 	#define HAS_SPECULAR_TEXTURE 2
// 	#define HAS_NORMAL_TEXTURE 4

// 	GLint lMaterialFlags(0);
// 	const auto& lMaterialShaderProgram = Renderer::getMaterialShaderProgram();
// 	lMaterialShaderProgram.setDiffuseColor(diffuseColor);
// 	lMaterialShaderProgram.setSpecularColor(specularColor);
// 	if (diffuseTexture)
// 	{
// 		lMaterialFlags |= HAS_DIFFUSE_TEXTURE;
// 		lMaterialShaderProgram.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 	}
// 	if (specularTexture)
// 	{
// 		lMaterialFlags |= HAS_SPECULAR_TEXTURE;
// 		lMaterialShaderProgram.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 	}
// 	if (normalTexture)
// 	{
// 		lMaterialFlags |= HAS_NORMAL_TEXTURE;
// 		lMaterialShaderProgram.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 	}
// 	lMaterialShaderProgram.setFlags(lMaterialFlags);

// 	if (instanced)
// 	{
// 		if (diffuseTexture)
// 		{
// 			if (specularTexture)
// 			{
// 				if (normalTexture)
// 				{
// 					// diffuse, specular, normal textures
// 					const auto& s = Renderer::getGeometryInstanced_DSN_ShaderProgram();
// 					s.activate();
// 					s.setDiffuseColor(diffuseColor);
// 					s.setSpecularColor(specularColor);
// 					s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 					s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 					s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 					// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 					// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 					diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 					normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 				}
// 				else
// 				{
// 					// diffuse and specular textures
// 					const auto& s = Renderer::getGeometryInstanced_DS_ShaderProgram();
// 					s.activate();
// 					s.setDiffuseColor(diffuseColor);
// 					s.setSpecularColor(specularColor);
// 					s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 					s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 					// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 					diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				}
// 			}
// 			else if (normalTexture)
// 			{
// 				// diffuse and normal textures
// 				const auto& s = Renderer::getGeometryInstanced_DN_ShaderProgram();
// 				s.activate();
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 				s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 				// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 				// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 				diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 				normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 			}
// 			else
// 			{
// 				// diffuse texture
// 				const auto& s = Renderer::getGeometryInstanced_D_ShaderProgram();
// 				s.activate();
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 				// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 				diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 			}
// 		}
// 		else if (specularTexture)
// 		{
// 			if (normalTexture)
// 			{
// 				// specular and normal textures
// 				const auto& s = Renderer::getGeometryInstanced_SN_ShaderProgram();
// 				s.activate();
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 				s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 				// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 				specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 			}
// 			else
// 			{
// 				// specular texture
// 				const auto& s = Renderer::getGeometryInstanced_S_ShaderProgram();
// 				s.activate();
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 				// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 			}
// 		}
// 		else if (normalTexture)
// 		{
// 			// normal texture
// 			const auto& s = Renderer::getGeometryInstanced_N_ShaderProgram();
// 			s.activate();
// 			s.setDiffuseColor(diffuseColor);
// 			s.setSpecularColor(specularColor);
// 			s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 			// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 			normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 		}
// 		else
// 		{
// 			// no textures at all
// 			const auto& s = Renderer::getGeometryInstanced_ShaderProgram();
// 			s.activate();
// 			s.setDiffuseColor(diffuseColor);
// 			s.setSpecularColor(specularColor);
// 		}
// 	}
// 	else // non-instanced rendering
// 	{
// 		if (diffuseTexture)
// 		{
// 			if (specularTexture)
// 			{
// 				if (normalTexture)
// 				{
// 					// diffuse, specular, normal textures
// 					const auto& s = Renderer::getGeometry_DSN_ShaderProgram();
// 					s.activate();
// 					s.set_matrix_PVM(Renderer::matrix_PVM());
// 					s.set_matrix_VM(Renderer::matrix_VM());
// 					s.set_matrix_N(Renderer::matrix_N());
// 					s.setDiffuseColor(diffuseColor);
// 					s.setSpecularColor(specularColor);
// 					s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 					s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 					s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 					// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 					// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 					diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 					normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 				}
// 				else
// 				{
// 					// diffuse and specular textures
// 					const auto& s = Renderer::getGeometry_DS_ShaderProgram();
// 					s.activate();
// 					s.set_matrix_PVM(Renderer::matrix_PVM());
// 					s.set_matrix_VM(Renderer::matrix_VM());
// 					s.set_matrix_N(Renderer::matrix_N());
// 					s.setDiffuseColor(diffuseColor);
// 					s.setSpecularColor(specularColor);
// 					s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 					s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 					// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 					diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 					specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				}
// 			}
// 			else if (normalTexture)
// 			{
// 				// diffuse and normal textures
// 				const auto& s = Renderer::getGeometry_DN_ShaderProgram();
// 				s.activate();
// 				s.set_matrix_PVM(Renderer::matrix_PVM());
// 				s.set_matrix_VM(Renderer::matrix_VM());
// 				s.set_matrix_N(Renderer::matrix_N());
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 				s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 				// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 				// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 				diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 				normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 			}
// 			else
// 			{
// 				// diffuse texture
// 				const auto& s = Renderer::getGeometry_D_ShaderProgram();
// 				s.activate();
// 				s.set_matrix_PVM(Renderer::matrix_PVM());
// 				s.set_matrix_VM(Renderer::matrix_VM());
// 				s.set_matrix_N(Renderer::matrix_N());
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setDiffuseTexture(Renderer::GBUFFER_TEX_DIFFUSE);
// 				// std::get<2>(*mDiffuseTexture).bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 				diffuseTexture->bind(Renderer::GBUFFER_TEX_DIFFUSE);
// 			}
// 		}
// 		else if (specularTexture)
// 		{
// 			if (normalTexture)
// 			{
// 				// specular and normal textures
// 				const auto& s = Renderer::getGeometry_SN_ShaderProgram();
// 				s.activate();
// 				s.set_matrix_PVM(Renderer::matrix_PVM());
// 				s.set_matrix_VM(Renderer::matrix_VM());
// 				s.set_matrix_N(Renderer::matrix_N());
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 				s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 				// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 				specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 			}
// 			else
// 			{
// 				// specular texture
// 				const auto& s = Renderer::getGeometry_S_ShaderProgram();
// 				s.activate();
// 				s.set_matrix_PVM(Renderer::matrix_PVM());
// 				s.set_matrix_VM(Renderer::matrix_VM());
// 				s.set_matrix_N(Renderer::matrix_N());
// 				s.setDiffuseColor(diffuseColor);
// 				s.setSpecularColor(specularColor);
// 				s.setSpecularTexture(Renderer::GBUFFER_TEX_SPECULAR);
// 				// std::get<2>(*mSpecularTexture).bind(Renderer::GBUFFER_TEX_SPECULAR);
// 				specularTexture->bind(Renderer::GBUFFER_TEX_SPECULAR);

// 			}
// 		}
// 		else if (normalTexture)
// 		{
// 			// normal texture
// 			const auto& s = Renderer::getGeometry_N_ShaderProgram();
// 			s.activate();
// 			s.set_matrix_PVM(Renderer::matrix_PVM());
// 			s.set_matrix_VM(Renderer::matrix_VM());
// 			s.set_matrix_N(Renderer::matrix_N());
// 			s.setDiffuseColor(diffuseColor);
// 			s.setSpecularColor(specularColor);
// 			s.setNormalTexture(Renderer::GBUFFER_TEX_NORMAL);
// 			// std::get<2>(*mNormalTexture).bind(Renderer::GBUFFER_TEX_NORMAL);
// 			normalTexture->bind(Renderer::GBUFFER_TEX_NORMAL);
// 		}
// 		else
// 		{
// 			// no textures at all
// 			const auto& s = Renderer::getGeometry_ShaderProgram();
// 			s.activate();
// 			s.set_matrix_PVM(Renderer::matrix_PVM());
// 			s.set_matrix_VM(Renderer::matrix_VM());
// 			s.set_matrix_N(Renderer::matrix_N());
// 			s.setDiffuseColor(diffuseColor);
// 			s.setSpecularColor(specularColor);
// 		}
// 	}
// }

std::ostream& operator << (std::ostream& os, const Material& lMaterial)
{
	os << "{ diffuseColor: " << lMaterial.diffuseColor << ", specularColor: "
		<< lMaterial.specularColor;

	if (lMaterial.diffuseTexture)
	{
		os << ", diffuseTexture: " << lMaterial.diffuseTexture->name;
	}
	if (lMaterial.specularTexture)
	{
		os << ", specularTexture: " << lMaterial.specularTexture->name;
	}
	if (lMaterial.normalTexture)
	{
		os << ", normalTexture: " << lMaterial.normalTexture->name;
	}
	os << " }";
	return os;
}

} // namespace gintonic
