#include "Asset.hpp"
#include <gintonic/Foundation/filesystem.hpp>

using namespace gintonic;

std::string Asset::sAssetFolder = (getExecutablePath() / "assets").string();

Asset::Asset(const std::string& relativeFilename) 
: mName(relativeFilename)
{
	GT_PROFILE_FUNCTION;
}

Asset::Asset(std::string&& relativeFilename)
: mName(std::move(relativeFilename))
{
	GT_PROFILE_FUNCTION;
}

Asset::Asset(const char* relativeFilename)
: mName(relativeFilename)
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
	const auto fullPath = path(getAssetFolder()) / path(std::move(relativeFilename));
	return fullPath.string();
}

std::string Asset::getFullPath() const
{
	return getFullPathFor(mName);
}

std::ifstream Asset::openForReading(const std::ios_base::openmode mode) const
{
	return std::ifstream(getFullPath(), mode);
}

std::ofstream Asset::openForWriting(const std::ios_base::openmode mode) const
{
	return std::ofstream(getFullPath(), mode);
}