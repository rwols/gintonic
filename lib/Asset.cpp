#include "Asset.hpp"
#include "Foundation/filesystem.hpp"
#include <eos/portable_oarchive.hpp>

using namespace gintonic;

std::string Asset::sAssetFolder = std::string();

std::unordered_map<std::string, std::weak_ptr<Asset>> sAssetMap =
    std::unordered_map<std::string, std::weak_ptr<Asset>>();

Asset::Asset(const std::string& relativeFilename) : mName(relativeFilename)
{
    GT_PROFILE_FUNCTION;
}

Asset::Asset(std::string&& relativeFilename)
    : mName(std::move(relativeFilename))
{
    GT_PROFILE_FUNCTION;
}

Asset::Asset(const char* relativeFilename) : mName(relativeFilename)
{
    GT_PROFILE_FUNCTION;
}

std::string Asset::getFullPathFor(const char* relativeFilename)
{
    using boost::filesystem::path;
    const auto fullPath = path(getAssetFolder()) / path(relativeFilename);
    return fullPath.string();
}

std::string Asset::getFullPathFor(const std::string& relativeFilename)
{
    return getFullPathFor(relativeFilename.c_str());
}

std::string Asset::getFullPathFor(std::string&& relativeFilename)
{
    using boost::filesystem::path;
    const auto fullPath =
        path(getAssetFolder()) / path(std::move(relativeFilename));
    return fullPath.string();
}

std::string Asset::getFullPath() const { return getFullPathFor(mName); }

std::ifstream Asset::openForReading() const
{
    return std::ifstream(getFullPath(), std::ios::binary);
}

std::ofstream Asset::openForWriting() const
{
    return std::ofstream(getFullPath(), std::ios::binary);
}
