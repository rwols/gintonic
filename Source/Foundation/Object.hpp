#pragma once

#include "filesystem.hpp"
#include "ReadWriteLock.hpp"

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

	NameType name;

	ReadWriteLock readWriteLock;

	Object() = default;

	Object(const NameType& name)
	: name(name)
	{
		/* Empty on purpose. */
	}

	Object(NameType&& name)
	: name(std::move(name))
	{
		/* Empty on purpose. */
	}

	Object(const Object& other)
	: name(other.name)
	{
		/* Don't copy the ReadWriteLock */
	}

	Object(Object&& other)
	: name(std::move(other.name))
	{
		/* Don't move the ReadWriteLock */
	}
	
	Object& operator = (const Object& other)
	{
		name = other.name;
		/* Don't copy the ReadWriteLock */
		return *this;
	}
	
	Object& operator = (Object&& other)
	{
		name = std::move(other.name);
		/* Don't move the ReadWriteLock */
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
		return os << object.name;
	}

protected:

	virtual ~Object() noexcept = default;

	Object(const FBXSDK_NAMESPACE::FbxNode* pFbxNode)
	: name(pFbxNode->GetName())
	{
		/* Empty on purpose. */
	}

	Object(const FBXSDK_NAMESPACE::FbxNodeAttribute* pFbxAttribute)
	{
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			name = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			name = pFbxAttribute->GetName();
		}
	}

	Object(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* pFbxMaterial)
	: name(pFbxMaterial->GetName())
	{
		/* Empty on purpose. */
	}

	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxNode* pFbxNode)
	{
		name = pFbxNode->GetName();
	}

	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxNodeAttribute* pFbxAttribute)
	{
		if (std::strcmp(pFbxAttribute->GetName(), "") == 0)
		{
			name = pFbxAttribute->GetNode()->GetName();
		}
		else
		{
			name = pFbxAttribute->GetName();
		}
	}

	void setNameWithFbx(const FBXSDK_NAMESPACE::FbxSurfaceMaterial* pFbxMaterial)
	{
		name = pFbxMaterial->GetName();
	}

private:

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & name;
	}
};



} // namespace gintonic