#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <fbxsdk.h>
#include "filesystem.hpp"
#include "locks.hpp"

namespace gintonic {

template
<
	class Derived
>
class Object : public std::enable_shared_from_this<Derived>
{
protected:

	virtual ~Object() noexcept = default;

	Object(const FbxNode* pFbxNode)
	{
		mGlobalName = boost::filesystem::path(pFbxNode->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
		mLocalName = pFbxNode->GetName();
		mFullName = mGlobalName + '.' + mLocalName;
	}

	Object(const FbxNodeAttribute* pFbxAttribute)
	{
		mGlobalName = boost::filesystem::path(pFbxAttribute->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			mLocalName = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			mLocalName = pFbxAttribute->GetName();
		}
		mFullName = mGlobalName + '.' + mLocalName;
	}

public:

	Object() = default;

	template <class StringType0, class StringType1>
	Object(StringType0&& globalName, StringType1&& localName)
	: mGlobalName(std::forward<StringType0>(globalName))
	, mLocalName(std::forward<StringType1>(localName))
	, mFullName(mGlobalName + '.' + mLocalName)
	{
		/* Empty on purpose. */
	}

	Object(const Object& other)
	: mGlobalName(other.mGlobalName)
	, mLocalName(other.mLocalName)
	, mFullName(other.mFullName)
	{
		/* Don't copy the read_write_lock */
	}

	Object(Object&& other)
	: mGlobalName(std::move(other.mGlobalName))
	, mLocalName(std::move(other.mLocalName))
	, mFullName(std::move(other.mFullName))
	{
		/* Don't move the read_write_lock */
	}
	
	Object& operator = (const Object& other)
	{
		mGlobalName = other.mGlobalName;
		mLocalName = other.mLocalName;
		mFullName = other.mFullName;
		/* Don't copy the read_write_lock */
		return *this;
	}
	
	Object& operator = (Object&& other)
	{
		mGlobalName = std::move(other.mGlobalName);
		mLocalName = std::move(other.mLocalName);
		mFullName = std::move(other.mFullName);
		/* Don't move the read_write_lock */
		return *this;
	}

	static void deleter(Object* object)
	{
		delete object;
	}

	template <class ...ConstructorArguments>
	static std::shared_ptr<Derived> create(ConstructorArguments&&... args)
	{
		return std::shared_ptr<Derived>(
			new Derived(std::forward<ConstructorArguments>(args)...),
			Object::deleter);
	}

	inline void obtainReadLock() const
	{
		mReadWriteLock.obtain_read();
	}

	inline void releaseReadLock() const
	{
		mReadWriteLock.release_read();
	}

	inline void obtainWriteLock()
	{
		mReadWriteLock.obtain_write();
	}
	
	inline void releaseWriteLock()
	{
		mReadWriteLock.release_write();
	}

	template <class StringType>
	void setGlobalName(StringType&& globalName)
	{
		mGlobalName = std::forward<StringType>(globalName);
		mFullName = mGlobalName + '.' + mLocalName;
	}

	template <class StringType>
	void setLocalName(StringType&& localName)
	{
		mLocalName = std::forward<StringType>(localName);
		mFullName = mGlobalName + '.' + mLocalName;
	}

	template <class StringType0, class StringType1>
	void setName(StringType0&& globalPart, StringType1&& localPart)
	{
		mGlobalName = std::forward<StringType0>(globalPart);
		mLocalName = std::forward<StringType1>(localPart);
		mFullName = mGlobalName + '.' + mLocalName;
	}
	
	inline const std::string& getGlobalName() const noexcept
	{
		return mGlobalName;
	}

	inline const std::string& getLocalName() const noexcept
	{
		return mLocalName;
	}

	inline const std::string& getName() const noexcept
	{
		return mFullName;
	}

	struct NameComparer
	{
		inline bool operator()(const Object& lhs, const Object& rhs)
		{
			return lhs.getName() == rhs.getName();
		}
	};

	struct GlobalNameComparer
	{
		inline bool operator()(const Object& lhs, const Object& rhs)
		{
			return lhs.getGlobalName() == rhs.getGlobalName();
		}
	};

	struct LocalNameComparer
	{
		inline bool operator()(const Object& lhs, const Object& rhs)
		{
			return lhs.getLocalName() == rhs.getLocalName();
		}
	};

	struct PointerAddressComparer
	{
		inline bool operator()(const Object& lhs, const Object& rhs)
		{
			return std::addressof(lhs) == std::addressof(rhs);
		}
	};

	inline friend std::ostream& operator << (std::ostream& os, const Object& object)
	{
		return os << object.mFullName;
	}

private:
	std::string mGlobalName;
	std::string mLocalName;
	std::string mFullName;
	read_write_lock mReadWriteLock;

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int /*version*/) const
	{
		ar & mGlobalName & mLocalName;
	}

	template <class Archive>
	void load(Archive& ar, const unsigned int /*version*/)
	{
		ar & mGlobalName & mLocalName;
		mFullName = mGlobalName + '.' + mLocalName;
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();

};



} // namespace gintonic