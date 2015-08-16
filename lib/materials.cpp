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

material::datastructure_type material::s_textures 
	= material::datastructure_type();

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
		if (--(std::get<1>(*iter)) == 0) // decreases reference count
		{
			s_textures.erase(iter); // erases entry from the global container
		}
	}
}

material::material()
{
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

material::~material() BOOST_NOEXCEPT_OR_NOTHROW
{
	s_textures_lock.obtain();
	unsafe_release_texture(m_diffuse_tex);
	unsafe_release_texture(m_specular_tex);
	unsafe_release_texture(m_normal_tex);
	s_textures_lock.release();
}

material::material(const material& other)
: diffuse_color(other.diffuse_color)
, specular_color(other.specular_color)
{
	s_textures_lock.obtain();
	m_diffuse_tex = other.m_diffuse_tex;
	m_specular_tex = other.m_specular_tex;
	m_normal_tex = other.m_normal_tex;
	if (m_diffuse_tex != s_textures.end()) ++(std::get<1>(*m_diffuse_tex));
	if (m_specular_tex != s_textures.end()) ++(std::get<1>(*m_specular_tex));
	if (m_normal_tex != s_textures.end()) ++(std::get<1>(*m_normal_tex));
	s_textures_lock.release();
}

material::material(material&& other) BOOST_NOEXCEPT_OR_NOTHROW
: diffuse_color(other.diffuse_color)
, specular_color(other.specular_color)
{
	s_textures_lock.obtain();
	m_diffuse_tex = other.m_diffuse_tex;
	m_specular_tex = other.m_specular_tex;
	m_normal_tex = other.m_normal_tex;
	other.m_diffuse_tex = s_textures.end();
	other.m_specular_tex = s_textures.end();
	other.m_normal_tex = s_textures.end();
	s_textures_lock.release();
}

material::material& operator = (const material& other)
{
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
	return *this;
}

material::material& operator = (material&& other) BOOST_NOEXCEPT_OR_NOTHROW
{
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
	return *this;
}

void material::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	s_textures_lock.obtain();
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
	s_textures_lock.release();
}

bool material::has_diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW
{
	s_textures_lock.obtain();
	bool result = (m_diffuse_tex != s_textures.end());
	s_textures_lock.release();
	return result;
}

bool material::has_specular_texture() const BOOST_NOEXCEPT_OR_NOTHROW
{
	s_textures_lock.obtain();
	bool result = (m_specular_tex != s_textures.end());
	s_textures_lock.release();
	return result;
}

bool material::has_normal_texture() const BOOST_NOEXCEPT_OR_NOTHROW
{
	s_textures_lock.obtain();
	bool result = (m_normal_tex != s_textures.end());
	s_textures_lock.release();
	return result;
}

const opengl::texture2d& material::diffuse_texture() const 
BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_diffuse_tex);
}

const opengl::texture2d& material::specular_texture() const 
BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_specular_tex);
}

const opengl::texture2d& material::normal_texture() const 
BOOST_NOEXCEPT_OR_NOTHROW
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

/*****************************************************************************
 * gintonic::material_dc (diffuse color)                                     *
 ****************************************************************************/

material_dc::material_dc(const vec4f& diffuse_color)
: diffuse_color(diffuse_color)
{
	/* Empty on purpose. */
}

material_dc::~material_dc() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void material_dc::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_dc_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_matrix_VM(renderer::matrix_VM());
	s.set_matrix_N(renderer::matrix_N());
	s.set_diffuse_color(diffuse_color);
}

/*****************************************************************************
 * gintonic::material_dcsc (diffuse color, specular color)                   *
 ****************************************************************************/

material_dcsc::material_dcsc(const vec4f& diffuse_color, const vec4f& specular_color)
: material_dc(diffuse_color)
, specular_color(specular_color)
{
	/* Empty on purpose. */
}

material_dcsc::~material_dcsc() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void material_dcsc::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_dcsc_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_matrix_VM(renderer::matrix_VM());
	s.set_matrix_N(renderer::matrix_N());
	s.set_diffuse_color(diffuse_color);
	s.set_specular_color(specular_color);
}

/*****************************************************************************
 * gintonic::material_dt (diffuse texture)                                   *
 ****************************************************************************/

material_dt::material_dt()
{
	safe_set_null_texture(m_diffuse);
}

material_dt::material_dt(boost::filesystem::path diffuse_filename)
{
	safe_obtain_texture(diffuse_filename, m_diffuse);
}

material_dt::~material_dt() BOOST_NOEXCEPT_OR_NOTHROW
{
	safe_release_texture(m_diffuse);
}

void material_dt::set_diffuse_texture(const boost::filesystem::path& filename)
{
	safe_release_texture(m_diffuse);
	safe_obtain_texture(filename, m_diffuse);
}

const opengl::texture2d& material_dt::diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return std::get<2>(*m_diffuse);
}

void material_dt::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_dt_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_matrix_VM(renderer::matrix_VM());
	s.set_matrix_N(renderer::matrix_N());
	s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
	diffuse_texture().bind(renderer::GBUFFER_TEX_DIFFUSE);
}

/*****************************************************************************
 * gintonic::material_dcdt (diffuse color+texture)                           *
 ****************************************************************************/

material_dcdt::material_dcdt(const vec4f& diffuse_color, boost::filesystem::path diffuse_filename)
: material_dt(std::move(diffuse_filename))
, diffuse_color(diffuse_color)
{
	/* Empty on purpose. */
}

material_dcdt::~material_dcdt() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void material_dcdt::bind() const BOOST_NOEXCEPT_OR_NOTHROW
{
	const auto& s = renderer::get_gp_dcdt_shader();
	s.activate();
	s.set_matrix_PVM(renderer::matrix_PVM());
	s.set_matrix_VM(renderer::matrix_VM());
	s.set_matrix_N(renderer::matrix_N());
	s.set_diffuse_color(diffuse_color);
	s.set_diffuse_texture(renderer::GBUFFER_TEX_DIFFUSE);
	diffuse_texture().bind(renderer::GBUFFER_TEX_DIFFUSE);
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
	s.set_diffuse(renderer::GBUFFER_TEX_DIFFUSE);
	s.set_color(diffuse_color);
	diffuse().bind(renderer::GBUFFER_TEX_DIFFUSE);
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
	s.set_diffuse(renderer::GBUFFER_TEX_DIFFUSE);
	s.set_specular(renderer::GBUFFER_TEX_SPECULAR);
	diffuse().bind(renderer::GBUFFER_TEX_DIFFUSE);
	specular().bind(renderer::GBUFFER_TEX_SPECULAR);
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
	s.set_diffuse(renderer::GBUFFER_TEX_DIFFUSE);
	s.set_normal(renderer::GBUFFER_TEX_NORMAL);
	diffuse().bind(renderer::GBUFFER_TEX_DIFFUSE);
	normal().bind(renderer::GBUFFER_TEX_NORMAL);
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

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::material, boost::serialization::track_always)
BOOST_CLASS_EXPORT(gintonic::material_dc)
BOOST_CLASS_EXPORT(gintonic::material_dcsc)
BOOST_CLASS_EXPORT(gintonic::material_dt)
BOOST_CLASS_EXPORT(gintonic::material_dcdt)
BOOST_CLASS_EXPORT(gintonic::material_c)
BOOST_CLASS_EXPORT(gintonic::material_cd)
BOOST_CLASS_EXPORT(gintonic::material_cds)
BOOST_CLASS_EXPORT(gintonic::material_cdn)