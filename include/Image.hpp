#pragma once

#include "Asset.hpp"
// #include "Graphics/OpenGL/stb_image.h"
#include <memory>

namespace gintonic {

namespace experimental {

class Image : public Asset<Image>
{
  public:
    Image() = default;
    Image(std::string relpath);

    void* getPixels() const noexcept { return mPixels.get(); }

    unsigned int getHeight() const noexcept { return mHeight; }

    unsigned int getWidth() const noexcept { return mWidth; }

    unsigned int getSize() const noexcept { return mSize; }

    unsigned int getComponents() const noexcept { return mComponents; }

    static const char* extension() noexcept { return ".image"; }

    static const char* prefixFolder() noexcept { return "images"; }

    template <class Archive>
    void save(Archive& archive, const unsigned /*version*/)
    {
        /* do nothing */
    }

    template <class Archive>
    void load(Archive& archive, const unsigned /*version*/)
    {
        init();
    }

  private:
    void init();
    void copyPixelsIfRequired(const Image& other);

    struct PixelFree
    {
        void operator()(void* p) { free(p); }
    };

    std::unique_ptr<unsigned char, PixelFree> mPixels;
    int mSize = 0;
    int mHeight = 0;
    int mWidth = 0;
    int mComponents = 0;
};

} // experimental

} // gintonic
