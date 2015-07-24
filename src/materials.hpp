#ifndef gintonic_materials_hpp
#define gintonic_materials_hpp

#include "math.hpp"
#include "locks.hpp"
#include "filesystem.hpp"
#include <list>

namespace gintonic {

class geometry_pass_shader;
class geometry_null_shader;
namespace opengl { class texture2d; class shader; }

class material : public std::enable_shared_from_this<material>
{
public:
	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~material() BOOST_NOEXCEPT_OR_NOTHROW = default;
	material() = default;
	virtual void serialize(std::ostream&) const;
	virtual const char* extension() const BOOST_NOEXCEPT_OR_NOTHROW { return ".material"; }
protected:
	typedef std::tuple<boost::filesystem::path, std::size_t, opengl::texture2d> item_type;
	typedef std::list<item_type> datastructure_type;
	typedef typename datastructure_type::iterator iter_type;
	static datastructure_type s_textures;
	static write_lock s_textures_lock;
	static void safe_obtain_texture(const boost::filesystem::path& filename, iter_type& iter);
	static void safe_set_null_texture(iter_type& iter);
	static void safe_release_texture(iter_type& iter);
	
};

class color_material : public material
{
public:
	color_material() = default;
	color_material(const vec4f& diffuse_color);
	color_material(std::istream&);
	virtual void serialize(std::ostream&) const;
	virtual ~color_material() BOOST_NOEXCEPT_OR_NOTHROW = default;
	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	vec4f diffuse_color;
	virtual const char* extension() const BOOST_NOEXCEPT_OR_NOTHROW { return ".color_material"; }
};

class diffuse_material : public material
{
public:
	const opengl::texture2d& diffuse() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse(const boost::filesystem::path& filename);
	float diffuse_factor;

	diffuse_material(
		boost::filesystem::path diffuse_filename,
		const float diffuse_factor);
	diffuse_material(std::istream&);
	virtual void serialize(std::ostream&) const;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW;
	virtual const char* extension() const BOOST_NOEXCEPT_OR_NOTHROW { return ".diffuse_material"; }
private:
	iter_type m_diffuse;
};

class material_cd : public color_material
{
public:
	const opengl::texture2d& diffuse() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse(const boost::filesystem::path& filename);

	material_cd(
		const vec4f& color,
		boost::filesystem::path diffuse_filename);
	material_cd(std::istream&);
	virtual void serialize(std::ostream&) const;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~material_cd() BOOST_NOEXCEPT_OR_NOTHROW;
	virtual const char* extension() const BOOST_NOEXCEPT_OR_NOTHROW { return ".material_cd"; }
private:
	iter_type m_diffuse;
};

class specular_diffuse_material : public diffuse_material
{
public:
	const opengl::texture2d& specular() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_specular(const boost::filesystem::path& filename);
	float shininess;

	specular_diffuse_material(
		boost::filesystem::path diffuse_filename,
		boost::filesystem::path specular_filename,
		const float diffuse_factor,
		const float shininess);
	specular_diffuse_material(std::istream&);
	virtual void serialize(std::ostream&) const;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~specular_diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW;
	virtual const char* extension() const BOOST_NOEXCEPT_OR_NOTHROW { return ".specular_diffuse_material"; }
private:
	iter_type m_specular;
};

class lambert_material : public material
{
public:

	std::shared_ptr<opengl::texture2d> diffuse_value;
	float diffuse_factor_value;

	lambert_material(
		std::shared_ptr<geometry_pass_shader> prog,
		std::shared_ptr<opengl::texture2d> diffuse_tex,
		const float diffuse_factor_value);

	lambert_material(
		std::shared_ptr<geometry_pass_shader> prog, 
		FbxSurfaceLambert* fbx_material);

	virtual ~lambert_material() BOOST_NOEXCEPT_OR_NOTHROW = default;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

private:
	std::shared_ptr<geometry_pass_shader> m_program;
};

class phong_material : public material
{
public:
	std::shared_ptr<opengl::texture2d> diffuse_value;
	std::shared_ptr<opengl::texture2d> specular_value;
	float diffuse_factor_value;
	float shininess;

	phong_material(
		std::shared_ptr<geometry_pass_shader> prog,
		std::shared_ptr<opengl::texture2d> diffuse_tex,
		std::shared_ptr<opengl::texture2d> specular_tex,
		const float diffuse_factor_value,
		const float shininess);

	phong_material(
		std::shared_ptr<geometry_pass_shader> prog, 
		FbxSurfacePhong* fbx_material);

	virtual ~phong_material() BOOST_NOEXCEPT_OR_NOTHROW = default;

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	std::shared_ptr<geometry_pass_shader> m_program;
};

} // namespace gintonic

#endif