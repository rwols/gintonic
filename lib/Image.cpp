#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
// #define STBI_NO_STDIO
#include "Graphics/OpenGL/stb_image.h"

namespace gintonic {
namespace experimental {

Image::Image(std::string relpath) : Asset<Image>(std::move(relpath)) { init(); }

void Image::init()
{
    auto path = getFilePath();
    path = path.substr(0, path.size() - std::strlen(Image::extension()));
    mPixels.reset(
        stbi_load(path.c_str(), &mWidth, &mHeight, &mComponents, STBI_default));
    if (!mPixels) throw std::runtime_error("unable to load this image");
}

void Image::copyPixelsIfRequired(const Image& other)
{
    if (other.mPixels)
    {
        mPixels.reset((unsigned char*)malloc(sizeof(unsigned char) * mSize));
        std::memcpy(mPixels.get(), other.mPixels.get(), mSize);
    }
}

} // experimental
} // gintonic
