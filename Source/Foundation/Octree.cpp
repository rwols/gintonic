#include "Octree.hpp"
#include "exception.hpp"
#include <sstream>

namespace gintonic {

void Octree::destroyEntityHolder(EntityHolder& holder)
{
	if (auto lPtr = std::get<0>(holder).lock())
	{
		std::get<1>(holder).disconnect();
		std::get<2>(holder).disconnect();
		lPtr->mOctree = nullptr;
	}
}

// Constructors.
Octree::Octree(const box3f& bounds)
: mBounds(bounds)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

Octree::Octree(const vec3f& minCorner, const vec3f& maxCorner)
: mBounds(minCorner, maxCorner)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

Octree::Octree(const Octree& other)
: mBounds(other.mBounds)
, mEntities(other.mEntities)
{
	if (other.mAllocationPlace)
	{
		mAllocationPlace = _mm_malloc(sizeof(Octree) * 8, 16);
		assert(mAllocationPlace != nullptr);
		for (std::size_t c = 0; c < 8; ++c)
		{
			mChild[0] = new ((Octree*)mAllocationPlace + c) Octree(*(other.mChild[c]));
		}
	}
	else
	{
		mChild[0] = mChild[1] = mChild[2] 
			= mChild[3] = mChild[4] = mChild[5] 
			= mChild[6] = mChild[7] = nullptr;
	}
	// if (other.mChild[0]) mChild[0] = new Octree(*(other.mChild[0]));
	// else mChild[0] = nullptr;
	// if (other.mChild[1]) mChild[1] = new Octree(*(other.mChild[1]));
	// else mChild[1] = nullptr;
	// if (other.mChild[2]) mChild[2] = new Octree(*(other.mChild[2]));
	// else mChild[2] = nullptr;
	// if (other.mChild[3]) mChild[3] = new Octree(*(other.mChild[3]));
	// else mChild[3] = nullptr;
	// if (other.mChild[4]) mChild[4] = new Octree(*(other.mChild[4]));
	// else mChild[4] = nullptr;
	// if (other.mChild[5]) mChild[5] = new Octree(*(other.mChild[5]));
	// else mChild[5] = nullptr;
	// if (other.mChild[6]) mChild[6] = new Octree(*(other.mChild[6]));
	// else mChild[6] = nullptr;
	// if (other.mChild[7]) mChild[7] = new Octree(*(other.mChild[7]));
	// else mChild[7] = nullptr;
}

Octree::Octree(Octree&& other)
: mAllocationPlace(other.mAllocationPlace)
, mParent(other.mParent)
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

	other.mAllocationPlace = nullptr;
	other.mParent = nullptr;
	
	other.mChild[0] = other.mChild[1] = other.mChild[2] 
		= other.mChild[3] = other.mChild[4] = other.mChild[5] 
		= other.mChild[6] = other.mChild[7] = nullptr;
}

Octree& Octree::operator = (const Octree& other)
{
	// Explicitly call the destructor.
	this->~Octree();

	assert(mAllocationPlace == nullptr);

	if (other.mAllocationPlace != nullptr)
	{
		mAllocationPlace = _mm_malloc(sizeof(Octree) * 8, 16);
		assert(mAllocationPlace != nullptr);
		for (std::size_t c = 0; c < 8; ++c)
		{
			mChild[0] = new ((Octree*)mAllocationPlace + c) Octree(*(other.mChild[c]));
		}
	}

	// if (other.mChild[0]) mChild[0] = new Octree(*(other.mChild[0]));
	// else mChild[0] = nullptr;
	// if (other.mChild[1]) mChild[1] = new Octree(*(other.mChild[1]));
	// else mChild[1] = nullptr;
	// if (other.mChild[2]) mChild[2] = new Octree(*(other.mChild[2]));
	// else mChild[2] = nullptr;
	// if (other.mChild[3]) mChild[3] = new Octree(*(other.mChild[3]));
	// else mChild[3] = nullptr;
	// if (other.mChild[4]) mChild[4] = new Octree(*(other.mChild[4]));
	// else mChild[4] = nullptr;
	// if (other.mChild[5]) mChild[5] = new Octree(*(other.mChild[5]));
	// else mChild[5] = nullptr;
	// if (other.mChild[6]) mChild[6] = new Octree(*(other.mChild[6]));
	// else mChild[6] = nullptr;
	// if (other.mChild[7]) mChild[7] = new Octree(*(other.mChild[7]));
	// else mChild[7] = nullptr;

	return *this;
}

Octree& Octree::operator = (Octree&& other)
{
	// Explicitly call the destructor.
	this->~Octree();

	mAllocationPlace = other.mAllocationPlace;
	other.mAllocationPlace = nullptr;

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
	if (mAllocationPlace != nullptr)
	{
		for (auto*& lChild : mChild)
		{
			// DEBUG_PRINT;
			(Octree::operator delete)(lChild, mAllocationPlace);
			lChild->~Octree();
			lChild = nullptr;
		}
		_mm_free(mAllocationPlace);
		mAllocationPlace = nullptr;
	}
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

void Octree::insert(std::shared_ptr<Entity> entity)
{
	assert(mBounds.contains(entity->globalBoundingBox()));

	// If we are at a leaf node, subdivide the space into eight octants.
	if (isLeaf()) subdivide();

	// If the entity's bounds are contained within
	// one of the mChild's bounds, then insert the entity
	// in that mChild node.
	for (auto* lChildNode : mChild)
	{
		if (lChildNode && lChildNode->mBounds.contains(entity->globalBoundingBox()))
		{
			lChildNode->insert(entity);
			return;
		}
	}

	// If we arrive here, then none of the mChild nodes
	// can contain the entity. So we add it to this node.

	// Subscribe to the onTransformChanged event of the Entity.
	// When the Entity changes its tranformation matrix, we need to
	// update the octree along with it. We store the connection object
	// so that we may disconnect from the entity once it leaves the bounding
	// box of this octree node.
	auto lTransformChangeConnection = entity->onTransformChange.connect( [this] (Entity::SharedPtr thisEntity)
	{
		assert(this == thisEntity->mOctree);
		auto lUpmostParent = this->erase(thisEntity->mOctreeListIter);
		assert(lUpmostParent);
		thisEntity->mOctree = nullptr;
		lUpmostParent->backRecursiveInsert(thisEntity);
	});

	// Subscribe to the onDie event of the Entity.
	// When the Entity dies (i.e. destructor is called), we need to
	// be notified of that event so that we remove the Entity from the octree node too.
	auto lDieConnection = entity->onDie.connect( [this] (Entity* thisEntity)
	{
		assert(this == thisEntity->mOctree);
		#ifdef NDEBUG
			thisEntity->mOctree->erase(thisEntity->mOctreeListIter);
		#else
			auto lUpmostParent = thisEntity->mOctree->erase(thisEntity->mOctreeListIter);
			assert(lUpmostParent);
		#endif
	});

	entity->mOctree = this;
	mEntities.emplace_back(entity, std::move(lTransformChangeConnection), std::move(lDieConnection));
	entity->mOctreeListIter = std::prev(mEntities.end());
}

Octree* Octree::erase()
{
	std::size_t lEraseCount(0);
	// DEBUG_PRINT;
	// if (entity->mOctree != this) return nullptr;
	// DEBUG_PRINT;
	auto lIter = mEntities.begin();
	while (lIter != mEntities.end())
	{
		if (std::get<0>(*lIter).expired())
		{
			// lIter->transformChangeConnection.disconnect();
			// lIter->destructConnection.disconnect();
			std::get<1>(*lIter).disconnect();
			std::get<2>(*lIter).disconnect();
			// destroyEntityHolder(*lIter);
			lIter = mEntities.erase(lIter);
			++lEraseCount;
		}
		else
		{
			++lIter;
		}
	}
	if (lEraseCount > 0)
	{
		return mParent ? mParent->backRecursiveDelete() : this;
	}
	else
	{
		return nullptr;
	}
}

Octree* Octree::erase(const typename std::list<EntityHolder>::iterator& iter)
{
	// std::get<1>(*iter).disconnect();
	// std::get<2>(*iter).disconnect();
	// destroyEntityHolder(*iter);
	mEntities.erase(iter);
	return mParent ? mParent->backRecursiveDelete() : this;
}

Octree* Octree::erase(Entity::SharedPtr entity)
{
	// DEBUG_PRINT;
	if (entity->mOctree != this) return nullptr;
	// DEBUG_PRINT;
	for (auto lIter = mEntities.begin(); lIter != mEntities.end(); ++lIter)
	{
		// DEBUG_PRINT;
		if (entity == std::get<0>(*lIter).lock())
		{
			// DEBUG_PRINT;
			// std::get<1>(*lIter).disconnect();
			// std::get<2>(*lIter).disconnect();

			// // DEBUG_PRINT;
			// assert(std::get<1>(*lIter).connected() == false);
			// assert(std::get<2>(*lIter).connected() == false);
			
			// DEBUG_PRINT;
			destroyEntityHolder(*lIter);
			mEntities.erase(lIter);
			// entity->mOctree = nullptr;

			// DEBUG_PRINT;
			break; 
		}
	}
	return mParent ? mParent->backRecursiveDelete() : this;
}

Octree* Octree::getRoot() noexcept
{
	if (!mParent) return this;
	auto* lResult = mParent;
	while (lResult->mParent) lResult = lResult->mParent;
	return lResult;
}

const Octree* Octree::getRoot() const noexcept
{
	if (!mParent) return this;
	auto* lResult = mParent;
	while (lResult->mParent) lResult = lResult->mParent;
	return lResult;
}

void Octree::backRecursiveInsert(std::shared_ptr<Entity> entity)
{
	if (mBounds.contains(entity->globalBoundingBox()))
	{
		insert(std::move(entity));
	}
	else if (mParent)
	{
		mParent->backRecursiveInsert(std::move(entity));
	}
	else
	{
		// we are the root ... but the entity is too big!
		throw std::runtime_error("Entity too big :(");
	}
}

Octree* Octree::backRecursiveDelete()
{
	// DEBUG_PRINT;

	int lEraseChildren{0};
	for (const auto* lChildNode : mChild)
	{
		lEraseChildren += lChildNode->hasNoEntities() ? 1 : 0;
		lEraseChildren += lChildNode->isLeaf() ? 1 : 0;
	}
	// DEBUG_PRINT;
	if (lEraseChildren == 16)
	{
		// DEBUG_PRINT;
		for (auto*& lChildNode : mChild)
		{
			(Octree::operator delete)(lChildNode, mAllocationPlace);
			lChildNode->~Octree();
			lChildNode = nullptr;
		}
		// DEBUG_PRINT;
		_mm_free(mAllocationPlace);
		// DEBUG_PRINT;
		mAllocationPlace = nullptr;
	}
	// DEBUG_PRINT;

	return mParent ? mParent->backRecursiveDelete() : this;
}

Octree::Octree(const float subdivisionThreshold, Octree* parent, const box3f& bounds)
: mParent(parent)
, mBounds(bounds)
, subdivisionThreshold(subdivisionThreshold)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

Octree::Octree(const float subdivisionThreshold, Octree* parent, const vec3f& min, const vec3f& max)
: mParent(parent)
, mBounds(min, max)
, subdivisionThreshold(subdivisionThreshold)
{
	mChild[0] = mChild[1] = mChild[2] 
		= mChild[3] = mChild[4] = mChild[5] 
		= mChild[6] = mChild[7] = nullptr;
}

void Octree::subdivide()
{
	// DEBUG_PRINT;
	auto lMin = mBounds.minCorner;
	// DEBUG_PRINT;
	// DEBUG_PRINT;
	const auto lHalf = (mBounds.maxCorner - lMin) / 2.0f;
	// DEBUG_PRINT;
	if (lHalf.x <= this->subdivisionThreshold 
		|| lHalf.y <= this->subdivisionThreshold 
		|| lHalf.z <= this->subdivisionThreshold)
	{
		// DEBUG_PRINT;
		return;
	}

	assert(mAllocationPlace == nullptr);

	mAllocationPlace = _mm_malloc(sizeof(Octree) * 8, 16);

	mChild[0] = new ((Octree*)mAllocationPlace + 0) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
	lMin.x += lHalf.x;
	mChild[1] = new ((Octree*)mAllocationPlace + 1) Octree(subdivisionThreshold, this, lMin, lMin + lHalf); 
	lMin.y += lHalf.y;
	mChild[2] = new ((Octree*)mAllocationPlace + 2) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
	lMin.x -= lHalf.x;
	mChild[3] = new ((Octree*)mAllocationPlace + 3) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
	lMin.y -= lHalf.y;
	// at this point, we are at the original location of lMin
	lMin.z += lHalf.z;
	mChild[4] = new ((Octree*)mAllocationPlace + 4) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
	lMin.x += lHalf.x;
	mChild[5] = new ((Octree*)mAllocationPlace + 5) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
	lMin.y += lHalf.y;
	mChild[6] = new ((Octree*)mAllocationPlace + 6) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
	lMin.x -= lHalf.x;
	mChild[7] = new ((Octree*)mAllocationPlace + 7) Octree(subdivisionThreshold, this, lMin, lMin + lHalf);
}

} // namespace gintonic