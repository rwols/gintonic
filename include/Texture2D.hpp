#pragma once

#include "Graphics/OpenGL/TextureObject.hpp"
#include "Image.hpp"
#include "gintonic_export.h"

namespace gintonic {
namespace experimental {

class GINTONIC_EXPORT Texture2D : public experimental::Asset<Texture2D>,
                                  public OpenGL::TextureObject
{
  public:
    Texture2D() = default;
    Texture2D(std::string name);
    Texture2D(std::string name, IntrusivePtr<const Image> image,
              const GLint mipmaps = 0);

    static const char* extension() noexcept { return ".texture"; }
    static const char* prefixFolder() noexcept { return "textures"; }

    IntrusivePtr<const Image> getImage() const noexcept { return mImage; }
    void                      setImage(IntrusivePtr<const Image> image);

    const GLint getMipMaps() const noexcept { return mMipMaps; }

  private:
    IntrusivePtr<const Image> mImage;
    GLint                     mMipMaps = 0;

    void init();

    template <class Archive>
    void save(Archive& archive, const unsigned /*version*/) const
    {
        using boost::serialization::base_object;
        using boost::serialization::make_nvp;
        archive << make_nvp("super",
                            base_object<experimental::Asset<Texture2D>>(*this));
        archive << make_nvp("image", mImage->name);
        archive << make_nvp("mipmaps", mMipMaps);
    }

    template <class Archive>
    void load(Archive& archive, const unsigned /*version*/)
    {
        using boost::serialization::base_object;
        using boost::serialization::make_nvp;
        std::string imageName;
        archive >> make_nvp("super",
                            base_object<experimental::Asset<Texture2D>>(*this));
        archive >> make_nvp("image", imageName);
        mImage = Image::request<Archive>(std::move(imageName));
        archive >> make_nvp("mipmaps", mMipMaps);
        init();
    }

    friend class boost::serialization::access;

    BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace experimental
} // namespace gintonic
