#include "Graphics/Texture2D.hpp"
#include "Foundation/exception.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/OpenGL/stb_image.h"
#include <fbxsdk.h>

namespace gintonic {

Texture2D::Texture2D(const FbxTexture* pFbxTexture)
{
	const auto pFbxFileTexture = FbxCast<FbxFileTexture>(pFbxTexture);
	if (!pFbxFileTexture)
	{
		exception lException(pFbxTexture->GetName());
		lException.append(" is not a file texture.");
		throw lException;
	}
	try
	{
		loadFromFile(pFbxFileTexture->GetFileName());	
	}
	catch (const std::exception& /*lException*/)
	{
		// Failed! We try it one more time, but this time
		// we take the parent path of the FbxScene's file, and
		// then append the filename. If this doesn't work,
		// we go home.

		// Sometimes this actually works.
		
		auto lFilename = boost::filesystem::path(
			pFbxTexture->GetScene()->GetSceneInfo()->Url.Get().Buffer()).parent_path() 
			/ pFbxFileTexture->GetFileName();

		loadFromFile(std::move(lFilename));
	}

}

Texture2D::Texture2D(const char* pathToImageFile) : Texture2D(boost::filesystem::path(pathToImageFile)) {}

Texture2D::Texture2D(const std::string& pathToImageFile) : Texture2D(boost::filesystem::path(pathToImageFile)) {}

Texture2D::Texture2D(boost::filesystem::path pathToImageFile)
{
	loadFromFile(std::move(pathToImageFile));
}

void Texture2D::bind(const GLint textureUnit) const noexcept
{
	mTextureObject.bind(GL_TEXTURE_2D, textureUnit);
}

void Texture2D::loadFromFile(boost::filesystem::path filename)
{
	this->name = std::move(filename);
	std::cerr << "Loading file: " << this->name << '\n';
	GLenum lFormat;
	int lWidth;
	int lHeight;
	int lComp;

	#ifdef BOOST_MSVC
	const auto lString = this->name.string();
	std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data(
		stbi_load(lString.c_str(), &lWidth, &lHeight, &lComp, STBI_default), 
		&stbi_image_free);
	#else
	std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data(
		stbi_load(this->name.c_str(), &lWidth, &lHeight, &lComp, STBI_default), 
		&stbi_image_free);
	#endif
	
	if (!data) throw NoImageDataException();

	switch (lComp)
	{
		case STBI_grey:       lFormat = GL_RED;  break;
		case STBI_grey_alpha: lFormat = GL_RG;   break;
		case STBI_rgb:        lFormat = GL_RGB;  break;
		case STBI_rgb_alpha:  lFormat = GL_RGBA; break;
		default: throw UnknownImageFormatException();
	}
	
	glBindTexture(GL_TEXTURE_2D, mTextureObject);

	glTexImage2D(GL_TEXTURE_2D, 0, lFormat, 
		static_cast<GLsizei>(lWidth), static_cast<GLsizei>(lHeight), 0, 
		lFormat, GL_UNSIGNED_BYTE, (const GLvoid*)data.get());	

	glGenerateMipmap(GL_TEXTURE_2D);  // Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

} // namespace gintonic
