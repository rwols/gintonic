#pragma once

#include "filesystem.hpp"
#include "locks.hpp"

#include <string>
#include <memory>
#include <iostream>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include <fbxsdk/core/fbxobject.h>
#include <fbxsdk/scene/shading/fbxsurfacematerial.h>
#include <fbxsdk/scene/geometry/fbxnode.h>
#include <fbxsdk/scene/geometry/fbxnodeattribute.h>

namespace gintonic {

template
<
	class Derived,
	class NameType
>
class Object : public std::enable_shared_from_this<Derived>
{
public:

	Object() = default;

	Object(const NameType& name)
	: mName(name)
	{
		/* Empty on purpose. */
	}

	Object(NameType&& name)
	: mName(std::move(name))
	{
		/* Empty on purpose. */
	}

	Object(const Object& other)
	: mName(other.mName)
	{
		/* Don't copy the read_write_lock */
	}

	Object(Object&& other)
	: mName(std::move(other.mName))
	{
		/* Don't move the read_write_lock */
	}
	
	Object& operator = (const Object& other)
	{
		mName = other.mName;
		/* Don't copy the read_write_lock */
		return *this;
	}
	
	Object& operator = (Object&& other)
	{
		mName = std::move(other.mName);
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

	template <class ArgType>
	void setName(ArgType&& name)
	{
		mName = std::forward<ArgType>(name);
	}
	
	inline const NameType& getName() const noexcept
	{
		return mName;
	}

	struct NameComparer
	{
		inline bool operator()(const Object& lhs, const Object& rhs)
		{
			return lhs.getName() == rhs.getName();
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

protected:

	virtual ~Object() noexcept = default;

	Object(const FBXSDK_NAMESPACE::FbxNode* pFbxNode)
	: mName(pFbxNode->GetName())
	{
		/* Empty on purpose. */
	}

	Object(const FBXSDK_NAMESPACE::FbxNodeAttribute* pFbxAttribute)
	{
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			mName = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			mName = pFbxAttribute->GetName();
		}
	}

	Object(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* pFbxMaterial)
	: mName(pFbxMaterial->GetName())
	{
		/* Empty on purpose. */
	}

	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxNode* pFbxNode)
	{
		mName = pFbxNode->GetName();
	}

	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxNodeAttribute* pFbxAttribute)
	{
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			mName = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			mName = pFbxAttribute->GetName();
		}
	}

	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* pFbxMaterial)
	{
		mName = pFbxMaterial->GetName();
	}

private:
	NameType mName;
	read_write_lock mReadWriteLock;

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & mName;
	}
};



} // namespace gintonic