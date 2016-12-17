#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include "Foundation/Profiler.hpp"
#include "Foundation/portable_iarchive.hpp"
#include "Foundation/portable_oarchive.hpp"

namespace gintonic {

class Asset {
public:

	Asset(const Asset&)              = delete;
	Asset(Asset&&)                   = delete;
	Asset& operator = (const Asset&) = delete;
	Asset& operator = (Asset&&)      = delete;

	inline const std::string& getRelativeFilename() const noexcept
	{
		return mName;
	}

	template <class StringType>
	inline static void setAssetFolder(StringType&& folder)
	{
		sAssetFolder = std::forward<StringType>(folder);
	}

	inline static const std::string& getAssetFolder() noexcept
	{
		return sAssetFolder;
	}

	template <class DerivedType, class StringType>
	static std::shared_ptr<DerivedType> request(StringType&& relativeFilename);

	static std::string getFullPathFor(const char* relativeFilename);
	static std::string getFullPathFor(const std::string& relativeFilename);
	static std::string getFullPathFor(std::string&& relativeFilename);

	virtual void saveToDisk() const = 0;

	virtual ~Asset() noexcept = default;

protected:

	template <class DerivedType> 
	void saveToDiskInternal() const;

	Asset(const std::string& relativeFilename);
	Asset(std::string&& relativeFilename);
	Asset(const char* relativeFilename);

	template <class StringType>
	inline void setRelativeFilename(StringType&& relativeFilename)
	{
		mName = std::forward<StringType>(relativeFilename);
	}

	std::string getFullPath() const;
	std::ifstream openForReading() const;
	std::ofstream openForWriting() const;

private:
	static std::unordered_map<std::string, std::weak_ptr<Asset>> sAssetMap;
	std::string mName;
	static std::string sAssetFolder;

	template <class DerivedType, class StringType>
	static std::shared_ptr<DerivedType> createNew(StringType&& relativeFilename);

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/);
};


template <class DerivedType, class StringType>
std::shared_ptr<DerivedType> Asset::createNew(StringType&& relativeFilename)
{
	GT_PROFILE_FUNCTION;
	std::shared_ptr<DerivedType> newAsset(new DerivedType(std::forward<StringType>(relativeFilename)));
	if (!newAsset) return nullptr;
	auto input = newAsset->openForReading();
	if (!input) return nullptr;
	eos::portable_iarchive archive(input, std::ios::binary);
	archive >> *newAsset;
	sAssetMap.emplace(relativeFilename, newAsset);
	return newAsset;
}

template <class DerivedType> 
void Asset::saveToDiskInternal() const
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
		return createNew<DerivedType>(std::forward<StringType>(relativeFilename));
	}
	else
	{
		if (auto ptr = findResult->second.lock())
		{
			return std::static_pointer_cast<DerivedType>(ptr);
		}
		else
		{
			return createNew<DerivedType>(std::forward<StringType>(relativeFilename));
		}
	}
}

template <class Archive>
void Asset::serialize(Archive& ar, const unsigned /*version*/)
{
	GT_PROFILE_FUNCTION;
	return;
	// ar & BOOST_SERIALIZATION_NVP(mName);
}

} // namespace gintonic