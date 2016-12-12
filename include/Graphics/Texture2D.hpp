/**
 * @file Texture2D.hpp
 * @author Raoul Wols
 * @brief Defines the Texture2D class.
 */

#pragma once

#include "config.hpp"

#include "../Foundation/filesystem.hpp"
#include "../Foundation/Object.hpp"

#include "OpenGL/TextureObject.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

// namespace FBX
// {
// 	class FbxTexture; // Forward declaration.
// }

namespace gintonic {

/**
 * @brief Simple two-dimensional texture. You can bind it to a texture unit.
 */
class Texture2D : public Object<Texture2D, boost::filesystem::path>
{
public:

	/**
	 * @brief Can be thrown during construction of a Texture2D.
	 */
	class NoImageDataException : public std::exception
	{
	public:
		inline NoImageDataException() noexcept {};
		virtual ~NoImageDataException() noexcept = default;
		virtual const char* what() const noexcept { return "NoImageDataException"; }
	};

	/**
	 * @brief Can be thrown during construction of a Texture2D.
	 */
	class UnknownImageFormatException : public std::exception
	{
	public:
		inline UnknownImageFormatException() noexcept {};
		virtual ~UnknownImageFormatException() noexcept = default;
		virtual const char* what() const noexcept { return "UnknownImageFormatException"; }
	};

	template <class ...Args>
	inline static SharedPtr create(Args&&... args)
	{
		return SharedPtr(new Texture2D(std::forward<Args>(args)...));
	}

private:

	/**
	 * @brief Construct from an FbxTexture.
	 * @param pFbxTexture Pointer to an FbxTexture.
	 * @throws NoImageDataException if the image contains no data.
	 * @throws UnknownImageFormatException if the format is not one of grey, grey+alpha, RGB or RGBA.
	 */
	Texture2D(
		const FBXSDK_NAMESPACE::FbxTexture* pFbxTexture);

	/**
	 * @brief Construct from a filepath.
	 * @param pathToImageFile Path to an image.
	 * @throws NoImageDataException if the image contains no data.
	 * @throws UnknownImageFormatException if the format is not one of grey, grey+alpha, RGB or RGBA.
	 */
	Texture2D(const char* pathToImageFile);

	/**
	 * @brief Construct from a filepath.
	 * @param pathToImageFile Path to an image.
	 * @throws NoImageDataException if the image contains no data.
	 * @throws UnknownImageFormatException if the format is not one of grey, grey+alpha, RGB or RGBA.
	 */
	Texture2D(const std::string& pathToImageFile);

	/**
	 * @brief Construct from a filepath.
	 * @param pathToImageFile Path to an image.
	 * @throws NoImageDataException if the image contains no data.
	 * @throws UnknownImageFormatException if the format is not one of grey, grey+alpha, RGB or RGBA.
	 */
	Texture2D(boost::filesystem::path pathToImageFile);

public:

	/// Defaulted destructor.
	virtual ~Texture2D() noexcept = default;

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

	void loadFromFile(boost::filesystem::path);

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& archive, const unsigned int /*version*/) const
	{
		archive & boost::serialization::base_object<Object<Texture2D, boost::filesystem::path>>(*this);
	}

	template <class Archive>
	void load(Archive& archive, const unsigned int /*version*/)
	{
		archive & boost::serialization::base_object<Object<Texture2D, boost::filesystem::path>>(*this);
		loadFromFile(name);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Texture2D, boost::serialization::track_always);