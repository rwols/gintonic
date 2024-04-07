#pragma once

#include "Foundation/Profiler.hpp"
#include <eos/portable_iarchive.hpp>
#include <eos/portable_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

namespace gintonic
{

namespace experimental
{

template <class TDerived> class Asset
{
  public:
    Asset(const Asset&) = delete;
    Asset(Asset&&) = delete;
    Asset& operator=(const Asset&) = delete;
    Asset& operator=(Asset&&) = delete;

    std::string name;

    static std::string baseFolder;

    template <class Archive, class S>
    static std::shared_ptr<const TDerived> request(S&& name);

    static std::size_t removeExpiredWeakPointers();

    template <class Archive> void saveToPersistentMedia() const;

    template <class Archive, class S>
    static std::shared_ptr<const TDerived> loadFromPersistentMedia(S&& name);

    static const char* extension() noexcept;

    static const char* prefixFolder() noexcept;

  protected:
    template <class S> Asset(S&& name);

  private:
    std::string getFilePath();
    template <class S> static std::string getFilePath(S&& name);
    static std::unordered_map<std::string, std::weak_ptr<const TDerived>>
        sCache;

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive&, const unsigned);
};

template <class T>
template <class S>
Asset<T>::Asset(S&& name) : name(std::forward<S>(name))
{
}

template <class T> std::string Asset<T>::getFilePath()
{
    return baseFolder + "/" + T::prefixFolder() + "/" + name + T::extension();
}

template <class T>
template <class S>
std::string Asset<T>::getFilePath(S&& name)
{
    return baseFolder + "/" + T::prefixFolder() + "/" + std::forward<S>(name) +
           T::extension();
}

template <class T>
template <class Archive, class S>
std::shared_ptr<const T> Asset<T>::request(S&& name)
{
    auto iter = sCache.find(name);
    if (iter != sCache.end())
    {
        if (auto ptr = iter->second.lock()) return ptr;
        auto ptr = loadFromPersistentMedia<Archive>(std::forward<S>(name));
        iter->second = ptr;
        return ptr;
    }
    auto ptr = loadFromPersistentMedia<Archive>(name);
    sCache.emplace(std::forward<S>(name), ptr);
    return ptr;
}

template <class T> std::size_t Asset<T>::removeExpiredWeakPointers()
{
    std::size_t count = 0;
    auto iter = sCache.begin();
    while (iter != sCache.end())
    {
        if (iter->second.expired())
        {
            iter = sCache.erase(iter);
            ++count;
        }
        else
        {
            ++iter;
        }
    }
    return count;
}

template <class T>
template <class Archive>
void Asset<T>::saveToPersistentMedia() const
{
    std::ofstream output(getFilePath());
    Archive oa(output);
    oa << static_cast<const T&>(*this);
}

template <class T>
template <class Archive, class S>
std::shared_ptr<const T> Asset<T>::loadFromPersistentMedia(S&& name)
{
    std::ifstream input(getFilePath(name));
    Archive ia(input);
    std::shared_ptr<T> ptr(new T(std::forward<S>(name)));
    ia >> *ptr;
    return ptr;
}

template <class T> const char* Asset<T>::extension() noexcept
{
    return ".unknown-asset";
}

template <class T> const char* Asset<T>::prefixFolder() noexcept
{
    return "unknown";
}

template <class T>
std::unordered_map<std::string, std::weak_ptr<const T>> Asset<T>::sCache =
    std::unordered_map<std::string, std::weak_ptr<const T>>();

template <class T>
template <class Archive>
void Asset<T>::serialize(Archive& /*archive*/, const unsigned /*version*/)
{
    /* empty */
}

} // experimental

/**
 * @brief      Base class for all asset types.
 *
 * @detail     Assets represent "heavy" files on disk. The idea is that assets
 *             are shared among various objects via `std::shared_ptr`, so that
 *             no two objects that represent the same object are allocated
 *             twice. The \ref `shortName()` of an \ref `Asset` works as
 *             follows. Every class derived from Asset *must* supply the
 *             following static methods: `extension()` and `prefixFolder()`,
 *             both returning a string(-like) object.
 */
class Asset
{
  public:
    Asset(const Asset&) = delete;
    Asset(Asset&&) = delete;
    Asset& operator=(const Asset&) = delete;
    Asset& operator=(Asset&&) = delete;

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
     * @brief      Request a new Asset of type DerivedType with the given name.
     *             If no such asset exists, nullptr is returned.
     *
     * @param[in]  name         The name of the asset, not including the prefix
     *                          folder nor including the extension. That is all
     *                          handled implicitly.
     *
     * @tparam     DerivedType  The type of the Asset (e.g. Material)
     * @tparam     StringType   A string-like type that is convertible to
     *                          std::string. Automatically deduced.
     *
     * @return     A shared pointer to the asset, or nullptr if something went
     *             wrong.
     */
    template <class DerivedType, class StringType>
    static std::shared_ptr<DerivedType> request(StringType&& name);

    /**
     * @brief      The absolute path for this asset.
     *
     * @return     The absolute path for this asset.
     */
    virtual std::string absolutePath() const noexcept = 0;

    static std::string getFullPathFor(const char* relativeFilename);
    static std::string getFullPathFor(const std::string& relativeFilename);
    static std::string getFullPathFor(std::string&& relativeFilename);

    virtual void saveToDisk() const = 0;

    virtual ~Asset() noexcept = default;

  protected:
    template <class DerivedType> void saveToDiskInternal() const;

    Asset(const std::string& relativeFilename);
    Asset(std::string&& relativeFilename);
    Asset(const char* relativeFilename);
    Asset() = default;

    std::string getFullPath() const;
    std::ifstream openForReading() const;
    std::ofstream openForWriting() const;

  private:
    static std::unordered_map<std::string, std::weak_ptr<Asset>> sAssetMap;
    std::string mName;
    static std::string sAssetFolder;

    template <class DerivedType, class StringType>
    static std::shared_ptr<DerivedType>
    createNew(StringType&& relativeFilename);

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned /*version*/);
};

template <class DerivedType, class StringType>
std::shared_ptr<DerivedType> Asset::createNew(StringType&& relativeFilename)
{
    GT_PROFILE_FUNCTION;
    std::shared_ptr<DerivedType> newAsset(
        new DerivedType(std::forward<StringType>(relativeFilename)));
    if (!newAsset) return nullptr;
    auto input = newAsset->openForReading();
    if (!input) return nullptr;
    eos::portable_iarchive archive(input, std::ios::binary);
    archive >> *newAsset;
    sAssetMap.emplace(relativeFilename, newAsset);
    return newAsset;
}

template <class DerivedType> void Asset::saveToDiskInternal() const
{
    GT_PROFILE_FUNCTION;
    auto output = openForWriting();
    if (!output) return; // FIXME: throw some helpful exception
    eos::portable_oarchive archive(output);
    archive << static_cast<const DerivedType&>(*this);
}

template <class DerivedType, class StringType>
std::shared_ptr<DerivedType> Asset::request(StringType&& relativeFilename)
{
    GT_PROFILE_FUNCTION;
    auto findResult = sAssetMap.find(relativeFilename);
    if (findResult == sAssetMap.end())
    {
        return createNew<DerivedType>(
            std::forward<StringType>(relativeFilename));
    }
    else
    {
        if (auto ptr = findResult->second.lock())
        {
            return std::static_pointer_cast<DerivedType>(ptr);
        }
        else
        {
            return createNew<DerivedType>(
                std::forward<StringType>(relativeFilename));
        }
    }
}

template <class Archive>
void Asset::serialize(Archive& ar, const unsigned /*version*/)
{
    GT_PROFILE_FUNCTION;
    ar& mName;
}

} // namespace gintonic

/**
 * @brief      Defines the necessary methods for a class derived from Asset.
 *
 * @param      class_name            The class name
 * @param      prefix_folder_string  The prefix folder string
 * @param      extension_string      The extension string
 */
#define GINTONIC_ASSET(class_name, prefix_folder_string, extension_string)     \
  public:                                                                      \
    static const char* extension() { return extension_string; }                \
    static const char* prefixFolder() { return prefix_folder_string; }         \
    std::string absolutePath() const noexcept override                         \
    {                                                                          \
        return Asset::getAssetFolder() + "/" + std::string(prefixFolder()) +   \
               "/" + shortName() + extension();                                \
    }                                                                          \
    void saveToDisk() const override { saveToDiskInternal<class_name>(); }     \
    class_name(const char* name) : Asset(name) {}                              \
    class_name(const std::string& name) : Asset(name) {}                       \
    class_name(std::string&& name) : Asset(std::move(name)) {}                 \
                                                                               \
  private:
