#include "materials.hpp"
#include "entity.hpp"
#include "shaders.hpp"
#include "renderer.hpp"
#include "exception.hpp"
#include "portable_iarchive.hpp"
#include "portable_oarchive.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
// #include <boost/archive/xml_iarchive.hpp>
// #include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>
#include <fbxsdk.h>

using FBX::FbxSurfaceMaterial;
using FBX::FbxSurfaceLambert;
using FBX::FbxSurfacePhong;
using FBX::FbxTexture;
using FBX::FbxFileTexture;
using FBX::FbxCast;
using FBX::FbxDouble;
using FBX::FbxDouble3;

namespace { // anonymous namespace

std::size_t textureCount(
	const FbxSurfaceMaterial* mat, 
	const char* property)
{
	const auto prop = mat->FindProperty(property);
	return prop.GetSrcObjectCount<FbxFileTexture>();
}

boost::filesystem::path getTextureFilename(
	const FbxSurfaceMaterial* mat, 
	const char* property, 
	const std::size_t index)
{
	const auto prop = mat->FindProperty(property);
	const auto file_prop = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(index));
	return file_prop->GetFileName();
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

/*****************************************************************************
 * gintonic::Material (base class for inheritance)                           *
 ****************************************************************************/

Material::DataStructureType Material::sTextures;

write_lock Material::sTextureWriteLock;

void Material::save(std::ostream& output) const
{
	eos::portable_oarchive archive(output);
	archive & boost::serialization::make_nvp("Material", *this);
}

void Material::save(const boost::filesystem::path& filename) const
{
	#ifdef BOOST_MSVC
		save(filename.string());
	#else
		save(filename.c_str());
	#endif
}

void Material::save(const std::string& filename) const
{
	save(filename.c_str());
}

void Material::save(const char* filename) const
{
	std::ofstream output(filename, std::ios::binary);
	save(output);
}

Material Material::load(std::istream& input)
{
	Material m;
	eos::portable_iarchive archive(input);
	archive & boost::serialization::make_nvp("Material", m);
	return m;
}

Material Material::load(const boost::filesystem::path& filename)
{
	#ifdef BOOST_MSVC
		return load(filename.string());
	#else
		return load(filename.c_str());
	#endif
}

Material Material::load(const std::string& filename)
{
	return load(filename.c_str());
}

Material Material::load(const char* filename)
{
	std::ifstream input(filename, std::ios::binary);
	return load(input);
}

void Material::safeObtainTexture(
	const boost::filesystem::path& filename, 
	IterType& iter)
{
	sTextureWriteLock.obtain();
	iter = std::find_if(sTextures.begin(), sTextures.end(), 
		[&filename](const Material::ItemType& tup)
	{
		if (std::get<0>(tup) == filename) return true;
		else return false;
	});
	if (iter == sTextures.end())
	{
		// texture filename is not present in our global container,
		// so we create a new texture2d initialized with a
		// reference count of 1
		try { sTextures.emplace_back(filename, 1, filename); }
		catch (const exception&)
		{
			sTextureWriteLock.release();
			throw;
		}
		iter = std::prev(sTextures.end());
	}
	else
	{
		++(std::get<1>(*iter)); // increase reference count by 1
	}
	sTextureWriteLock.release();
}

void Material::unsafeObtainTexture(
	const boost::filesystem::path& filename,
	IterType& iter)
{
	iter = std::find_if(sTextures.begin(), sTextures.end(),
		[&filename](const Material::ItemType& tup)
	{
		if (std::get<0>(tup) == filename) return true;
		else return false;
	});
	if (iter == sTextures.end())
	{
		// texture filename is not present in our global container,
		// so we create a new texture2d initialized with a
		// reference count of 1
		try { sTextures.emplace_back(filename, 1, filename); }
		catch (const exception&)
		{
			sTextureWriteLock.release();
			throw;
		}
		iter = std::prev(sTextures.end());
	}
	else
	{
		++(std::get<1>(*iter)); // increase reference count by 1
	}
}

void Material::safeSetNullTexture(IterType& iter)
{
	sTextureWriteLock.obtain();
	iter = sTextures.end();
	sTextureWriteLock.release();
}

void Material::safeReleaseTexture(IterType& iter)
{
	sTextureWriteLock.obtain();
	if (iter != sTextures.end())
	{
		if (--(std::get<1>(*iter)) == 0) // decreases reference count
		{
			sTextures.erase(iter); // erases entry from the global container
		}
	}
	sTextureWriteLock.release();
}

void Material::unsafeReleaseTexture(IterType& iter)
{
	if (iter != sTextures.end())
	{
		auto& refcount = std::get<1>(*iter);
		--refcount;
		if (refcount == 0)
		{
			sTextures.erase(iter); // erases entry from the global container
			iter = sTextures.end();
		}
	}
}

Material::Material()
{
	sTextureWriteLock.obtain();
	mDiffuseTexture = sTextures.end();
	mSpecularTexture = sTextures.end();
	mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
}

Material::Material(
	const vec4f& diffuseColor)
: diffuseColor(diffuseColor)
{
	sTextureWriteLock.obtain();
	mDiffuseTexture = sTextures.end();
	mSpecularTexture = sTextures.end();
	mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	sTextureWriteLock.obtain();
	mDiffuseTexture = sTextures.end();
	mSpecularTexture = sTextures.end();
	mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor,
	const boost::filesystem::path& diffuse_texture)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	sTextureWriteLock.obtain();
	unsafeObtainTexture(diffuse_texture, mDiffuseTexture);
	mSpecularTexture = sTextures.end();
	mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
}

Material::Material(
	const vec4f& diffuseColor, 
	const vec4f& specularColor,
	const boost::filesystem::path& diffuse_texture,
	const boost::filesystem::path& specular_texture)
: diffuseColor(diffuseColor)
, specularColor(specularColor)
{
	sTextureWriteLock.obtain();
	unsafeObtainTexture(diffuse_texture, mDiffuseTexture);
	unsafeObtainTexture(specular_texture, mSpecularTexture);
	mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
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
	sTextureWriteLock.obtain();
	unsafeObtainTexture(diffuse_texture, mDiffuseTexture);
	unsafeObtainTexture(specular_texture, mSpecularTexture);
	unsafeObtainTexture(normal_texture, mNormalTexture);
	sTextureWriteLock.release();
}

Material::Material(const FbxSurfaceMaterial* pFbxMaterial)
{
	const auto lGlobalName = boost::filesystem::path(pFbxMaterial->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
	setName(lGlobalName, pFbxMaterial->GetName());

	std::cout << "\tFound Material: " << pFbxMaterial->GetName() << '\n';

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

	std::cout << "\tDiffuse color: " << diffuseColor << '\n';
	std::cout << "\tSpecular color: " << specularColor << '\n';

	sTextureWriteLock.obtain();

	boost::filesystem::path lFilename;
	if (lNumDiffuseTextures)
	{
		lFilename = getTextureFilename(pFbxMaterial, FbxSurfaceMaterial::sDiffuse, 0);
		try
		{
			unsafeObtainTexture(lFilename, mDiffuseTexture);
		}
		catch (const std::exception& e)
		{
			std::cerr << "\tFailed to fetch texture " << lFilename << '\n';
			lFilename = boost::filesystem::path(pFbxMaterial->GetScene()->GetSceneInfo()->Url.Get().Buffer()).parent_path() / lFilename;
			std::cerr << "\tTrying " << lFilename << '\n';
			unsafeObtainTexture(lFilename, mDiffuseTexture);
		}
		std::cout << "\tDiffuse texture: " << lFilename << '\n';
	}
	else
	{
		std::cout << "\tNo diffuse texture present.\n";
		mDiffuseTexture = sTextures.end();
	}
	if (lNumSpecularTextures)
	{
		lFilename = getTextureFilename(pFbxMaterial, FbxSurfaceMaterial::sSpecular, 0);
		unsafeObtainTexture(lFilename, mSpecularTexture);
		std::cout << "\tSpecular texture: " << lFilename << '\n';
	}
	else
	{
		std::cout << "\tNo specular texture present.\n";
		mSpecularTexture = sTextures.end();
	}
	if (lNumNormalTextures)
	{
		lFilename = getTextureFilename(pFbxMaterial, FbxSurfaceMaterial::sNormalMap, 0);
		unsafeObtainTexture(lFilename, mNormalTexture);
		std::cout << "\tNormal texture: " << lFilename << '\n';
	}
	else
	{
		std::cout << "\tNo normal map present.\n";
		mNormalTexture = sTextures.end();
	}
	sTextureWriteLock.release();
}

Material::~Material() noexcept
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	unsafeReleaseTexture(mSpecularTexture);
	unsafeReleaseTexture(mNormalTexture);
	sTextureWriteLock.release();
	// for (auto* e : m_ents) e->m_Material_component = nullptr;
}

Material::Material(const Material& other)
// : component(other)
: diffuseColor(other.diffuseColor)
, specularColor(other.specularColor)
{
	std::cout << "Material::Material(const Material&): num textures: " << sTextures.size() << '\n';
	sTextureWriteLock.obtain();
	mDiffuseTexture = other.mDiffuseTexture;
	mSpecularTexture = other.mSpecularTexture;
	mNormalTexture = other.mNormalTexture;
	if (mDiffuseTexture != sTextures.end()) ++(std::get<1>(*mDiffuseTexture));
	if (mSpecularTexture != sTextures.end()) ++(std::get<1>(*mSpecularTexture));
	if (mNormalTexture != sTextures.end()) ++(std::get<1>(*mNormalTexture));
	sTextureWriteLock.release();
	std::cout << "Material::Material(const Material&): num textures: " << sTextures.size() << '\n';
}

Material::Material(Material&& other) noexcept
// : component(std::move(other))
: diffuseColor(std::move(other.diffuseColor))
, specularColor(std::move(other.specularColor))
{
	std::cout << "Material::Material(Material&&): num textures: " << sTextures.size() << '\n';
	sTextureWriteLock.obtain();
	mDiffuseTexture = other.mDiffuseTexture;
	mSpecularTexture = other.mSpecularTexture;
	mNormalTexture = other.mNormalTexture;
	other.mDiffuseTexture = sTextures.end();
	other.mSpecularTexture = sTextures.end();
	other.mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
	std::cout << "Material::Material(Material&&): num textures: " << sTextures.size() << '\n';
}

Material& Material::operator = (const Material& other)
{
	std::cout << "Material::operator=(const Material&): num textures: " << sTextures.size() << '\n';
	// component::operator=(other);
	diffuseColor = other.diffuseColor;
	specularColor = other.specularColor;
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	unsafeReleaseTexture(mSpecularTexture);
	unsafeReleaseTexture(mNormalTexture);
	mDiffuseTexture = other.mDiffuseTexture;
	mSpecularTexture = other.mSpecularTexture;
	mNormalTexture = other.mNormalTexture;
	if (mDiffuseTexture != sTextures.end()) ++(std::get<1>(*mDiffuseTexture));
	if (mSpecularTexture != sTextures.end()) ++(std::get<1>(*mSpecularTexture));
	if (mNormalTexture != sTextures.end()) ++(std::get<1>(*mNormalTexture));
	sTextureWriteLock.release();
	std::cout << "Material::operator=(const Material&): num textures: " << sTextures.size() << '\n';
	return *this;
}

Material& Material::operator = (Material&& other) noexcept
{
	std::cout << "Material::operator=(Material&&): num textures: " << sTextures.size() << '\n';
	// component::operator=(std::move(other));
	diffuseColor = std::move(other.diffuseColor);
	specularColor = std::move(other.specularColor);
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	unsafeReleaseTexture(mSpecularTexture);
	unsafeReleaseTexture(mNormalTexture);
	mDiffuseTexture = other.mDiffuseTexture;
	mSpecularTexture = other.mSpecularTexture;
	mNormalTexture = other.mNormalTexture;
	other.mDiffuseTexture = sTextures.end();
	other.mSpecularTexture = sTextures.end();
	other.mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
	std::cout << "Material::operator=(Material&&): num textures: " << sTextures.size() << '\n';
	return *this;
}

// void Material::attach(entity& e)
// {
// 	if (e.m_Material_component == this) return;
// 	else if (e.m_Material_component) e.m_Material_component->detach(e);
// 	e.m_Material_component = this;
// 	m_ents.push_back(&e);
// }

// void Material::detach(entity& e)
// {
// 	for (auto i = begin(); i != end(); ++i)
// 	{
// 		if (*i == &e)
// 		{
// 			e.m_Material_component = nullptr;
// 			m_ents.erase(i);
// 			return;
// 		}
// 	}
// }

void Material::bind(const bool instanced) const noexcept
{
	sTextureWriteLock.obtain();
	if (instanced)
	{
		if (mDiffuseTexture != sTextures.end())
		{
			if (mSpecularTexture != sTextures.end())
			{
				if (mNormalTexture != sTextures.end())
				{
					// diffuse, specular, normal textures
					const auto& s = renderer::get_gpi_dsn_shader();
					s.activate();
					s.set_diffuse_color(diffuseColor);
					s.set_specular_color(specularColor);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
					std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
				}
				else
				{
					// diffuse and specular textures
					const auto& s = renderer::get_gpi_ds_shader();
					s.activate();
					s.set_diffuse_color(diffuseColor);
					s.set_specular_color(specularColor);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
				}
			}
			else if (mNormalTexture != sTextures.end())
			{
				// diffuse and normal textures
				const auto& s = renderer::get_gpi_dn_shader();
				s.activate();
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// diffuse texture
				const auto& s = renderer::get_gpi_d_shader();
				s.activate();
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
			}
		}
		else if (mSpecularTexture != sTextures.end())
		{
			if (mNormalTexture != sTextures.end())
			{
				// specular and normal textures
				const auto& s = renderer::get_gpi_sn_shader();
				s.activate();
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// specular texture
				const auto& s = renderer::get_gpi_s_shader();
				s.activate();
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
			}
		}
		else if (mNormalTexture != sTextures.end())
		{
			// normal texture
			const auto& s = renderer::get_gpi_n_shader();
			s.activate();
			s.set_diffuse_color(diffuseColor);
			s.set_specular_color(specularColor);
			s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
			std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
		}
		else
		{
			// no textures at all
			const auto& s = renderer::get_gpi_shader();
			s.activate();
			s.set_diffuse_color(diffuseColor);
			s.set_specular_color(specularColor);
		}
	}
	else // non-instanced rendering
	{
		if (mDiffuseTexture != sTextures.end())
		{
			if (mSpecularTexture != sTextures.end())
			{
				if (mNormalTexture != sTextures.end())
				{
					// diffuse, specular, normal textures
					const auto& s = renderer::get_gp_dsn_shader();
					s.activate();
					s.set_matrix_PVM(renderer::matrix_PVM());
					s.set_matrix_VM(renderer::matrix_VM());
					s.set_matrix_N(renderer::matrix_N());
					s.set_diffuse_color(diffuseColor);
					s.set_specular_color(specularColor);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
					std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
				}
				else
				{
					// diffuse and specular textures
					const auto& s = renderer::get_gp_ds_shader();
					s.activate();
					s.set_matrix_PVM(renderer::matrix_PVM());
					s.set_matrix_VM(renderer::matrix_VM());
					s.set_matrix_N(renderer::matrix_N());
					s.set_diffuse_color(diffuseColor);
					s.set_specular_color(specularColor);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
				}
			}
			else if (mNormalTexture != sTextures.end())
			{
				// diffuse and normal textures
				const auto& s = renderer::get_gp_dn_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// diffuse texture
				const auto& s = renderer::get_gp_d_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*mDiffuseTexture).bind(renderer::GBUFFER_TEX_DIFFUSE);
			}
		}
		else if (mSpecularTexture != sTextures.end())
		{
			if (mNormalTexture != sTextures.end())
			{
				// specular and normal textures
				const auto& s = renderer::get_gp_sn_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// specular texture
				const auto& s = renderer::get_gp_s_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuseColor);
				s.set_specular_color(specularColor);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*mSpecularTexture).bind(renderer::GBUFFER_TEX_SPECULAR);
			}
		}
		else if (mNormalTexture != sTextures.end())
		{
			// normal texture
			const auto& s = renderer::get_gp_n_shader();
			s.activate();
			s.set_matrix_PVM(renderer::matrix_PVM());
			s.set_matrix_VM(renderer::matrix_VM());
			s.set_matrix_N(renderer::matrix_N());
			s.set_diffuse_color(diffuseColor);
			s.set_specular_color(specularColor);
			s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
			std::get<2>(*mNormalTexture).bind(renderer::GBUFFER_TEX_NORMAL);
		}
		else
		{
			// no textures at all
			const auto& s = renderer::get_gp_shader();
			s.activate();
			s.set_matrix_PVM(renderer::matrix_PVM());
			s.set_matrix_VM(renderer::matrix_VM());
			s.set_matrix_N(renderer::matrix_N());
			s.set_diffuse_color(diffuseColor);
			s.set_specular_color(specularColor);
		}
	}
	sTextureWriteLock.release();
}

bool Material::hasDiffuseTexture() const noexcept
{
	sTextureWriteLock.obtain();
	bool result = (mDiffuseTexture != sTextures.end());
	sTextureWriteLock.release();
	return result;
}

bool Material::hasSpecularTexture() const noexcept
{
	sTextureWriteLock.obtain();
	bool result = (mSpecularTexture != sTextures.end());
	sTextureWriteLock.release();
	return result;
}

bool Material::hasNormalTexture() const noexcept
{
	sTextureWriteLock.obtain();
	bool result = (mNormalTexture != sTextures.end());
	sTextureWriteLock.release();
	return result;
}

const opengl::texture2d& Material::diffuseTexture() const 
noexcept
{
	return std::get<2>(*mDiffuseTexture);
}

const opengl::texture2d& Material::specularTexture() const 
noexcept
{
	return std::get<2>(*mSpecularTexture);
}

const opengl::texture2d& Material::normalTexture() const 
noexcept
{
	return std::get<2>(*mNormalTexture);
}

void Material::setDiffuseTexture(const boost::filesystem::path& filename)
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	unsafeObtainTexture(filename, mDiffuseTexture);
	sTextureWriteLock.release();
}

void Material::setSpecularTexture(const boost::filesystem::path& filename)
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	unsafeObtainTexture(filename, mSpecularTexture);
	sTextureWriteLock.release();
}

void Material::setNormalTexture(const boost::filesystem::path& filename)
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	unsafeObtainTexture(filename, mNormalTexture);
	sTextureWriteLock.release();
}

void Material::clearDiffuseTexture()
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mDiffuseTexture);
	mDiffuseTexture = sTextures.end();
	sTextureWriteLock.release();
}

void Material::clearSpecularTexture()
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mSpecularTexture);
	mSpecularTexture = sTextures.end();
	sTextureWriteLock.release();
}

void Material::clearNormalTexture()
{
	sTextureWriteLock.obtain();
	unsafeReleaseTexture(mNormalTexture);
	mNormalTexture = sTextures.end();
	sTextureWriteLock.release();
}

std::ostream& operator << (std::ostream& os, const Material& mat)
{
	// First print out the diffuse color and specular color.
	os << "{ diffuseColor: " << mat.diffuseColor << ", specularColor: "
		<< mat.specularColor;

	// Obtain a lock on the global texture container.
	Material::sTextureWriteLock.obtain();

	if (mat.mDiffuseTexture != Material::sTextures.end())
	{
		os << ", diffuse_texture: " << std::get<2>(*mat.mDiffuseTexture);
	}
	if (mat.mSpecularTexture != Material::sTextures.end())
	{
		os << ", specular_texture: " << std::get<2>(*mat.mSpecularTexture);
	}
	if (mat.mNormalTexture != Material::sTextures.end())
	{
		os << ", normal_texture: " << std::get<2>(*mat.mNormalTexture);
	}
	os << " }";

	// Release the lock on the global texture container.
	Material::sTextureWriteLock.release();

	return os;
}

} // namespace gintonic
