#ifndef gintonic_materials_hpp
#define gintonic_materials_hpp

#include "math.hpp"
#include "locks.hpp"
#include "textures.hpp"
#include <list>

namespace gintonic {

class material;

void serialize(material*, std::ostream& os);
void deserialize(material**, std::istream& is);

class material //: public std::enable_shared_from_this<material>
{
public:
	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~material() BOOST_NOEXCEPT_OR_NOTHROW = default;
	material() = default;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		/* Empty on purpose. */
	}
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".gtm"; }
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

class material_c : public material
{
public:
	material_c() = default;
	material_c(const vec4f& diffuse_color);
	virtual ~material_c() BOOST_NOEXCEPT_OR_NOTHROW = default;
	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	vec4f diffuse_color;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		boost::serialization::void_cast_register<material_c, material>();
		std::cout << "serializing material_c...\n";
		ar & diffuse_color;
	}
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

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW;
private:
	iter_type m_diffuse;
};

class material_cd : public material_c
{
public:
	const opengl::texture2d& diffuse() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_diffuse(const boost::filesystem::path& filename);

	material_cd(
		const vec4f& color,
		boost::filesystem::path diffuse_filename);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~material_cd() BOOST_NOEXCEPT_OR_NOTHROW;

	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & boost::serialization::base_object<material_c>(*this);
		ar & std::get<0>(*m_diffuse);
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		ar & boost::serialization::base_object<material_c>(*this);
		boost::filesystem::path diffuse_filename;
		ar & diffuse_filename;
		set_diffuse(diffuse_filename);
	}
private:
	iter_type m_diffuse;
};

class material_cdn : public material_cd
{
public:
	const opengl::texture2d& normal() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_normal(const boost::filesystem::path&);

	material_cdn(
		const vec4f& color,
		boost::filesystem::path diffuse_filename,
		boost::filesystem::path normal_filename);

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~material_cdn() BOOST_NOEXCEPT_OR_NOTHROW;

	template <class Archive>
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & boost::serialization::base_object<material_cd>(*this);
		ar & std::get<0>(*m_normal);
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		ar & boost::serialization::base_object<material_cd>(*this);
		boost::filesystem::path normal_filename;
		ar & normal_filename;
		set_normal(normal_filename);
	}
private:
	iter_type m_normal;
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

	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
	virtual ~specular_diffuse_material() BOOST_NOEXCEPT_OR_NOTHROW;
private:
	iter_type m_specular;
};

// class lambert_material : public material
// {
// public:

// 	std::shared_ptr<opengl::texture2d> diffuse_value;
// 	float diffuse_factor_value;

// 	lambert_material(
// 		std::shared_ptr<geometry_pass_shader> prog,
// 		std::shared_ptr<opengl::texture2d> diffuse_tex,
// 		const float diffuse_factor_value);

// 	lambert_material(
// 		std::shared_ptr<geometry_pass_shader> prog, 
// 		FbxSurfaceLambert* fbx_material);

// 	virtual ~lambert_material() BOOST_NOEXCEPT_OR_NOTHROW = default;

// 	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;

// private:
// 	std::shared_ptr<geometry_pass_shader> m_program;
// };

// class phong_material : public material
// {
// public:
// 	std::shared_ptr<opengl::texture2d> diffuse_value;
// 	std::shared_ptr<opengl::texture2d> specular_value;
// 	float diffuse_factor_value;
// 	float shininess;

// 	phong_material(
// 		std::shared_ptr<geometry_pass_shader> prog,
// 		std::shared_ptr<opengl::texture2d> diffuse_tex,
// 		std::shared_ptr<opengl::texture2d> specular_tex,
// 		const float diffuse_factor_value,
// 		const float shininess);

// 	phong_material(
// 		std::shared_ptr<geometry_pass_shader> prog, 
// 		FbxSurfacePhong* fbx_material);

// 	virtual ~phong_material() BOOST_NOEXCEPT_OR_NOTHROW = default;

// 	virtual void bind() const BOOST_NOEXCEPT_OR_NOTHROW;
// private:
// 	std::shared_ptr<geometry_pass_shader> m_program;
// };

} // namespace gintonic

namespace boost { namespace serialization {

template <class Archive> 
void load_construct_data(Archive& ar, gintonic::material_cd* mat, const unsigned /*version*/)
{
	gintonic::vec4f color;
	boost::filesystem::path diffuse_filename;
	ar & color & diffuse_filename;
	::new(mat) gintonic::material_cd(color, std::move(diffuse_filename));
}

template <class Archive>
void load_construct_data(Archive& ar, gintonic::material_cdn* mat, const unsigned /*version*/)
{
	gintonic::vec4f color;
	boost::filesystem::path diffuse_filename;
	boost::filesystem::path normal_filename;
	ar & color & diffuse_filename & normal_filename;
	::new(mat) gintonic::material_cdn(color, std::move(diffuse_filename), std::move(normal_filename));
}

}} // namespace boost::serialization

#endif