#include "Octree.hpp"
#include "exception.hpp"
#include <sstream>

namespace gintonic {

// Constructors.
Octree::Octree(const box3f& bounds)
: mBounds(bounds)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

Octree::Octree(const vec3f& min_corner, const vec3f& max_corner)
: mBounds(min_corner, max_corner)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

Octree::Octree(const Octree& other)
: mBounds(other.mBounds)
, mEntities(other.mEntities)
{
	if (other.mChild[0]) mChild[0] = new Octree(*(other.mChild[0]));
	else mChild[0] = nullptr;
	if (other.mChild[1]) mChild[1] = new Octree(*(other.mChild[1]));
	else mChild[1] = nullptr;
	if (other.mChild[2]) mChild[2] = new Octree(*(other.mChild[2]));
	else mChild[2] = nullptr;
	if (other.mChild[3]) mChild[3] = new Octree(*(other.mChild[3]));
	else mChild[3] = nullptr;
	if (other.mChild[4]) mChild[4] = new Octree(*(other.mChild[4]));
	else mChild[4] = nullptr;
	if (other.mChild[5]) mChild[5] = new Octree(*(other.mChild[5]));
	else mChild[5] = nullptr;
	if (other.mChild[6]) mChild[6] = new Octree(*(other.mChild[6]));
	else mChild[6] = nullptr;
	if (other.mChild[7]) mChild[7] = new Octree(*(other.mChild[7]));
	else mChild[7] = nullptr;
}

Octree::Octree(Octree&& other)
: mParent(other.mParent)
, mBounds(std::move(other.mBounds))
, mEntities(std::move(other.mEntities))
{
	mChild[0] = other.mChild[0];
	mChild[1] = other.mChild[1];
	mChild[2] = other.mChild[2];
	mChild[3] = other.mChild[3];
	mChild[4] = other.mChild[4];
	mChild[5] = other.mChild[5];
	mChild[6] = other.mChild[6];
	mChild[7] = other.mChild[7];

	other.mParent = nullptr;
	
	other.mChild[0] = other.mChild[1] = other.mChild[2] 
		= other.mChild[3] = other.mChild[4] = other.mChild[5] 
		= other.mChild[6] = other.mChild[7] = nullptr;
}

Octree& Octree::operator = (const Octree& other)
{
	// Explicitly call the destructor.
	this->~Octree();

	if (other.mChild[0]) mChild[0] = new Octree(*(other.mChild[0]));
	else mChild[0] = nullptr;
	if (other.mChild[1]) mChild[1] = new Octree(*(other.mChild[1]));
	else mChild[1] = nullptr;
	if (other.mChild[2]) mChild[2] = new Octree(*(other.mChild[2]));
	else mChild[2] = nullptr;
	if (other.mChild[3]) mChild[3] = new Octree(*(other.mChild[3]));
	else mChild[3] = nullptr;
	if (other.mChild[4]) mChild[4] = new Octree(*(other.mChild[4]));
	else mChild[4] = nullptr;
	if (other.mChild[5]) mChild[5] = new Octree(*(other.mChild[5]));
	else mChild[5] = nullptr;
	if (other.mChild[6]) mChild[6] = new Octree(*(other.mChild[6]));
	else mChild[6] = nullptr;
	if (other.mChild[7]) mChild[7] = new Octree(*(other.mChild[7]));
	else mChild[7] = nullptr;

	return *this;
}

Octree& Octree::operator = (Octree&& other)
{
	// Explicitly call the destructor.
	this->~Octree();

	mChild[0] = other.mChild[0];
	mChild[1] = other.mChild[1];
	mChild[2] = other.mChild[2];
	mChild[3] = other.mChild[3];
	mChild[4] = other.mChild[4];
	mChild[5] = other.mChild[5];
	mChild[6] = other.mChild[6];
	mChild[7] = other.mChild[7];

	mParent = other.mParent; // Move the parent.

	other.mParent = nullptr;
	
	other.mChild[0] = other.mChild[1] = other.mChild[2] 
		= other.mChild[3] = other.mChild[4] = other.mChild[5] 
		= other.mChild[6] = other.mChild[7] = nullptr;

	return *this;
}

// Non-trivial destructor calls delete on all of its children.
Octree::~Octree()
{
	for (const auto* c : mChild) delete c;
}

bool Octree::isLeaf() const noexcept
{
	for (const auto* c : mChild) if (c) return false;
	return true;
}

std::size_t Octree::count() const
{
	std::size_t result = mEntities.size();
	for (const auto* c : mChild) if (c) result += c->count();
	return result;
}

void Octree::insert(Entity* object)
{
	#ifndef NDEBUG
	// If the item is not contained in this quad,
	// we made a logical mistake somewhere
	if (!mBounds.contains(object->globalBoundingBox()))
	{
		std::stringstream ss;
		ss << "Entity with bounds " << object->globalBoundingBox()
			<< " is not entirely contained within "
			<< mBounds << ".";
		throw exception(ss.str());
	}
	#endif

	// If we are at a leaf node, subdivide the space.
	if (isLeaf()) subdivide();

	// If the object's bounds are contained within
	// one of the mChild's bounds, then insert the object
	// in that mChild node.
	for (auto* c : mChild)
	{
		if (c && c->mBounds.contains(object->globalBoundingBox()))
		{
			c->insert(object);
			return;
		}
	}

	// If we arrive here, then none of the mChild nodes
	// can contain the object. So we add it to this node.
	mEntities.push_back(object);

	// Give the Entity a reference to this node.
	object->mOctree = this;
}

int Octree::erase(Entity* object)
{
	if (object->mOctree != this) return 0;
	for (auto iter = mEntities.begin(); iter != mEntities.end(); ++iter)
	{
		if (object == *iter)
		{
			mEntities.erase(iter);
			object->mOctree = nullptr;
			break;
		}
	}
	if (mParent) // check if we need to delete ourself and our neighbors
	{
		int lEraseMyself = 0;
		for (const auto* c : mParent->mChild)
		{
			lEraseMyself += c->hasNoEntities() ? 1 : 0;
			lEraseMyself += c->isLeaf() ? 1 : 0;
		}
		if (lEraseMyself == (1 << (2+1)))
		{
			for (const auto* c : mParent->mChild)
			{
				if (c != this) delete c;
			}
			for (auto*& c : mParent->mChild) c = nullptr;
			delete this;
			return 2;
		}
	}
	return 1;
}

void Octree::notify(Entity* object)
{
	if (this != object->mOctree)
	{
		throw std::logic_error("Cannot call notify on a node which is not"
		" equal to the object's spatialtree property.");
	}
	// If the object moved but still fits entirely inside
	// the bounds of this quad, then we do nothing at all.
	// It's already part of the list of entities of this quad.
	if (mBounds.contains(object->globalBoundingBox()))
	{
		return;
	}
	// At this point, the object has moved and does not
	// fit anymore in the current quad's bounds. So we erase
	// this object from the current quad's Entity list.

	#ifndef NDEBUG
	bool lObjectHasBeenRemoved = false;
	#endif

	for (auto i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if (*i == object)
		{
			mEntities.erase(i);

			#ifndef NDEBUG
			lObjectHasBeenRemoved = true;
			#endif

			break;
		}
	}

	#ifndef NDEBUG
	if (!lObjectHasBeenRemoved) throw std::logic_error("Object was not present in this quad.");
	#endif

	// If the quad has a parent, we let the parent handle
	// the moved object.
	if (mParent)
	{
		mParent->notifyHelper(object);
	}
	// If the uad has no parent, then this quad is the root node.
	// But the object doesn't fit anymore; i.e. it doesn't fit
	// in the Octree at all.
	// We handle this by just adding this object to the root node.
	else
	{
		object->mOctree = this;
		mEntities.push_back(object);
	}
}

void Octree::notifyHelper(Entity* object)
{
	if (mBounds.contains(object->globalBoundingBox()))
	{
		insert(object);
	}
	else if (mParent)
	{
		mParent->notifyHelper(object);
	}
	else
	{
		// This node is the root node, but the object
		// still does not fit.
		// We handle this by just adding this object to the root node.
		object->mOctree = this;
		mEntities.push_back(object);
	}
}

Octree::Octree(Octree* parent, const box3f& bounds)
: mParent(parent)
, mBounds(bounds)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

Octree::Octree(Octree* parent, const vec3f& min, const vec3f& max)
: mParent(parent)
, mBounds(min, max)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

void Octree::subdivide()
{
	auto min = mBounds.min_corner;
	auto max = mBounds.max_corner;
	const auto half = (max - min) / 2.0f;
	if (half.x <= this->subdivisionThreshold 
		|| half.y <= this->subdivisionThreshold 
		|| half.z <= this->subdivisionThreshold)
	{
		return;
	}
	mChild[0] = new Octree(this, min, min + half);
	min.x += half.x;
	mChild[1] = new Octree(this, min, min + half);
	min.y += half.y;
	mChild[2] = new Octree(this, min, min + half);
	min.x -= half.x;
	mChild[3] = new Octree(this, min, min + half);
	min.y -= half.y;
	// at this point, we are at the original location of min
	min.z += half.z;
	mChild[4] = new Octree(this, min, min + half);
	min.x += half.x;
	mChild[5] = new Octree(this, min, min + half);
	min.y += half.y;
	mChild[6] = new Octree(this, min, min + half);
	min.x -= half.x;
	mChild[7] = new Octree(this, min, min + half);
}

} // namespace gintonic