#include <gintonic/Graphics/Texture2D.hpp>

#include <gintonic/Foundation/exception.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <gintonic/Graphics/OpenGL/stb_image.h>

#include <fbxsdk.h>

using namespace gintonic;

void Texture2D::construct() { loadFromFile(getFullPath()); }

std::shared_ptr<Texture2D> Texture2D::fromImage(const ImageLoadOptions& options)
{
    using namespace boost::filesystem;
    path p(Asset::getAssetFolder() / path(options.relativeFilename));
    auto tex = std::make_shared<Texture2D>(p.stem().string());
    if (tex)
    {
        tex->loadFromFile(p);
    }
    return tex;
}

std::shared_ptr<Texture2D>
Texture2D::fromFbxTexture(const FbxTexture* pFbxTexture)
{
    using namespace boost::filesystem;
    const auto pFbxFileTexture = FbxCast<FbxFileTexture>(pFbxTexture);
    if (!pFbxFileTexture)
    {
        exception lException(pFbxTexture->GetName());
        lException.append(" is not a file texture.");
        throw lException;
    }
    path p(pFbxFileTexture->GetFileName());
    auto tex = std::make_shared<Texture2D>(p.stem().string());
    if (!tex) return nullptr;
    try
    {
        tex->loadFromFile(p);
    }
    catch (const std::exception& /*lException*/)
    {
        // Failed! We try it one more time, but this time
        // we take the parent path of the FbxScene's file, and
        // then append the filename. If this doesn't work,
        // we go home.

        // Sometimes this actually works.
        auto lFilename =
            boost::filesystem::path(
                pFbxTexture->GetScene()->GetSceneInfo()->Url.Get().Buffer())
                .parent_path() /
            pFbxFileTexture->GetFileName();

        tex->setName(lFilename.stem().string());
        tex->loadFromFile(std::move(lFilename));
    }
    return tex;
}

// Texture2D::Texture2D(const char* pathToImageFile) :
// Texture2D(boost::filesystem::path(pathToImageFile)) {}

// Texture2D::Texture2D(const std::string& pathToImageFile) :
// Texture2D(boost::filesystem::path(pathToImageFile)) {}

// Texture2D::Texture2D(boost::filesystem::path pathToImageFile)
// {
// 	loadFromFile(std::move(pathToImageFile));
// }

void Texture2D::bind(const GLint textureUnit) const noexcept
{
    mTextureObject.bind(GL_TEXTURE_2D, textureUnit);
}

void Texture2D::loadFromFile(boost::filesystem::path filename)
{
    // std::cerr << "Loading file: " << this->name << '\n';
    GLenum format;
    int width;
    int height;
    int comp;

#ifdef BOOST_MSVC
    const auto lString = filename.string();
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data(
        stbi_load(lString.c_str(), &width, &height, &comp, STBI_default),
        &stbi_image_free);
#else
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data(
        stbi_load(filename.c_str(), &width, &height, &comp, STBI_default),
        &stbi_image_free);
#endif

    if (!data) throw NoImageDataException(filename);

    switch (comp)
    {
    case STBI_grey:
        format = GL_RED;
        break;
    case STBI_grey_alpha:
        format = GL_RG;
        break;
    case STBI_rgb:
        format = GL_RGB;
        break;
    case STBI_rgb_alpha:
        format = GL_RGBA;
        break;
    default:
        throw UnknownImageFormatException(comp);
    }

    glBindTexture(GL_TEXTURE_2D, mTextureObject);

    glTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE,
                 (const GLvoid*)data.get());

    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps now!!!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
}
