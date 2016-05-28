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

	using Super = Object<Material, std::string>;

	/**
	 * @name Constructors and destructor
	 */

	//@{
	
	template <class ...Args>
	inline static SharedPtr create(Args&&... args)
	{
		return SharedPtr(new Material(std::forward<Args>(args)...));
	}
	
	/// Destructor.
	virtual ~Material() noexcept = default;

private:

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
	Material(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* fbxmat);

public:

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
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
		archive & diffuseColor;
		archive & specularColor;
		archive & diffuseTexture;
		archive & specularTexture;
		archive & normalTexture;
	}
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Material, boost::serialization::track_always);