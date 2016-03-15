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

class Texture2D : public Object<Texture2D, boost::filesystem::path>
{
public:

	Texture2D(
		const FBX::FbxTexture* pFbxTexture);

	Texture2D(const char* pathToImageFile);

	Texture2D(const std::string& pathToImageFile);

	Texture2D(boost::filesystem::path pathToImageFile);

	virtual ~Texture2D() noexcept = default;

	void bind(const GLint textureUnit) const noexcept;

private:

	OpenGL::TextureObject mTextureObject;

	void loadFromFile(boost::filesystem::path);

};

} // namespace gintonic