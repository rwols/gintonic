#include "Texture2D.hpp"
#include "Graphics/OpenGL/stb_image.h"

namespace gintonic {
namespace experimental {

Texture2D::Texture2D(std::string name)
    : experimental::Asset<Texture2D>(std::move(name))
{
}

Texture2D::Texture2D(std::string name, IntrusivePtr<const Image> image,
                     const GLint mipmaps)
    : experimental::Asset<Texture2D>(std::move(name)), mImage(std::move(image)),
      mMipMaps(mipmaps)
{
    init();
}

void Texture2D::init()
{
    if (!mImage)
    {
        return;
    }
    GLenum format;
    switch (mImage->getComponents())
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
        throw std::runtime_error("unknown image format");
    }

    glBindTexture(GL_TEXTURE_2D, *this);

    glTexImage2D(GL_TEXTURE_2D, mMipMaps, format,
                 static_cast<GLsizei>(mImage->getWidth()),
                 static_cast<GLsizei>(mImage->getHeight()), 0, format,
                 GL_UNSIGNED_BYTE, (const GLvoid*)mImage->getPixels());

    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps now!!!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
}

void Texture2D::setImage(
    IntrusivePtr<const gintonic::experimental::Image> image)
{
    mImage = std::move(image);
    init();
}

} // experimental
} // gintonic
