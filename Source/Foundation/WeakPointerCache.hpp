#pragma once

#include <map>
#include <memory>

namespace gintonic {

template <class DerivedFromObject>
class WeakPointerCache
{
public:

	using object_type = DerivedFromObject;
	using key_type = typename object_type::name_type;


	std::shared_ptr<object_type> request(key_type key)
	{
		auto lFindResult = mMap.find(key);
		if (lFindResult == mMap.end())
		{
			auto lResult = object_type::create(key);
			mMap.emplace(std::move(key), lResult);
			return lResult;
		}
		else if (auto lSharedPtr = lFindResult->second.lock())
		{
			return lSharedPtr;
		}
		else
		{
			// It's in the map, but the pointer expired.
			auto lResult = object_type::create(std::move(key));
			lFindResult->second = lResult;
			return lResult;
		}
	}

	inline std::size_t size() const noexcept { return mMap.size(); }

	std::size_t removeExpired()
	{
		std::size_t lRemoveCount(0);
		auto lIter = mMap.begin();
		while (lIter != mMap.end())
		{
			if (lIter->second.expired())
			{
				lIter = mMap.erase(lIter);
				++lRemoveCount;
			}
			else
			{
				++lIter;
			}
		}
		return lRemoveCount;
	}

private:
	std::map<key_type,std::weak_ptr<object_type>> mMap;
};

} // namespace gintonic