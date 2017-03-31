/**
 * @file Texture2D.hpp
 * @author Raoul Wols
 * @brief Defines the Texture2D class.
 */

#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/split_member.hpp>
#include <gintonic/Asset.hpp>
#include <gintonic/Foundation/FilePath.hpp>
#include <gintonic/Foundation/Object.hpp>
#include <gintonic/Graphics/OpenGL/TextureObject.hpp>

#include <gintonic/nsbegin.hpp>

/**
 * @brief Simple two-dimensional texture. You can bind it to a texture unit.
 */
class Texture2D : public Asset
{
    GINTONIC_ASSET_BOILERPLATE(Texture2D, "textures", ".tex");
    GINTONIC_ASSET_BOILERPLATE_TRIVIAL_CONSTRUCTORS(Texture2D);

  public:
    /**
     * @brief Can be thrown during construction of a Texture2D.
     */
    class NoImageData : public std::invalid_argument
    {
      public:
        inline NoImageData(boost::filesystem::path errorPath) noexcept
            : std::invalid_argument("NoImagedata"), mPath(std::move(errorPath))
        {
        }
        ~NoImageData() noexcept override = default;
        inline const boost::filesystem::path& getPath() noexcept
        {
            return mPath;
        }

      private:
        boost::filesystem::path mPath;
    };

    /**
     * @brief Can be thrown during construction of a Texture2D.
     */
    class UnknownImageFormat : public std::invalid_argument
    {
      public:
        inline UnknownImageFormat(const int format) noexcept
            : std::invalid_argument("UnknownImageFormat"), mFormat(format)
        {
        }
        ~UnknownImageFormat() noexcept override = default;
        inline int getFormat() const noexcept { return mFormat; }

      private:
        int mFormat;
    };

    // template <class ...Args>
    // inline static SharedPtr create(Args&&... args)
    // {
    //  return SharedPtr(new Texture2D(std::forward<Args>(args)...));
    // }

    struct ImageLoadOptions
    {
        std::string relativeFilename;
    };

    static std::shared_ptr<Texture2D>
    fromImage(const ImageLoadOptions& options);

    static std::shared_ptr<Texture2D>
    fromFbxTexture(const FBXSDK_NAMESPACE::FbxTexture* texture);

  private:
    // /**
    //  * @brief Construct from an FbxTexture.
    //  * @param pFbxTexture Pointer to an FbxTexture.
    //  * @throws NoImageDataException if the image contains no data.
    //  * @throws UnknownImageFormatException if the format is not one of grey,
    //  grey+alpha, RGB or RGBA.
    //  */
    // Texture2D(
    //  const FBXSDK_NAMESPACE::FbxTexture* pFbxTexture);

    // /**
    //  * @brief Construct from a filepath.
    //  * @param pathToImageFile Path to an image.
    //  * @throws NoImageDataException if the image contains no data.
    //  * @throws UnknownImageFormatException if the format is not one of grey,
    //  grey+alpha, RGB or RGBA.
    //  */
    // Texture2D(const char* pathToImageFile);

    // /**
    //  * @brief Construct from a filepath.
    //  * @param pathToImageFile Path to an image.
    //  * @throws NoImageDataException if the image contains no data.
    //  * @throws UnknownImageFormatException if the format is not one of grey,
    //  grey+alpha, RGB or RGBA.
    //  */
    // Texture2D(const std::string& pathToImageFile);

    // /**
    //  * @brief Construct from a filepath.
    //  * @param pathToImageFile Path to an image.
    //  * @throws NoImageDataException if the image contains no data.
    //  * @throws UnknownImageFormatException if the format is not one of grey,
    //  grey+alpha, RGB or RGBA.
    //  */
    // Texture2D(boost::filesystem::path pathToImageFile);

  public:
    /// Defaulted destructor.
    ~Texture2D() noexcept override = default;

    /**
     * @brief Bind this texture to the given OpenGL texture unit.
     * @param textureUnit The texture unit.
     * @attention Counting starts from 0, not from GL_TEXTURE0. This
     * unifies sampler2D and texture units.
     */
    void bind(const GLint textureUnit) const noexcept;

  private:
    OpenGL::TextureObject mTextureObject;
    std::string imageFile;

    void loadFromFile(boost::filesystem::path);

    template <class Archive>
    void save(Archive& archive, const unsigned int /*version*/) const
    {
        archive << BOOST_SERIALIZATION_BASE_OBJECT_NVP(Asset)
                << boost::serialization::make_nvp("image_path", imageFile);
    }

    template <class Archive>
    void load(Archive& archive, const unsigned int /*version*/)
    {
        using namespace boost::serialization;
        using namespace boost::filesystem;
        archive >> BOOST_SERIALIZATION_BASE_OBJECT_NVP(Asset) >>
            make_nvp("image_path", imageFile);
        loadFromFile(path(Asset::getAssetFolder()) / path(imageFile));
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
};

#include <gintonic/nsend.hpp>

BOOST_CLASS_TRACKING(gintonic::Texture2D, boost::serialization::track_always);
