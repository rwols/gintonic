#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <fstream>
#include <gintonic/Foundation/Profiler.hpp>
#include <memory>
#include <string>

#include <gintonic/nsbegin.hpp>

/**
 * @brief      Base class for all asset types.
 *
 * @detail     Assets represent "heavy" files on disk. The idea is that assets
 *             are shared among various objects via `std::shared_ptr`, so that
 *             no two objects that represent the same object are allocated
 *             twice. The \ref `name()` of an \ref `Asset` works as follows.
 *             Every class derived from Asset *must* supply the following static
 *             methods: `extension()` and `prefixFolder()`, both returning a
 *             string(-like) object.
 */
class Asset
{
  public:
    /**
     * @brief      The name of this Asset.
     *
     * @return     The name of this Asset.
     */
    inline const std::string& shortName() const noexcept { return mName; }

    template <class StringType>
    inline void setName(StringType&& relativeFilename)
    {
        mName = std::forward<StringType>(relativeFilename);
    }

    /**
     * @brief      Sets the global absolute asset folder.
     *
     * @param[in]  folder      The absolute path to the asset folder.
     *
     * @tparam     StringType  A string-like type that is convertible to
     *                         std::string.
     */
    template <class StringType>
    inline static void setAssetFolder(StringType&& folder)
    {
        sAssetFolder = std::forward<StringType>(folder);
    }

    /**
     * @brief      Gets the asset folder.
     *
     * @return     The asset folder.
     */
    inline static const std::string& getAssetFolder() noexcept
    {
        return sAssetFolder;
    }

    /**
     * @brief      The absolute path for this asset.
     *
     * @return     The absolute path for this asset.
     */
    virtual std::string absolutePath() const noexcept = 0;

    static std::string getFullPathFor(const char* relativeFilename);
    static std::string getFullPathFor(const std::string& relativeFilename);
    static std::string getFullPathFor(std::string&& relativeFilename);

    virtual ~Asset() noexcept = default;

  protected:
    template <class DerivedType> void saveToDiskInternal() const;

    Asset(const std::string& relativeFilename);
    Asset(std::string&& relativeFilename);
    Asset(const char* relativeFilename);

    std::string getFullPath() const;

    std::ifstream
    openForReading(const std::ios_base::openmode mode = std::ios::binary) const;

    std::ofstream
    openForWriting(const std::ios_base::openmode mode = std::ios::binary) const;

  private:
    std::string mName;
    static std::string sAssetFolder;

    friend class boost::serialization::access;

    template <class Archive>
    inline void serialize(Archive& /*archive*/, const unsigned /*version*/)
    {
        /* Does nothing. */
    }
};

#include <gintonic/nsend.hpp>

/**
 * @brief      Defines the necessary methods for a class derived from Asset.
 *
 * @param      class_name            The class name
 * @param      prefix_folder_string  The prefix folder string
 * @param      extension_string      The extension string
 */
#define GINTONIC_ASSET_BOILERPLATE(class_name, prefix_folder_string,           \
                                   extension_string)                           \
  public:                                                                      \
    inline static const char* extension() { return extension_string; }         \
    inline static const char* prefixFolder() { return prefix_folder_string; }  \
    std::string absolutePath() const noexcept override                         \
    {                                                                          \
        return Asset::getAssetFolder() + "/" +                                 \
               std::string(class_name::prefixFolder()) + "/" + shortName() +   \
               class_name::extension();                                        \
    }                                                                          \
                                                                               \
  private:                                                                     \
    friend class boost::serialization::access;                                 \
    template <class Archive>                                                   \
    inline friend void load_construct_data(                                    \
        Archive& archive, class_name* asset, const unsigned version)           \
    {                                                                          \
        std::string name;                                                      \
        archive >> BOOST_SERIALIZATION_NVP(name);                              \
        new (asset) class_name(std::move(name));                               \
    }                                                                          \
    template <class Archive>                                                   \
    inline friend void save_construct_data(                                    \
        Archive& archive, const class_name* asset, const unsigned version)     \
    {                                                                          \
        const auto& name = asset->shortName();                                 \
        archive << BOOST_SERIALIZATION_NVP(name);                              \
    }

#define GINTONIC_ASSET_BOILERPLATE_TRIVIAL_CONSTRUCTORS(class_name)            \
  protected:                                                                   \
    inline class_name(const char* name) : Asset(name) {}                       \
    inline class_name(const std::string& name) : Asset(name) {}                \
    inline class_name(std::string&& name) : Asset(std::move(name)) {}
