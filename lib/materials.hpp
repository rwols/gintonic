#ifndef gintonic_materials_hpp
#define gintonic_materials_hpp

#include "math.hpp"
#include "locks.hpp"
#include "textures.hpp"
#include <list>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

namespace gintonic {

/*****************************************************************************
* gintonic::material                                                         *
*****************************************************************************/

class material
{
public:

	// Bind the material.
	virtual void bind(const bool instanced = false) 
		const BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Destructor.
	virtual ~material() BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Default constructor.
	material();

	// Constructor that sets the diffuse color.
	material(
		const vec4f& diffuse_color);

	// Constructor that sets the diffuse color and the specular color.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color);

	// Constructor that sets the diffuse color,
	// the specular color and
	// the diffuse texture.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture);

	// Constructor that sets the diffuse color,
	// the specular color,
	// the diffuse texture and
	// the specular texture.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture,
		const boost::filesystem::path& specular_texture);

	// Constructor that sets the diffuse color,
	// the specular color,
	// the diffuse texture,
	// the specular texture and
	// the normal texture.
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture,
		const boost::filesystem::path& specular_texture,
		const boost::filesystem::path& normal_texture);

	// Copy constructor.
	material(const material&);

	// Move constructor.
	material(material&&) BOOST_NOEXCEPT_OR_NOTHROW;

	// Copy assignment operator.
	material& operator = (const material&);

	// Move assignment operator.
	material& operator = (material&&) BOOST_NOEXCEPT_OR_NOTHROW;

	// Static method that returns the default extension for a material.
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return ".gtm";
	}

	// Methods to save a material to a stream or file.
	void save(std::ostream&) const;
	void save(const boost::filesystem::path&) const;
	void save(const std::string&) const;
	void save(const char*) const;

	// Static methods to load a material from a stream or file.
	static material load(std::istream&);
	static material load(const boost::filesystem::path&);
	static material load(const std::string&);
	static material load(const char*);

	// The diffuse color property.
	vec4f diffuse_color;

	// The specular color property.
	vec4f specular_color;

	// Methods to determine if a material carries various textures.
	bool has_diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	bool has_specular_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	bool has_normal_texture() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Methods to obtain a const reference to various textures.
	// Be sure to first check if the material has such textures.
	const opengl::texture2d& diffuse_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	const opengl::texture2d& specular_texture() const BOOST_NOEXCEPT_OR_NOTHROW;
	const opengl::texture2d& normal_texture() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Methods to set various textures.
	void set_diffuse_texture(const boost::filesystem::path&);
	void set_specular_texture(const boost::filesystem::path&);
	void set_normal_texture(const boost::filesystem::path&);

	// Methods to clear various textures.
	void clear_diffuse_texture();
	void clear_specular_texture();
	void clear_normal_texture();

	// Stream output operator support.
	// Don't use this for serialization, instead use the save/load methods.
	friend std::ostream& operator << (std::ostream&, const material&);

	// Needed because a material carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

protected:

	typedef std::tuple
	<
		boost::filesystem::path, 
		std::size_t, 
		opengl::texture2d
	> item_type;

	typedef std::list<item_type> datastructure_type;
	
	typedef datastructure_type::iterator iter_type;

	static void safe_obtain_texture(
		const boost::filesystem::path& filename, 
		iter_type& iter);

	static void unsafe_obtain_texture(
		const boost::filesystem::path& filename,
		iter_type& iter);

	static void safe_set_null_texture(iter_type& iter);

	static void safe_release_texture(iter_type& iter);

	static void unsafe_release_texture(iter_type& iter);

private:

	static datastructure_type s_textures;
	
	static write_lock s_textures_lock;

	friend boost::serialization::access;
	
	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		// Serialize the diffuse color and the specular color.
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
		ar & BOOST_SERIALIZATION_NVP(specular_color);

		// Obtain a lock on the global texture container.
		s_textures_lock.obtain(); 

		// Determine what textures we carry.
		bool has_diffuse = m_diffuse_tex != s_textures.end();
		bool has_specular = m_specular_tex != s_textures.end();
		bool has_normal = m_normal_tex != s_textures.end();

		boost::filesystem::path filename;

		// Serialize if we carry a diffuse texture.
		ar & BOOST_SERIALIZATION_NVP(has_diffuse);
		if (has_diffuse)
		{
			// If we do, serialize the filename of the diffuse texture.
			filename = std::get<0>(*m_diffuse_tex);
			ar & boost::serialization::make_nvp("diffuse_texture", filename);
		}

		// Serialize if we carry a specular texture.
		ar & BOOST_SERIALIZATION_NVP(has_specular);
		if (has_specular)
		{
			// If we do, serialize the filename of the specular texture.
			filename = std::get<0>(*m_specular_tex);
			ar & boost::serialization::make_nvp("specular_texture", filename);
		}

		// Serialize if we carry a normal texture.
		ar & BOOST_SERIALIZATION_NVP(has_normal);
		if (has_normal)
		{
			// If we do, serialize the filename of the normal texture.
			filename = std::get<0>(*m_normal_tex);
			ar & boost::serialization::make_nvp("normal_texture", filename);
		}

		// Release the lock on global texture container.
		s_textures_lock.release(); 
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		// Load the diffuse color and the specular color.
		// These are always present.
		ar & BOOST_SERIALIZATION_NVP(diffuse_color);
		ar & BOOST_SERIALIZATION_NVP(specular_color);

		// Obtain a lock on the global texture container.
		s_textures_lock.obtain();

		bool has_diffuse;
		bool has_specular;
		bool has_normal;
		boost::filesystem::path filename;

		// Check if we need to load a diffuse texture.
		ar & BOOST_SERIALIZATION_NVP(has_diffuse);
		if (has_diffuse)
		{
			// If we do, then retrieve the filename.
			ar & boost::serialization::make_nvp("diffuse_texture", filename);
			// Load the texture. We can do this the "unsafe" way since we
			// already have a lock anyway.
			unsafe_obtain_texture(filename, m_diffuse_tex);
		}
		else
		{
			// Otherwise, set our diffuse iterator to point
			// to the end of the global texture container.
			m_diffuse_tex = s_textures.end();
		}

		// Check if we need to load a specular texture.
		ar & BOOST_SERIALIZATION_NVP(has_specular);
		if (has_specular)
		{
			// If we do, then retrieve the filename.
			ar & boost::serialization::make_nvp("specular_texture", filename);
			// Load the texture. We can do this the "unsafe" way since we
			// already have a lock anyway.
			unsafe_obtain_texture(filename, m_specular_tex);
		}
		else
		{
			// Otherwise, set our diffuse iterator to point
			// to the end of the global texture container.
			m_specular_tex = s_textures.end();
		}

		// Check if we need to load a normal texture.
		ar & BOOST_SERIALIZATION_NVP(has_normal);
		if (has_normal)
		{
			// If we do, then retrieve the filename.
			ar & boost::serialization::make_nvp("normal_texture", filename);
			// Load the texture. We can do this the "unsafe" way since we
			// already have a lock anyway.
			unsafe_obtain_texture(filename, m_normal_tex);
		}
		else
		{
			// Otherwise, set our normal iterator to point
			// to the end of the global texture container.
			m_normal_tex = s_textures.end();
		}

		// Release the lock on global texture container.
		s_textures_lock.release();
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	iter_type m_diffuse_tex;
	iter_type m_specular_tex;
	iter_type m_normal_tex;

	friend class material_component_diffuse_texture;
	friend class material_component_specular_texture;
	friend class material_component_normal_texture;	
};

} // namespace gintonic

#endif