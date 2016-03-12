/**
 * @file materials.hpp
 * @brief Defines, currently, only one material.
 * @author Raoul Wols
 */

#pragma once

#include "Object.hpp"
#include "vec4f.hpp"
#include "locks.hpp"
// #include "opengl/textures.hpp"
#include "Texture2D.hpp"
#include <list>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

namespace FBX
{
	class FbxSurfaceMaterial; // Forward declaration.
}

namespace gintonic {

// class entity; // Forward declaration.

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
 */
class Material : public Object<Material>
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
	 * @name Constructors and destructor
	 */

	//@{
	
	/// Destructor.
	virtual ~Material() noexcept = default;

	/**
	 * @brief Default constructor.
	 */
	Material() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuseColor The base diffuse color property.
	 */
	Material(
		const vec4f& diffuseColor);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuseColor The base diffuse color property.
	 * @param specularColor The base specular color property.
	 */
	Material(
		const vec4f& diffuseColor, 
		const vec4f& specularColor);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuseColor The base diffuse color property.
	 * @param specularColor The base specular color property.
	 * @param diffuseTexture The filepath to the diffuse texture.
	 */
	Material(
		const vec4f& diffuseColor, 
		const vec4f& specularColor,
		const boost::filesystem::path& diffuseTexture);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuseColor The base diffuse color property.
	 * @param specularColor The base specular color property.
	 * @param diffuseTexture The filepath to the diffuse texture.
	 * @param specularTexture The filepath to the specular texture.
	 */
	Material(
		const vec4f& diffuseColor, 
		const vec4f& specularColor,
		const boost::filesystem::path& diffuseTexture,
		const boost::filesystem::path& specularTexture);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuseColor The base diffuse color property.
	 * @param specularColor The base specular color property.
	 * @param diffuseTexture The filepath to the diffuse texture.
	 * @param specularTexture The filepath to the specular texture.
	 * @param normalTexture The filepath to the normal texture.
	 */
	Material(
		const vec4f& diffuseColor, 
		const vec4f& specularColor,
		const boost::filesystem::path& diffuseTexture,
		const boost::filesystem::path& specularTexture,
		const boost::filesystem::path& normalTexture);

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuseColor The base diffuse color property.
	 * @param specularColor The base specular color property.
	 * @param diffuseTexture Shared pointer to the diffuse texture.
	 * @param specularTexture Shared pointer to the specular texture.
	 * @param normalTexture Shared pointer to the normal texture.
	 */
	Material(
		const vec4f& diffuseColor,
		const vec4f& specularColor,
		std::shared_ptr<Texture2D> diffuseTexture,
		std::shared_ptr<Texture2D> specularTexture,
		std::shared_ptr<Texture2D> normalTexture);

	/**
	 * @brief Constructor.
	 * 
	 * @details Converts an FBX surface material into our own material.
	 * The constructor will output various log information to the standard
	 * output stream.
	 * 
	 * @param fbxmat The FBX material.
	 */
	Material(const FBX::FbxSurfaceMaterial* fbxmat);

	// /// Copy constructor.
	// Material(const Material&);

	// /// Move constructor.
	// Material(Material&&) noexcept;

	// /// Copy assignment operator.
	// Material& operator = (const Material&);

	// /// Move assignment operator.
	// Material& operator = (Material&&) noexcept;

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

	// /**
	//  * @brief Serialize a material to a binary output stream.
	//  * 
	//  * @details The material is serialized to binary. You have to make sure
	//  * that the output stream is in binary mode.
	//  * 
	//  * @param os The binary output stream.
	//  */
	// void save(std::ostream& os) const;

	// /**
	//  * @brief Serialize a material to a file.
	//  * 
	//  * @details If the file already exists, it is bluntly overwritten.
	//  * 
	//  * @param p The filepath to serialize to.
	//  */
	// void save(const boost::filesystem::path& p) const;

	// /**
	//  * @brief Serialize a material to a file.
	//  * 
	//  * @details If the file already exists, it is bluntly overwritten.
	//  * 
	//  * @param filename The filepath to serialize to.
	//  */
	// void save(const std::string& filename) const;

	// /**
	//  * @brief Serialize a material to a file.
	//  * 
	//  * @details If the file already exists, it is bluntly overwritten.
	//  * 
	//  * @param filename The filepath to serialize to.
	//  */
	// void save(const char* filename) const;

	// *
	//  * @brief Deserialize a material from a binary input stream.
	//  * 
	//  * @details The material is deserialized from binary. You have to make
	//  * sure that the input stream is in binary mode.
	//  * 
	//  * @param is The binary input stream.
	//  * 
	//  * @return The deserialized material.
	 
	// static Material load(std::istream& is);

	// /**
	//  * @brief Deserialize a material from a file.
	//  * 
	//  * @param p The filepath to deserialize from.
	//  * 
	//  * @return The deserialized material.
	//  */
	// static Material load(const boost::filesystem::path& p);

	// /**
	//  * @brief Deserialize a material from a file.
	//  * 
	//  * @param filename The filepath to deserialize from.
	//  * 
	//  * @return The deserialized material.
	//  */
	// static Material load(const std::string& filename);

	// /**
	//  * @brief Deserialize a material from a file.
	//  * 
	//  * @param filename The filepath to deserialize from.
	//  * 
	//  * @return The deserialized material.
	//  */
	// static Material load(const char* filename);

	//@}

	/**
	 * @name Property management
	 */

	//@{

	/// The diffuse color property.
	vec4f diffuseColor;

	/// The specular color property.
	vec4f specularColor;

	/// The diffuse texture.
	std::shared_ptr<Texture2D> diffuseTexture;

	/// The specular texture.
	std::shared_ptr<Texture2D> specularTexture;

	/// The normal texture.
	std::shared_ptr<Texture2D> normalTexture;

	// /**
	//  * @brief Check wether the material has a diffuse texture.
	//  * @return True if the material has a diffuse texture, false otherwise.
	//  */
	// bool hasDiffuseTexture() const noexcept;

	// /**
	//  * @brief Check wether the material has a specular texture.
	//  * @return True if the material has a specular texture, false otherwise.
	//  */
	// bool hasSpecularTexture() const noexcept;

	// /**
	//  * @brief Check wether the material has a normal texture.
	//  * @return True if the material has a normal texture, false otherwise.
	//  */
	// bool hasNormalTexture() const noexcept;

	// // Methods to obtain a const reference to various textures.
	// // Be sure to first check if the material has such textures.

	// /**
	//  * @brief Obtain a constant reference to the diffuse texture.
	//  * @details If the material has no diffuse texture, then this will crash
	//  * the application. Be sure to check if the material has a diffuse
	//  * texture before calling this method.
	//  * @return A constant reference to the diffuse texture.
	//  */
	// const opengl::texture2d& diffuseTexture() const
	// 	noexcept;

	// *
	//  * @brief Obtain a constant reference to the specular texture.
	//  * @details If the material has no specular texture, then this will crash
	//  * the application. Be sure to check if the material has a specular
	//  * texture before calling this method.
	//  * @return A constant reference to the specular texture.
	 
	// const opengl::texture2d& specularTexture() const
	// 	noexcept;

	// /**
	//  * @brief Obtain a constant reference to the normal texture.
	//  * @details If the material has no normal texture, then this will crash
	//  * the application. Be sure to check if the material has a normal
	//  * texture before calling this method.
	//  * @return A constant reference to the normal texture.
	//  */
	// const opengl::texture2d& normalTexture() const
	// 	noexcept;

	// /**
	//  * @brief Set the diffuse texture.
	//  * 
	//  * @param p The filepath to the diffuse texture.
	//  */
	// void setDiffuseTexture(const boost::filesystem::path& p);

	// /**
	//  * @brief Set the specular texture.
	//  * 
	//  * @param p The filepath to the specular texture.
	//  */
	// void setSpecularTexture(const boost::filesystem::path& p);

	// /**
	//  * @brief Set the normal texture.
	//  * 
	//  * @param p The filepath to the normal texture.
	//  */
	// void setNormalTexture(const boost::filesystem::path& p);

	// /**
	//  * @brief Clear the diffuse texture.
	//  */
	// void clearDiffuseTexture();

	// /**
	//  * @brief Clear the specular texture.
	//  */
	// void clearSpecularTexture();

	// /**
	//  * @brief Clear the normal texture.
	//  */
	// void clearNormalTexture();

	//@}

	/**
	 * @brief Debug stream output.
	 * @details Do not use this method for serialization. Instead use the
	 * various material::save methods.
	 * 
	 * @param os The output stream.
	 * @param m The material.
	 */
	friend std::ostream& operator << (std::ostream& os, const Material& m);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

// protected:

// 	/**
// 	 * @brief The item type of the global texture datastructure.
// 	 */
// 	typedef std::tuple
// 	<
// 		boost::filesystem::path, 
// 		std::size_t, 
// 		opengl::texture2d
// 	> ItemType;

// 	/**
// 	 * @brief The datastructure type of the textures.
// 	 */
// 	typedef std::list<ItemType> DataStructureType;
	
// 	/**
// 	 * @brief The iterator type of the global textures list.
// 	 */
// 	typedef DataStructureType::iterator IterType;

// 	/**
// 	 * @brief Attempt to set the iterator to a texture in the global map.
// 	 * @details Before attempting, this method requests a write-lock on the
// 	 * global textures map.
// 	 * 
// 	 * @param filename The filepath to the texture.
// 	 * @param iter The iterator to set.
// 	 */
// 	static void safeObtainTexture(
// 		const boost::filesystem::path& filename, 
// 		IterType& iter);

// 	/**
// 	 * @brief Attempt to set the iterator to a texture in the global map.
// 	 * @details This method does not request a write-lock on the global
// 	 * textures map. Therefore you must do so yourself.
// 	 * 
// 	 * @param filename The filepath to the texture.
// 	 * @param iter The iterator to set.
// 	 */
// 	static void unsafeObtainTexture(
// 		const boost::filesystem::path& filename,
// 		IterType& iter);

// 	/**
// 	 * @brief Set the iterator to point to nothing.
// 	 * @details Before setting the iterator to point to nothing, this method
// 	 * requests a write-lock on the global texures map.
// 	 * 
// 	 * @param iter The iterator to set.
// 	 */
// 	static void safeSetNullTexture(IterType& iter);

// 	/**
// 	 * @brief Decrease the reference count of the iterator by one.
// 	 * @details Before decreasing the reference count of the iterator by one,
// 	 * this method requests a write-lock on the global textures map. When the
// 	 * reference count has reached zero, the texure is actually removed from
// 	 * the global textures map.
// 	 * 
// 	 * @param iter The iterator to release.
// 	 */
// 	static void safeReleaseTexture(IterType& iter);

// 	/**
// 	 * @brief Decrease the reference count of the iterator by one.
// 	 * @details This method does not request a write-lock on the global
// 	 * textures map. Therefore you must do so yourself. When the reference
// 	 * count has reached zero, the texure is actually removed from the global 
// 	 * textures map.
// 	 * 
// 	 * @param iter The iterator to release.
// 	 */
// 	static void unsafeReleaseTexture(IterType& iter);

private:

	friend class boost::serialization::access;
	
	template <class Archive> 
	void save(Archive& ar, const unsigned /*version*/) const
	{
		// Serialize the base class.
		ar & boost::serialization::base_object<Object<Material>>(*this);

		// Serialize the diffuse color and the specular color.
		ar & BOOST_SERIALIZATION_NVP(diffuseColor);
		ar & BOOST_SERIALIZATION_NVP(specularColor);

		// Determine what textures we carry.
		bool lHasDiffuse = diffuseTexture != nullptr;
		bool lHasSpecular = specularTexture != nullptr;
		bool lHasNormal = normalTexture != nullptr;

		// Serialize what textures we carry.
		ar & boost::serialization::make_nvp("hasDiffuseTexture", lHasDiffuse);
		ar & boost::serialization::make_nvp("hasSpecularTexture", lHasSpecular);
		ar & boost::serialization::make_nvp("hasNormalTexture", lHasNormal);

		if (lHasDiffuse)
		{
			// We carry a diffuse texture, serialize its filename.
			ar & boost::serialization::make_nvp("diffuseTexture", diffuseTexture->getFilename());
		}
		if (lHasSpecular)
		{
			// We carry a specular texture, serialize its filename.
			ar & boost::serialization::make_nvp("specularTexture", specularTexture->getFilename());
		}
		if (lHasNormal)
		{
			// We carry a normal texture, serialize its filename.
			ar & boost::serialization::make_nvp("normalTexture", normalTexture->getFilename());
		}
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		// Deserialize the base class.
		ar & boost::serialization::base_object<Object<Material>>(*this);

		// Deserialize the diffuse color and specular color.
		ar & diffuseColor;
		ar & specularColor;

		bool lHasDiffuse;
		bool lHasSpecular;
		bool lHasNormal;
		boost::filesystem::path lFilename;

		// Determine what textures we carry.
		ar & boost::serialization::make_nvp("hasDiffuseTexture", lHasDiffuse);
		ar & boost::serialization::make_nvp("hasSpecularTexture", lHasSpecular);
		ar & boost::serialization::make_nvp("hasNormalTexture", lHasNormal);

		if (lHasDiffuse)
		{
			// Deserialize the filename.
			ar & boost::serialization::make_nvp("diffuseTexture", lFilename);
			
			// Load the texture.
			diffuseTexture = std::make_shared<Texture2D>(lFilename);
		}

		if (lHasSpecular)
		{
			// Deserialize the filename.
			ar & boost::serialization::make_nvp("specularTexture", lFilename);

			// Load the texture.
			specularTexture = std::make_shared<Texture2D>(lFilename);
		}

		if (lHasNormal)
		{
			// Deserialize the filename.
			ar & boost::serialization::make_nvp("normalTexture", lFilename);
			
			// Load the texture.
			normalTexture = std::make_shared<Texture2D>(lFilename);
		}
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace gintonic
