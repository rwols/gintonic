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
#include <fbxsdk/scene/shading/fbxsurfacematerial.h>
#include <fbxsdk/scene/shading/fbxsurfacelambert.h>
#include <fbxsdk/scene/shading/fbxsurfacephong.h>
#include <fbxsdk/scene/shading/fbxtexture.h>
#include <fbxsdk/scene/shading/fbxfiletexture.h>

using FBX::FbxSurfaceMaterial;
using FBX::FbxSurfaceLambert;
using FBX::FbxSurfacePhong;
using FBX::FbxTexture;
using FBX::FbxFileTexture;
using FBX::FbxCast;
using FBX::FbxDouble;
using FBX::FbxDouble3;

namespace { // anonymous namespace

std::size_t texture_count(
	const FbxSurfaceMaterial* mat, 
	const char* property)
{
	const auto prop = mat->FindProperty(property);
	return prop.GetSrcObjectCount<FbxFileTexture>();
}

boost::filesystem::path get_texture_filename(
	const FbxSurfaceMaterial* mat, 
	const char* property, 
	const std::size_t index)
{
	const auto prop = mat->FindProperty(property);
	const auto file_prop = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(index));
	return file_prop->GetFileName();
}

float get_texture_factor(
	const FbxSurfaceMaterial* mat,
	const char* property)
{
	const auto prop = mat->FindProperty(property);
	return static_cast<float>(prop.Get<FbxDouble>());
}

gintonic::vec3f get_material_color(
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
 * gintonic::material (base class for inheritance)                           *
 ****************************************************************************/

material::datastructure_type material::s_textures ;
	// = material::datastructure_type();

write_lock material::s_textures_lock;

void material::save(std::ostream& output) const
{
	eos::portable_oarchive archive(output);
	archive & boost::serialization::make_nvp("material", *this);
}

void material::save(const boost::filesystem::path& filename) const
{
	#ifdef BOOST_MSVC
		save(filename.string());
	#else
		save(filename.c_str());
	#endif
}

void material::save(const std::string& filename) const
{
	save(filename.c_str());
}

void material::save(const char* filename) const
{
	std::ofstream output(filename, std::ios::binary);
	save(output);
}

material material::load(std::istream& input)
{
	material m;
	eos::portable_iarchive archive(input);
	archive & boost::serialization::make_nvp("material", m);
	return m;
}

material material::load(const boost::filesystem::path& filename)
{
	#ifdef BOOST_MSVC
		return load(filename.string());
	#else
		return load(filename.c_str());
	#endif
}

material material::load(const std::string& filename)
{
	return load(filename.c_str());
}

material material::load(const char* filename)
{
	std::ifstream input(filename, std::ios::binary);
	return load(input);
}

void material::safe_obtain_texture(
	const boost::filesystem::path& filename, 
	iter_type& iter)
{
	s_textures_lock.obtain();
	iter = std::find_if(s_textures.begin(), s_textures.end(), 
		[&filename](const material::item_type& tup)
	{
		if (std::get<0>(tup) == filename) return true;
		else return false;
	});
	if (iter == s_textures.end())
	{
		// texture filename is not present in our global container,
		// so we create a new texture2d initialized with a
		// reference count of 1
		try { s_textures.emplace_back(filename, 1, filename); }
		catch (const exception&)
		{
			s_textures_lock.release();
			throw;
		}
		iter = std::prev(s_textures.end());
	}
	else
	{
		++(std::get<1>(*iter)); // increase reference count by 1
	}
	s_textures_lock.release();
}

void material::unsafe_obtain_texture(
	const boost::filesystem::path& filename,
	iter_type& iter)
{
	iter = std::find_if(s_textures.begin(), s_textures.end(),
		[&filename](const material::item_type& tup)
	{
		if (std::get<0>(tup) == filename) return true;
		else return false;
	});
	if (iter == s_textures.end())
	{
		// texture filename is not present in our global container,
		// so we create a new texture2d initialized with a
		// reference count of 1
		try { s_textures.emplace_back(filename, 1, filename); }
		catch (const exception&)
		{
			s_textures_lock.release();
			throw;
		}
		iter = std::prev(s_textures.end());
	}
	else
	{
		++(std::get<1>(*iter)); // increase reference count by 1
	}
}

void material::safe_set_null_texture(iter_type& iter)
{
	s_textures_lock.obtain();
	iter = s_textures.end();
	s_textures_lock.release();
}

void material::safe_release_texture(iter_type& iter)
{
	s_textures_lock.obtain();
	if (iter != s_textures.end())
	{
		if (--(std::get<1>(*iter)) == 0) // decreases reference count
		{
			s_textures.erase(iter); // erases entry from the global container
		}
	}
	s_textures_lock.release();
}

void material::unsafe_release_texture(iter_type& iter)
{
	if (iter != s_textures.end())
	{
		auto& refcount = std::get<1>(*iter);
		--refcount;
		if (refcount == 0)
		{
			s_textures.erase(iter); // erases entry from the global container
			iter = s_textures.end();
		}
	}
}

material::material()
{
	std::cout << "material::material num textures: " << s_textures.size() << '\n';
	s_textures_lock.obtain();
	m_diffuse_tex = s_textures.end();
	m_specular_tex = s_textures.end();
	m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

material::material(
	const vec4f& diffuse_color)
: diffuse_color(diffuse_color)
{
	s_textures_lock.obtain();
	m_diffuse_tex = s_textures.end();
	m_specular_tex = s_textures.end();
	m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

material::material(
	const vec4f& diffuse_color, 
	const vec4f& specular_color)
: diffuse_color(diffuse_color)
, specular_color(specular_color)
{
	s_textures_lock.obtain();
	m_diffuse_tex = s_textures.end();
	m_specular_tex = s_textures.end();
	m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

material::material(
	const vec4f& diffuse_color, 
	const vec4f& specular_color,
	const boost::filesystem::path& diffuse_texture)
: diffuse_color(diffuse_color)
, specular_color(specular_color)
{
	s_textures_lock.obtain();
	unsafe_obtain_texture(diffuse_texture, m_diffuse_tex);
	m_specular_tex = s_textures.end();
	m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

material::material(
	const vec4f& diffuse_color, 
	const vec4f& specular_color,
	const boost::filesystem::path& diffuse_texture,
	const boost::filesystem::path& specular_texture)
: diffuse_color(diffuse_color)
, specular_color(specular_color)
{
	s_textures_lock.obtain();
	unsafe_obtain_texture(diffuse_texture, m_diffuse_tex);
	unsafe_obtain_texture(specular_texture, m_specular_tex);
	m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

material::material(
	const vec4f& diffuse_color, 
	const vec4f& specular_color,
	const boost::filesystem::path& diffuse_texture,
	const boost::filesystem::path& specular_texture,
	const boost::filesystem::path& normal_texture)
: diffuse_color(diffuse_color)
, specular_color(specular_color)
{
	s_textures_lock.obtain();
	unsafe_obtain_texture(diffuse_texture, m_diffuse_tex);
	unsafe_obtain_texture(specular_texture, m_specular_tex);
	unsafe_obtain_texture(normal_texture, m_normal_tex);
	s_textures_lock.release();
}

material::material(const FbxSurfaceMaterial* fbxmat)
{
	std::cout << "\tFound material: " << fbxmat->GetName() << '\n';

	const auto diffuse_count   = texture_count(fbxmat, FbxSurfaceMaterial::sDiffuse);
	const auto specular_count  = texture_count(fbxmat, FbxSurfaceMaterial::sSpecular);
	const auto normal_count    = texture_count(fbxmat, FbxSurfaceMaterial::sNormalMap);

	const auto diffuse_factor  = get_texture_factor(fbxmat, FbxSurfaceMaterial::sDiffuseFactor);
	const auto specular_factor = get_texture_factor(fbxmat, FbxSurfaceMaterial::sSpecularFactor);
	const auto shininess       = get_texture_factor(fbxmat, FbxSurfaceMaterial::sShininess);

	diffuse_color = vec4f(get_material_color(fbxmat, FbxSurfaceMaterial::sDiffuse), diffuse_factor);
	specular_color = vec4f(get_material_color(fbxmat, FbxSurfaceMaterial::sSpecular), 1.0f);

	specular_color *= specular_factor;
	specular_color.w = shininess;

	std::cout << "\tDiffuse color: " << diffuse_color << '\n';
	std::cout << "\tSpecular color: " << specular_color << '\n';

	s_textures_lock.obtain();

	boost::filesystem::path tex_filename;
	if (diffuse_count)
	{
		tex_filename = get_texture_filename(fbxmat, FbxSurfaceMaterial::sDiffuse, 0);
		unsafe_obtain_texture(tex_filename, m_diffuse_tex);
		std::cout << "\tDiffuse texture: " << tex_filename << '\n';
	}
	else
	{
		std::cout << "\tNo diffuse texture present.\n";
		m_diffuse_tex = s_textures.end();
	}
	if (specular_count)
	{
		tex_filename = get_texture_filename(fbxmat, FbxSurfaceMaterial::sSpecular, 0);
		unsafe_obtain_texture(tex_filename, m_specular_tex);
		std::cout << "\tSpecular texture: " << tex_filename << '\n';
	}
	else
	{
		std::cout << "\tNo specular texture present.\n";
		m_specular_tex = s_textures.end();
	}
	if (normal_count)
	{
		tex_filename = get_texture_filename(fbxmat, FbxSurfaceMaterial::sNormalMap, 0);
		unsafe_obtain_texture(tex_filename, m_normal_tex);
		std::cout << "\tNormal texture: " << tex_filename << '\n';
	}
	else
	{
		std::cout << "\tNo normal map present.\n";
		m_normal_tex = s_textures.end();
	}
	s_textures_lock.release();
}

material::~material() noexcept
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_release_texture(m_specular_tex);
	unsafe_release_texture(m_normal_tex);
	s_textures_lock.release();
	for (auto* e : m_ents) e->m_material_component = nullptr;
}

material::material(const material& other)
: component(other)
, diffuse_color(other.diffuse_color)
, specular_color(other.specular_color)
{
	std::cout << "material::material(const material&): num textures: " << s_textures.size() << '\n';
	s_textures_lock.obtain();
	m_diffuse_tex = other.m_diffuse_tex;
	m_specular_tex = other.m_specular_tex;
	m_normal_tex = other.m_normal_tex;
	if (m_diffuse_tex != s_textures.end()) ++(std::get<1>(*m_diffuse_tex));
	if (m_specular_tex != s_textures.end()) ++(std::get<1>(*m_specular_tex));
	if (m_normal_tex != s_textures.end()) ++(std::get<1>(*m_normal_tex));
	s_textures_lock.release();
	std::cout << "material::material(const material&): num textures: " << s_textures.size() << '\n';
}

material::material(material&& other) noexcept
: component(std::move(other))
, diffuse_color(std::move(other.diffuse_color))
, specular_color(std::move(other.specular_color))
{
	std::cout << "material::material(material&&): num textures: " << s_textures.size() << '\n';
	s_textures_lock.obtain();
	m_diffuse_tex = other.m_diffuse_tex;
	m_specular_tex = other.m_specular_tex;
	m_normal_tex = other.m_normal_tex;
	other.m_diffuse_tex = s_textures.end();
	other.m_specular_tex = s_textures.end();
	other.m_normal_tex = s_textures.end();
	s_textures_lock.release();
	std::cout << "material::material(material&&): num textures: " << s_textures.size() << '\n';
}

material& material::operator = (const material& other)
{
	std::cout << "material::operator=(const material&): num textures: " << s_textures.size() << '\n';
	component::operator=(other);
	diffuse_color = other.diffuse_color;
	specular_color = other.specular_color;
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_release_texture(m_specular_tex);
	unsafe_release_texture(m_normal_tex);
	m_diffuse_tex = other.m_diffuse_tex;
	m_specular_tex = other.m_specular_tex;
	m_normal_tex = other.m_normal_tex;
	if (m_diffuse_tex != s_textures.end()) ++(std::get<1>(*m_diffuse_tex));
	if (m_specular_tex != s_textures.end()) ++(std::get<1>(*m_specular_tex));
	if (m_normal_tex != s_textures.end()) ++(std::get<1>(*m_normal_tex));
	s_textures_lock.release();
	std::cout << "material::operator=(const material&): num textures: " << s_textures.size() << '\n';
	return *this;
}

material& material::operator = (material&& other) noexcept
{
	std::cout << "material::operator=(material&&): num textures: " << s_textures.size() << '\n';
	component::operator=(std::move(other));
	diffuse_color = std::move(other.diffuse_color);
	specular_color = std::move(other.specular_color);
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_release_texture(m_specular_tex);
	unsafe_release_texture(m_normal_tex);
	m_diffuse_tex = other.m_diffuse_tex;
	m_specular_tex = other.m_specular_tex;
	m_normal_tex = other.m_normal_tex;
	other.m_diffuse_tex = s_textures.end();
	other.m_specular_tex = s_textures.end();
	other.m_normal_tex = s_textures.end();
	s_textures_lock.release();
	std::cout << "material::operator=(material&&): num textures: " << s_textures.size() << '\n';
	return *this;
}

void material::attach(entity& e)
{
	if (e.m_material_component == this) return;
	else if (e.m_material_component) e.m_material_component->detach(e);
	e.m_material_component = this;
	m_ents.push_back(&e);
}

void material::detach(entity& e)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (*i == &e)
		{
			e.m_material_component = nullptr;
			m_ents.erase(i);
			return;
		}
	}
}

void material::bind(const bool instanced) const noexcept
{
	s_textures_lock.obtain();
	if (instanced)
	{
		if (m_diffuse_tex != s_textures.end())
		{
			if (m_specular_tex != s_textures.end())
			{
				if (m_normal_tex != s_textures.end())
				{
					// diffuse, specular, normal textures
					const auto& s = renderer::get_gpi_dsn_shader();
					s.activate();
					s.set_diffuse_color(diffuse_color);
					s.set_specular_color(specular_color);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
					std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
				}
				else
				{
					// diffuse and specular textures
					const auto& s = renderer::get_gpi_ds_shader();
					s.activate();
					s.set_diffuse_color(diffuse_color);
					s.set_specular_color(specular_color);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
				}
			}
			else if (m_normal_tex != s_textures.end())
			{
				// diffuse and normal textures
				const auto& s = renderer::get_gpi_dn_shader();
				s.activate();
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// diffuse texture
				const auto& s = renderer::get_gpi_d_shader();
				s.activate();
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
			}
		}
		else if (m_specular_tex != s_textures.end())
		{
			if (m_normal_tex != s_textures.end())
			{
				// specular and normal textures
				const auto& s = renderer::get_gpi_sn_shader();
				s.activate();
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// specular texture
				const auto& s = renderer::get_gpi_s_shader();
				s.activate();
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
			}
		}
		else if (m_normal_tex != s_textures.end())
		{
			// normal texture
			const auto& s = renderer::get_gpi_n_shader();
			s.activate();
			s.set_diffuse_color(diffuse_color);
			s.set_specular_color(specular_color);
			s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
			std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
		}
		else
		{
			// no textures at all
			const auto& s = renderer::get_gpi_shader();
			s.activate();
			s.set_diffuse_color(diffuse_color);
			s.set_specular_color(specular_color);
		}
	}
	else // non-instanced rendering
	{
		if (m_diffuse_tex != s_textures.end())
		{
			if (m_specular_tex != s_textures.end())
			{
				if (m_normal_tex != s_textures.end())
				{
					// diffuse, specular, normal textures
					const auto& s = renderer::get_gp_dsn_shader();
					s.activate();
					s.set_matrix_PVM(renderer::matrix_PVM());
					s.set_matrix_VM(renderer::matrix_VM());
					s.set_matrix_N(renderer::matrix_N());
					s.set_diffuse_color(diffuse_color);
					s.set_specular_color(specular_color);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
					std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
				}
				else
				{
					// diffuse and specular textures
					const auto& s = renderer::get_gp_ds_shader();
					s.activate();
					s.set_matrix_PVM(renderer::matrix_PVM());
					s.set_matrix_VM(renderer::matrix_VM());
					s.set_matrix_N(renderer::matrix_N());
					s.set_diffuse_color(diffuse_color);
					s.set_specular_color(specular_color);
					s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
					s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
					std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
					std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
				}
			}
			else if (m_normal_tex != s_textures.end())
			{
				// diffuse and normal textures
				const auto& s = renderer::get_gp_dn_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// diffuse texture
				const auto& s = renderer::get_gp_d_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
				std::get<2>(*m_diffuse_tex).bind(renderer::GBUFFER_TEX_DIFFUSE);
			}
		}
		else if (m_specular_tex != s_textures.end())
		{
			if (m_normal_tex != s_textures.end())
			{
				// specular and normal textures
				const auto& s = renderer::get_gp_sn_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
				std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
			}
			else
			{
				// specular texture
				const auto& s = renderer::get_gp_s_shader();
				s.activate();
				s.set_matrix_PVM(renderer::matrix_PVM());
				s.set_matrix_VM(renderer::matrix_VM());
				s.set_matrix_N(renderer::matrix_N());
				s.set_diffuse_color(diffuse_color);
				s.set_specular_color(specular_color);
				s.set_specular_texture(renderer::GBUFFER_TEX_SPECULAR);
				std::get<2>(*m_specular_tex).bind(renderer::GBUFFER_TEX_SPECULAR);
			}
		}
		else if (m_normal_tex != s_textures.end())
		{
			// normal texture
			const auto& s = renderer::get_gp_n_shader();
			s.activate();
			s.set_matrix_PVM(renderer::matrix_PVM());
			s.set_matrix_VM(renderer::matrix_VM());
			s.set_matrix_N(renderer::matrix_N());
			s.set_diffuse_color(diffuse_color);
			s.set_specular_color(specular_color);
			s.set_normal_texture(renderer::GBUFFER_TEX_NORMAL);
			std::get<2>(*m_normal_tex).bind(renderer::GBUFFER_TEX_NORMAL);
		}
		else
		{
			// no textures at all
			const auto& s = renderer::get_gp_shader();
			s.activate();
			s.set_matrix_PVM(renderer::matrix_PVM());
			s.set_matrix_VM(renderer::matrix_VM());
			s.set_matrix_N(renderer::matrix_N());
			s.set_diffuse_color(diffuse_color);
			s.set_specular_color(specular_color);
		}
	}
	s_textures_lock.release();
}

bool material::has_diffuse_texture() const noexcept
{
	s_textures_lock.obtain();
	bool result = (m_diffuse_tex != s_textures.end());
	s_textures_lock.release();
	return result;
}

bool material::has_specular_texture() const noexcept
{
	s_textures_lock.obtain();
	bool result = (m_specular_tex != s_textures.end());
	s_textures_lock.release();
	return result;
}

bool material::has_normal_texture() const noexcept
{
	s_textures_lock.obtain();
	bool result = (m_normal_tex != s_textures.end());
	s_textures_lock.release();
	return result;
}

const opengl::texture2d& material::diffuse_texture() const 
noexcept
{
	return std::get<2>(*m_diffuse_tex);
}

const opengl::texture2d& material::specular_texture() const 
noexcept
{
	return std::get<2>(*m_specular_tex);
}

const opengl::texture2d& material::normal_texture() const 
noexcept
{
	return std::get<2>(*m_normal_tex);
}

void material::set_diffuse_texture(const boost::filesystem::path& filename)
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_obtain_texture(filename, m_diffuse_tex);
	s_textures_lock.release();
}

void material::set_specular_texture(const boost::filesystem::path& filename)
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_obtain_texture(filename, m_specular_tex);
	s_textures_lock.release();
}

void material::set_normal_texture(const boost::filesystem::path& filename)
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_obtain_texture(filename, m_normal_tex);
	s_textures_lock.release();
}

void material::clear_diffuse_texture()
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	m_diffuse_tex = s_textures.end();
	s_textures_lock.release();
}

void material::clear_specular_texture()
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_specular_tex);
	m_specular_tex = s_textures.end();
	s_textures_lock.release();
}

void material::clear_normal_texture()
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_normal_tex);
	m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

std::ostream& operator << (std::ostream& os, const material& mat)
{
	// First print out the diffuse color and specular color.
	os << "{ diffuse_color: " << mat.diffuse_color << ", specular_color: "
		<< mat.specular_color;

	// Obtain a lock on the global texture container.
	material::s_textures_lock.obtain();

	if (mat.m_diffuse_tex != material::s_textures.end())
	{
		os << ", diffuse_texture: " << std::get<2>(*mat.m_diffuse_tex);
	}
	if (mat.m_specular_tex != material::s_textures.end())
	{
		os << ", specular_texture: " << std::get<2>(*mat.m_specular_tex);
	}
	if (mat.m_normal_tex != material::s_textures.end())
	{
		os << ", normal_texture: " << std::get<2>(*mat.m_normal_tex);
	}
	os << " }";

	// Release the lock on the global texture container.
	material::s_textures_lock.release();

	return os;
}

} // namespace gintonic
