#include "materials.hpp"
#include "shaders.hpp"
#include "renderer.hpp"
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

namespace gintonic {

/*****************************************************************************
 * gintonic::material (base class for inheritance)                           *
 ****************************************************************************/

material::datastructure_type material::s_textures = material::datastructure_type();
write_lock material::s_textures_lock;

void material::save(std::ostream& output) const
{
	eos::portable_oarchive archive(output);
	const material* tmp = this;
	archive & boost::serialization::make_nvp("base_ptr", tmp);
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

material* material::load(std::istream& input)
{
	material* ptr = nullptr;
	eos::portable_iarchive archive(input);
	archive & boost::serialization::make_nvp("base_ptr", ptr);
	return ptr;
}

material* material::load(const boost::filesystem::path& filename)
{
	#ifdef BOOST_MSVC
		return load(filename.string());
	#else
		return load(filename.c_str());
	#endif
}

material* material::load(const std::string& filename)
{
	return load(filename.c_str());
}

material* material::load(const char* filename)
{
	std::ifstream input(filename, std::ios::binary);
	return load(input);
}

void material::safe_obtain_texture(const boost::filesystem::path& filename, iter_type& iter)
{
	s_textures_lock.obtain();
	iter = std::find_if(s_textures.begin(), s_textures.end(), [&filename](const material::item_type& tup){
		if (std::get<0>(tup) == filename) return true;
		else return false;
	});
	if (iter == s_textures.end())
	{
		// texture filename is not present in our global container, so we create a new texture2d
		// initialized with a reference count of 1
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

material::~material() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void material::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto s = renderer::get_geometry_null_shader();
	s->activate();
	s->set_matrix_PVM(renderer::matrix_PVM());
	s->set_matrix_VM(renderer::matrix_VM());
	s->set_matrix_N(renderer::matrix_N());
}

/*****************************************************************************
 * gintonic::material_c (color)                                              *
 ****************************************************************************/

material_c::material_c(const vec4f& diffuse_color)
: diffuse_color(diffuse_color)
{
	/* Empty on purpose. */
}

material_c::~material_c() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void material_c::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto s = renderer::get_gp_c_shader();
	s->activate();
	s->set_matrix_PVM(renderer::matrix_PVM());
	s->set_matrix_VM(renderer::matrix_VM());
	s->set_matrix_N(renderer::matrix_N());
	s->set_color(diffuse_color);
}

diffuse_material::diffuse_material(
	boost::filesystem::path diffuse_filename,
	const float diffuse_factor)
: diffuse_factor(diffuse_factor)
{
	safe_obtain_texture(diffuse_filename, m_diffuse);
}

diffuse_material::~diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW
{
	safe_release_texture(m_diffuse);
}

const opengl::texture2d& diffuse_material::diffuse() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_diffuse);
}

void diffuse_material::set_diffuse(const boost::filesystem::path& filename)
{
	safe_release_texture(m_diffuse);
	safe_obtain_texture(filename, m_diffuse);
}

void diffuse_material::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto s = renderer::get_geometry_pass_shader();
	s->activate();
	s->set_matrix_PVM(renderer::matrix_PVM());
	s->set_matrix_VM(renderer::matrix_VM());
	s->set_matrix_N(renderer::matrix_N());
	s->set_diffuse(0);
	s->set_diffuse_factor(diffuse_factor);
	diffuse().bind(0);
}

/*****************************************************************************
 * gintonic::material_cd (color, diffuse)                                    *
 ****************************************************************************/

material_cd::material_cd()
{
	safe_set_null_texture(m_diffuse);
}

const opengl::texture2d& material_cd::diffuse() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_diffuse);
}

void material_cd::set_diffuse(const boost::filesystem::path& filename)
{
	safe_release_texture(m_diffuse);
	safe_obtain_texture(filename, m_diffuse);
}

material_cd::material_cd(
	const vec4f& color,
	boost::filesystem::path diffuse_filename)
: material_c(color)
{
	safe_obtain_texture(diffuse_filename, m_diffuse);
}

void material_cd::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_cd_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_matrix_VM(renderer::matrix_VM());
	s.set_matrix_N(renderer::matrix_N());
	s.set_diffuse(0);
	s.set_color(diffuse_color);
	diffuse().bind(0);
}

material_cd::~material_cd() BOOST_NOEXCEPT_OR_NOTHROW
{
	safe_release_texture(m_diffuse);
}

/*****************************************************************************
 * gintonic::material_cds (color, diffuse, specular)                         *
 ****************************************************************************/

material_cds::material_cds()
{
	safe_set_null_texture(m_specular);
}

const opengl::texture2d& material_cds::specular() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_specular);
}

void material_cds::set_specular(const boost::filesystem::path& filename)
{
	safe_release_texture(m_specular);
	safe_obtain_texture(filename, m_specular);
}

material_cds::material_cds(
	const vec4f& color,
	boost::filesystem::path diffuse_filename,
	boost::filesystem::path specular_filename)
: material_cd(color, std::move(diffuse_filename))
{
	safe_obtain_texture(specular_filename, m_specular);
}

void material_cds::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_cds_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_matrix_VM(renderer::matrix_VM());
	s.set_matrix_N(renderer::matrix_N());
	s.set_color(diffuse_color);
	s.set_diffuse(0);
	s.set_specular(1);
	diffuse().bind(0);
	specular().bind(1);
}

material_cds::~material_cds() BOOST_NOEXCEPT_OR_NOTHROW
{
	safe_release_texture(m_specular);
}

/*****************************************************************************
 * gintonic::material_cdn (color, diffuse, normal map)                       *
 ****************************************************************************/

const opengl::texture2d& material_cdn::normal() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_normal);
}

material_cdn::material_cdn()
{
	safe_set_null_texture(m_normal);
}

void material_cdn::set_normal(const boost::filesystem::path& filename)
{
	safe_release_texture(m_normal);
	safe_obtain_texture(filename, m_normal);
}

material_cdn::material_cdn(
	const vec4f& color,
	boost::filesystem::path diffuse_filename,
	boost::filesystem::path normal_filename)
: material_cd(color, std::move(diffuse_filename))
{
	safe_obtain_texture(normal_filename, m_normal);
}

void material_cdn::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_cdn_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_color(diffuse_color);
	s.set_diffuse(0);
	s.set_normal(1);
	diffuse().bind(0);
	normal().bind(1);
}

material_cdn::~material_cdn() BOOST_NOEXCEPT_OR_NOTHROW
{
	safe_release_texture(m_normal);
}

const opengl::texture2d& specular_diffuse_material::specular() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_specular);
}

void specular_diffuse_material::set_specular(const boost::filesystem::path& filename)
{
	safe_release_texture(m_specular);
	safe_obtain_texture(filename, m_specular);
}

specular_diffuse_material::specular_diffuse_material(
	boost::filesystem::path diffuse_filename,
	boost::filesystem::path specular_filename,
	const float diffuse_factor,
	const float shininess)
: diffuse_material(std::move(diffuse_filename), diffuse_factor), shininess(shininess)
{
	safe_obtain_texture(specular_filename, m_specular);
}

void specular_diffuse_material::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	// TODO
}

specular_diffuse_material::~specular_diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW
{
	safe_release_texture(m_specular);
}

// lambert_material::lambert_material(
// 	std::shared_ptr<geometry_pass_shader> prog,
// 	std::shared_ptr<opengl::texture2d> diffuse_tex, 
// 	const float diffuse_tex_factor)
// : diffuse_value(diffuse_tex)
// , diffuse_factor_value(diffuse_tex_factor)
// , m_program(prog)
// {
// 	/* Empty on purpose. */
// }

// lambert_material::lambert_material(std::shared_ptr<geometry_pass_shader> prog, FbxSurfaceLambert* fbx_mat)
// : m_program(prog)
// {
// 	auto prop_diffuse = fbx_mat->FindProperty(FbxSurfaceMaterial::sDiffuse);
// 	auto prop_diffuse_factor = fbx_mat->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
// 	std::cerr << "Determining number of file textures...\n";
// 	auto file_texture_count = prop_diffuse.GetSrcObjectCount<FbxFileTexture>();
// 	std::cerr << "Lambert material has " << file_texture_count << " file textures.\n";
// 	if (!file_texture_count) throw std::runtime_error("No diffuse texture.");
// 	auto file_texture = FbxCast<FbxFileTexture>(prop_diffuse.GetSrcObject<FbxFileTexture>(0));
// 	std::cerr << file_texture->GetFileName() << '\n';
// }

// void lambert_material::bind() const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	m_program->activate();
// 	m_program->set_matrix_PVM(renderer::matrix_PVM());
// 	m_program->set_matrix_VM(renderer::matrix_VM());
// 	m_program->set_matrix_N(renderer::matrix_N());
// 	m_program->set_diffuse(0);
// 	m_program->set_diffuse_factor(diffuse_factor_value);
// 	diffuse_value->bind(0);
// }

// phong_material::phong_material(
// 	std::shared_ptr<geometry_pass_shader> prog,
// 	std::shared_ptr<opengl::texture2d> diffuse_tex,
// 	std::shared_ptr<opengl::texture2d> specular_tex,
// 	const float diffuse_factor_value,
// 	const float shininess)
// : diffuse_value(diffuse_tex)
// , specular_value(specular_tex)
// , diffuse_factor_value(diffuse_factor_value)
// , shininess(shininess)
// , m_program(prog)
// {
// 	/* Empty on purpose. */
// }

// phong_material::phong_material(
// 	std::shared_ptr<geometry_pass_shader> prog, 
// 	FbxSurfacePhong* fbx_mat)
// : m_program(prog)
// {
// 	auto prop_diffuse = fbx_mat->FindProperty(FbxSurfaceMaterial::sDiffuse);
// 	auto prop_diffuse_factor = fbx_mat->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
// 	auto prop_specular = fbx_mat->FindProperty(FbxSurfaceMaterial::sSpecular);
// 	auto prop_normal = fbx_mat->FindProperty(FbxSurfaceMaterial::sNormalMap);
// 	auto diffuse_texture_count = prop_diffuse.GetSrcObjectCount<FbxFileTexture>();
// 	auto specular_texture_count = prop_specular.GetSrcObjectCount<FbxFileTexture>();
// 	auto normal_texture_count = prop_normal.GetSrcObjectCount<FbxFileTexture>();
// 	std::cerr << "Phong material has " << diffuse_texture_count << " diffuse textures.\n";
// 	std::cerr << "Phong material has " << specular_texture_count << " specular textures.\n";
// 	std::cerr << "Phong material has " << normal_texture_count << " normal textures.\n";
// 	if (!diffuse_texture_count) throw std::runtime_error("No diffuse texture.");
// 	if (!specular_texture_count) throw std::runtime_error("No specular texture.");
// 	if (!normal_texture_count)
// 	{
// 		auto prop_bump = fbx_mat->FindProperty(FbxSurfaceMaterial::sBump);
// 		auto prop_bump_factor = fbx_mat->FindProperty(FbxSurfaceMaterial::sBumpFactor);
// 		auto bump_texture_count = prop_bump.GetSrcObjectCount<FbxFileTexture>();
// 		if (!bump_texture_count) throw std::runtime_error("Neither a normal map nor a bump map.");
// 		auto file_diffuse = FbxCast<FbxFileTexture>(prop_diffuse.GetSrcObject<FbxFileTexture>(0));
// 		auto file_specular = FbxCast<FbxFileTexture>(prop_specular.GetSrcObject<FbxFileTexture>(0));
// 		auto file_bump = FbxCast<FbxFileTexture>(prop_bump.GetSrcObject<FbxFileTexture>(0));

// 		std::cerr << file_diffuse->GetFileName() << '\n';
// 		std::cerr << file_specular->GetFileName() << '\n';
// 		std::cerr << file_bump->GetFileName() << '\n';
// 		std::cerr << "Bump factor: " << prop_bump_factor.Get<FbxDouble>() << '\n';
// 	}
// 	else
// 	{
// 		auto file_diffuse = FbxCast<FbxFileTexture>(prop_diffuse.GetSrcObject<FbxFileTexture>(0));
// 		auto file_specular = FbxCast<FbxFileTexture>(prop_specular.GetSrcObject<FbxFileTexture>(0));
// 		auto file_normal = FbxCast<FbxFileTexture>(prop_normal.GetSrcObject<FbxFileTexture>(0));

// 		std::cerr << file_diffuse->GetFileName() << '\n';
// 		std::cerr << file_specular->GetFileName() << '\n';
// 		std::cerr << file_normal->GetFileName() << '\n';
// 	}
// 	throw std::runtime_error("The rest is not yet implemented. :-(");
// }

// void phong_material::bind() const BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	m_program->activate();
// 	m_program->set_matrix_PVM(renderer::matrix_PVM());
// 	m_program->set_matrix_VM(renderer::matrix_VM());
// 	m_program->set_matrix_N(renderer::matrix_N());
// 	m_program->set_diffuse(0);
// 	m_program->set_diffuse_factor(diffuse_factor_value);
// 	// m_program->set_specular(1);
// 	// m_program->set_shininess(shininess);
// 	diffuse_value->bind(0);
// 	// specular_value->bind(1);
// }

} // namespace gintonic

// BOOST_CLASS_EXPORT(gintonic::material)
BOOST_CLASS_EXPORT(gintonic::material_c)
BOOST_CLASS_EXPORT(gintonic::material_cd)
BOOST_CLASS_EXPORT(gintonic::material_cds)
BOOST_CLASS_EXPORT(gintonic::material_cdn)