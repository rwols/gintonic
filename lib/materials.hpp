/**
 * @file materials.hpp
 * @brief Defines, currently, only one material.
 * @author Raoul Wols
 */

#ifndef gintonic_materials_hpp
#define gintonic_materials_hpp

#include "component.hpp"
#include "vec4f.hpp"
#include "locks.hpp"
#include "opengl/textures.hpp"
#include <list>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

namespace FBX
{
	class FbxSurfaceMaterial; // Forward declaration.
}

namespace gintonic {

class entity; // Forward declaration.

/**
 * @brief A material describes the look of a rendered mesh.
 * 
 * @details The material class manages two four-dimensional vectors
 * representing a diffuse color and a specular color. You can view
 * these as the base color value when a diffuse and specular are
 * attached to the material. A material also manages three optional
 * textures, namely a diffuse texture, a specular texture and a normal
 * map.
 * 
 * To activate, or bind, a material you call the material::bind method.
 * If the mesh is rendered instanced, you must give the material::bind method
 * a value of true. Otherwise the default value of false is used to signal
 * that we are not rendering an instanced mesh.
 * 
 * A material is a component.
 */
class material : public component
{
public:

	/**
	 * @name Rendering
	 */

	//@{

	/**
	 * @brief Bind the material for rendering.
	 * 
	 * @details When this method is called, the material selects the
	 * correct shader class depending on what kind of textures it carries.
	 * The material then activates this shader. When we are not rendering
	 * an instanced mesh, the material then uploads the current PVM matrix,
	 * the VM matrix and N matrix that are set via the renderer.
	 * If we are rendering an instanced mesh, then it is the duty of the
	 * instanced mesh to handle the matrices. Thus no uniform matrices are
	 * set by the material.
	 * 
	 * @param instanced Set wether we are doing instanced rendering or not.
	 * The default is false.
	 */
	void bind(const bool instanced = false) const noexcept;

	//@}

	/**
	 * @name Entity management
	 */

	//@{

	virtual void attach(entity&) final;

	virtual void detach(entity&) final;

	//@}

	/**
	 * @name Constructors and destructor
	 */

	//@{
	
	/// Destructor.
	virtual ~material() noexcept;

	/**
	 * @brief Default constructor.
	 */
	material();


	/**
	 * @brief Constructor.
	 * 
	 * @param diffuse_color The base diffuse color property.
	 */
	material(
		const vec4f& diffuse_color);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuse_color The base diffuse color property.
	 * @param specular_color The base specular color property.
	 */
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuse_color The base diffuse color property.
	 * @param specular_color The base specular color property.
	 * @param diffuse_texture The filepath to the diffuse texture.
	 */
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuse_color The base diffuse color property.
	 * @param specular_color The base specular color property.
	 * @param diffuse_texture The filepath to the diffuse texture.
	 * @param specular_texture The filepath to the specular texture.
	 */
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture,
		const boost::filesystem::path& specular_texture);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuse_color The base diffuse color property.
	 * @param specular_color The base specular color property.
	 * @param diffuse_texture The filepath to the diffuse texture.
	 * @param specular_texture The filepath to the specular texture.
	 * @param normal_texture The filepath to the normal texture.
	 */
	material(
		const vec4f& diffuse_color, 
		const vec4f& specular_color,
		const boost::filesystem::path& diffuse_texture,
		const boost::filesystem::path& specular_texture,
		const boost::filesystem::path& normal_texture);

	/**
	 * @brief Constructor.
	 * 
	 * @details Converts an FBX surface material into our own material.
	 * The constructor will output various log information to the standard
	 * output stream.
	 * 
	 * @param fbxmat The FBX material.
	 */
	material(const FBX::FbxSurfaceMaterial* fbxmat);

	/// Copy constructor.
	material(const material&);

	/// Move constructor.
	material(material&&) noexcept;

	/// Copy assignment operator.
	material& operator = (const material&);

	/// Move assignment operator.
	material& operator = (material&&) noexcept;

	//@}

	/**
	 * @name Loading and saving materials
	 */

	//@{

	/**
	 * @brief Returns the default extension of a material for a filename.
	 * 
	 * @return Always returns the literal string ".gtm".
	 */
	inline static const char* extension() noexcept
	{
		return ".gtm";
	}

	/**
	 * @brief Serialize a material to a binary output stream.
	 * 
	 * @details The material is serialized to binary. You have to make sure
	 * that the output stream is in binary mode.
	 * 
	 * @param os The binary output stream.
	 */
	void save(std::ostream& os) const;

	/**
	 * @brief Serialize a material to a file.
	 * 
	 * @details If the file already exists, it is bluntly overwritten.
	 * 
	 * @param p The filepath to serialize to.
	 */
	void save(const boost::filesystem::path& p) const;

	/**
	 * @brief Serialize a material to a file.
	 * 
	 * @details If the file already exists, it is bluntly overwritten.
	 * 
	 * @param filename The filepath to serialize to.
	 */
	void save(const std::string& filename) const;

	/**
	 * @brief Serialize a material to a file.
	 * 
	 * @details If the file already exists, it is bluntly overwritten.
	 * 
	 * @param filename The filepath to serialize to.
	 */
	void save(const char* filename) const;

	/**
	 * @brief Deserialize a material from a binary input stream.
	 * 
	 * @details The material is deserialized from binary. You have to make
	 * sure that the input stream is in binary mode.
	 * 
	 * @param is The binary input stream.
	 * 
	 * @return The deserialized material.
	 */
	static material load(std::istream& is);

	/**
	 * @brief Deserialize a material from a file.
	 * 
	 * @param p The filepath to deserialize from.
	 * 
	 * @return The deserialized material.
	 */
	static material load(const boost::filesystem::path& p);

	/**
	 * @brief Deserialize a material from a file.
	 * 
	 * @param filename The filepath to deserialize from.
	 * 
	 * @return The deserialized material.
	 */
	static material load(const std::string& filename);

	/**
	 * @brief Deserialize a material from a file.
	 * 
	 * @param filename The filepath to deserialize from.
	 * 
	 * @return The deserialized material.
	 */
	static material load(const char* filename);

	//@}

	/**
	 * @name Property management
	 */

	//@{

	/// The diffuse color property.
	vec4f diffuse_color;

	/// The specular color property.
	vec4f specular_color;

	/**
	 * @brief Check wether the material has a diffuse texture.
	 * @return True if the material has a diffuse texture, false otherwise.
	 */
	bool has_diffuse_texture() const noexcept;

	/**
	 * @brief Check wether the material has a specular texture.
	 * @return True if the material has a specular texture, false otherwise.
	 */
	bool has_specular_texture() const noexcept;

	/**
	 * @brief Check wether the material has a normal texture.
	 * @return True if the material has a normal texture, false otherwise.
	 */
	bool has_normal_texture() const noexcept;

	// Methods to obtain a const reference to various textures.
	// Be sure to first check if the material has such textures.

	/**
	 * @brief Obtain a constant reference to the diffuse texture.
	 * @details If the material has no diffuse texture, then this will crash
	 * the application. Be sure to check if the material has a diffuse
	 * texture before calling this method.
	 * @return A constant reference to the diffuse texture.
	 */
	const opengl::texture2d& diffuse_texture() const
		noexcept;

	/**
	 * @brief Obtain a constant reference to the specular texture.
	 * @details If the material has no specular texture, then this will crash
	 * the application. Be sure to check if the material has a specular
	 * texture before calling this method.
	 * @return A constant reference to the specular texture.
	 */
	const opengl::texture2d& specular_texture() const
		noexcept;

	/**
	 * @brief Obtain a constant reference to the normal texture.
	 * @details If the material has no normal texture, then this will crash
	 * the application. Be sure to check if the material has a normal
	 * texture before calling this method.
	 * @return A constant reference to the normal texture.
	 */
	const opengl::texture2d& normal_texture() const
		noexcept;

	/**
	 * @brief Set the diffuse texture.
	 * 
	 * @param p The filepath to the diffuse texture.
	 */
	void set_diffuse_texture(const boost::filesystem::path& p);

	/**
	 * @brief Set the specular texture.
	 * 
	 * @param p The filepath to the specular texture.
	 */
	void set_specular_texture(const boost::filesystem::path& p);

	/**
	 * @brief Set the normal texture.
	 * 
	 * @param p The filepath to the normal texture.
	 */
	void set_normal_texture(const boost::filesystem::path& p);

	/**
	 * @brief Clear the diffuse texture.
	 */
	void clear_diffuse_texture();

	/**
	 * @brief Clear the specular texture.
	 */
	void clear_specular_texture();

	/**
	 * @brief Clear the normal texture.
	 */
	void clear_normal_texture();

	//@}

	/**
	 * @brief Debug stream output.
	 * @details Do not use this method for serialization. Instead use the
	 * various material::save methods.
	 * 
	 * @param os The output stream.
	 * @param m The material.
	 */
	friend std::ostream& operator << (std::ostream& os, const material& m);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

protected:

	/**
	 * @brief The item type of the global texture datastructure.
	 */
	typedef std::tuple
	<
		boost::filesystem::path, 
		std::size_t, 
		opengl::texture2d
	> item_type;

	/**
	 * @brief The datastructure type of the textures.
	 */
	typedef std::list<item_type> datastructure_type;
	
	/**
	 * @brief The iterator type of the global textures list.
	 */
	typedef datastructure_type::iterator iter_type;

	/**
	 * @brief Attempt to set the iterator to a texture in the global map.
	 * @details Before attempting, this method requests a write-lock on the
	 * global textures map.
	 * 
	 * @param filename The filepath to the texture.
	 * @param iter The iterator to set.
	 */
	static void safe_obtain_texture(
		const boost::filesystem::path& filename, 
		iter_type& iter);

	/**
	 * @brief Attempt to set the iterator to a texture in the global map.
	 * @details This method does not request a write-lock on the global
	 * textures map. Therefore you must do so yourself.
	 * 
	 * @param filename The filepath to the texture.
	 * @param iter The iterator to set.
	 */
	static void unsafe_obtain_texture(
		const boost::filesystem::path& filename,
		iter_type& iter);

	/**
	 * @brief Set the iterator to point to nothing.
	 * @details Before setting the iterator to point to nothing, this method
	 * requests a write-lock on the global texures map.
	 * 
	 * @param iter The iterator to set.
	 */
	static void safe_set_null_texture(iter_type& iter);

	/**
	 * @brief Decrease the reference count of the iterator by one.
	 * @details Before decreasing the reference count of the iterator by one,
	 * this method requests a write-lock on the global textures map. When the
	 * reference count has reached zero, the texure is actually removed from
	 * the global textures map.
	 * 
	 * @param iter The iterator to release.
	 */
	static void safe_release_texture(iter_type& iter);

	/**
	 * @brief Decrease the reference count of the iterator by one.
	 * @details This method does not request a write-lock on the global
	 * textures map. Therefore you must do so yourself. When the reference
	 * count has reached zero, the texure is actually removed from the global 
	 * textures map.
	 * 
	 * @param iter The iterator to release.
	 */
	static void unsafe_release_texture(iter_type& iter);

private:

	static datastructure_type s_textures;
	
	static write_lock s_textures_lock;

	friend class boost::serialization::access;
	
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
};

} // namespace gintonic

#endif