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

namespace FBX
{
	class FbxTexture; // Forward declaration.
}

namespace gintonic {

/**
 * @brief Simple two-dimensional texture. You can bind it to a texture unit.
 */
class Texture2D : public Object<Texture2D, boost::filesystem::path>
{
public:

	/**
	 * @brief Construct from an FbxTexture.
	 * @param pFbxTexture Pointer to an FbxTexture.
	 * @exception gintonic::exception
	 */
	Texture2D(
		const FBX::FbxTexture* pFbxTexture);

	/**
	 * @brief Construct from a filepath.
	 * @param pathToImageFile Path to an image.
	 * @exception gintonic::exception
	 */
	Texture2D(const char* pathToImageFile);

	/**
	 * @brief Construct from a filepath.
	 * @param pathToImageFile Path to an image.
	 * @exception gintonic::exception
	 */
	Texture2D(const std::string& pathToImageFile);

	/**
	 * @brief Construct from a filepath.
	 * @param pathToImageFile Path to an image.
	 * @exception gintonic::exception
	 */
	Texture2D(boost::filesystem::path pathToImageFile);

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

	OpenGL::TextureObject mTextureObject;

	void loadFromFile(boost::filesystem::path);

};

} // namespace gintonic