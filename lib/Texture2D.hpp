#pragma once

#include "opengl/texture_object.hpp"
#include "filesystem.hpp"
#include "config.hpp"
#include <memory>

namespace FBX
{
	class FbxTexture; // Forward declaration.
}

namespace gintonic {

class Texture2D : public std::enable_shared_from_this<Texture2D>
{
public:

	Texture2D(
		const FBX::FbxTexture* pFbxTexture);

	Texture2D(const char* pathToImageFile);

	Texture2D(const std::string& pathToImageFile);

	Texture2D(boost::filesystem::path pathToImageFile);

	virtual ~Texture2D() noexcept = default;

	void bind(const GLint textureUnit) const noexcept;

	inline const boost::filesystem::path& getFilename() const noexcept
	{
		return mFilename;
	}

private:

	opengl::texture_object mTextureObject;
	boost::filesystem::path mFilename;

	void loadFromFile(boost::filesystem::path);

};

} // namespace gintonic