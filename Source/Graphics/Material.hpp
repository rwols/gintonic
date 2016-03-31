/**
 * @file Material.hpp
 * @brief Defines a material for rendering.
 * @author Raoul Wols
 */

#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/Object.hpp"

#include "../Math/vec4f.hpp"

#include "Texture2D.hpp"
 
#include <list>
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

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
class Material : public Object<Material, std::string>
{
public:

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

	/**
	 * @brief Debug stream output.
	 * @param os The output stream.
	 * @param m The material.
	 */
	friend std::ostream& operator << (std::ostream& os, const Material& m);

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& archive, const unsigned int /*version*/)
	{
		archive & boost::serialization::base_object<Object<Material, std::string>>(*this);
		archive & diffuseColor;
		archive & specularColor;
		archive & diffuseTexture;
		archive & specularTexture;
		archive & normalTexture;
	}
	
	// template <class Archive> 
	// void save(Archive& ar, const unsigned /*version*/) const
	// {
	// 	// Serialize the base class.
	// 	ar & boost::serialization::base_object<Object<Material, std::string>>(*this);

	// 	// Serialize the diffuse color and the specular color.
	// 	ar & BOOST_SERIALIZATION_NVP(diffuseColor);
	// 	ar & BOOST_SERIALIZATION_NVP(specularColor);

	// 	// Determine what textures we carry.
	// 	bool lHasDiffuse = diffuseTexture != nullptr;
	// 	bool lHasSpecular = specularTexture != nullptr;
	// 	bool lHasNormal = normalTexture != nullptr;

	// 	// Serialize what textures we carry.
	// 	ar & boost::serialization::make_nvp("hasDiffuseTexture", lHasDiffuse);
	// 	ar & boost::serialization::make_nvp("hasSpecularTexture", lHasSpecular);
	// 	ar & boost::serialization::make_nvp("hasNormalTexture", lHasNormal);

	// 	if (lHasDiffuse)
	// 	{
	// 		// We carry a diffuse texture, serialize its filename.
	// 		ar & boost::serialization::make_nvp("diffuseTexture", diffuseTexture->name);
	// 	}
	// 	if (lHasSpecular)
	// 	{
	// 		// We carry a specular texture, serialize its filename.
	// 		ar & boost::serialization::make_nvp("specularTexture", specularTexture->name);
	// 	}
	// 	if (lHasNormal)
	// 	{
	// 		// We carry a normal texture, serialize its filename.
	// 		ar & boost::serialization::make_nvp("normalTexture", normalTexture->name);
	// 	}
	// }

	// template <class Archive>
	// void load(Archive& ar, const unsigned /*version*/)
	// {
	// 	// Deserialize the base class.
	// 	ar & boost::serialization::base_object<Object<Material, std::string>>(*this);

	// 	// Deserialize the diffuse color and specular color.
	// 	ar & diffuseColor;
	// 	ar & specularColor;

	// 	bool lHasDiffuse;
	// 	bool lHasSpecular;
	// 	bool lHasNormal;
	// 	boost::filesystem::path lFilename;

	// 	// Determine what textures we carry.
	// 	ar & boost::serialization::make_nvp("hasDiffuseTexture", lHasDiffuse);
	// 	ar & boost::serialization::make_nvp("hasSpecularTexture", lHasSpecular);
	// 	ar & boost::serialization::make_nvp("hasNormalTexture", lHasNormal);

	// 	if (lHasDiffuse)
	// 	{
	// 		// Deserialize the filename.
	// 		ar & boost::serialization::make_nvp("diffuseTexture", lFilename);
			
	// 		// Load the texture.
	// 		diffuseTexture = std::make_shared<Texture2D>(lFilename);
	// 	}

	// 	if (lHasSpecular)
	// 	{
	// 		// Deserialize the filename.
	// 		ar & boost::serialization::make_nvp("specularTexture", lFilename);

	// 		// Load the texture.
	// 		specularTexture = std::make_shared<Texture2D>(lFilename);
	// 	}

	// 	if (lHasNormal)
	// 	{
	// 		// Deserialize the filename.
	// 		ar & boost::serialization::make_nvp("normalTexture", lFilename);
			
	// 		// Load the texture.
	// 		normalTexture = std::make_shared<Texture2D>(lFilename);
	// 	}
	// }

	// BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Material, boost::serialization::track_always);