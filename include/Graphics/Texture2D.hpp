/**
 * @file Texture2D.hpp
 * @author Raoul Wols
 * @brief Defines the Texture2D class.
 */

#pragma once

#include "config.hpp"

#include "Asset.hpp"

#include "Foundation/filesystem.hpp"
#include "Foundation/Object.hpp"

#include "OpenGL/TextureObject.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

/**
 * @brief Simple two-dimensional texture. You can bind it to a texture unit.
 */
class Texture2D 
: public Object<Texture2D, boost::filesystem::path>
, public Asset
{
	GINTONIC_ASSET(Texture2D, "textures", ".tex")

public:

	/**
	 * @brief Can be thrown during construction of a Texture2D.
	 */
	class NoImageDataException : public std::exception
	{
	public:
		inline NoImageDataException() noexcept {};
		~NoImageDataException() noexcept override = default;
		const char* what() const noexcept override { return "NoImageDataException"; }
	};

	/**
	 * @brief Can be thrown during construction of a Texture2D.
	 */
	class UnknownImageFormatException : public std::exception
	{
	public:
		inline UnknownImageFormatException() noexcept {};
		~UnknownImageFormatException() noexcept override = default;
		const char* what() const noexcept override { return "UnknownImageFormatException"; }
	};

	// template <class ...Args>
	// inline static SharedPtr create(Args&&... args)
	// {
	// 	return SharedPtr(new Texture2D(std::forward<Args>(args)...));
	// }

	struct ImageLoadOptions
	{
		std::string relativeFilename;
	};

	static std::shared_ptr<Texture2D> fromImage(const ImageLoadOptions& options);
	
	/// Defaulted destructor.
	~Texture2D() noexcept override = default;

	/**
	 * @brief Bind this texture to the given OpenGL texture unit.
	 * @param textureUnit The texture unit.
	 * @attention Counting starts from 0, not from GL_TEXTURE0. This
	 * unifies sampler2D and texture units.
	 */
	void bind(const GLint textureUnit) const noexcept;

private:

	Texture2D() = default;

	OpenGL::TextureObject mTextureObject;
	std::string imageFile;

	void loadFromFile(boost::filesystem::path);

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& archive, const unsigned int /*version*/) const
	{
		using namespace boost::serialization;
		using namespace boost::filesystem;
		archive & base_object<Object<Texture2D, path>>(*this);
		archive & base_object<Asset>(*this);
		archive & imageFile;
	}

	template <class Archive>
	void load(Archive& archive, const unsigned int /*version*/)
	{
		using namespace boost::serialization;
		using namespace boost::filesystem;
		archive & base_object<Object<Texture2D, path>>(*this);
		archive & base_object<Asset>(*this);
		archive & imageFile;
		loadFromFile(path(Asset::getAssetFolder()) / path(imageFile));
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Texture2D, boost::serialization::track_always);
