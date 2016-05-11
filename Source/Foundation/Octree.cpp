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

void Octree::insert(std::shared_ptr<Entity> entity)
{
	// DEBUG_PRINT;
	assert(mBounds.contains(entity->globalBoundingBox()));

	// PRINT_VAR(mBounds);
	// PRINT_VAR(entity->globalBoundingBox());

	// If we are at a leaf node, subdivide the space into eight octants.
	if (isLeaf()) subdivide();

	// DEBUG_PRINT;

	// If the entity's bounds are contained within
	// one of the mChild's bounds, then insert the entity
	// in that mChild node.
	for (auto* lChildNode : mChild)
	{
		// DEBUG_PRINT;
		if (lChildNode && lChildNode->mBounds.contains(entity->globalBoundingBox()))
		{
			// DEBUG_PRINT;
			lChildNode->insert(entity);
			// DEBUG_PRINT;
			return;
		}
	}
	// DEBUG_PRINT;

	// If we arrive here, then none of the mChild nodes
	// can contain the entity. So we add it to this node.

	// DEBUG_PRINT;

	// Give the Entity a reference to this node.
	entity->mOctree = this;

	// DEBUG_PRINT;

	// Subscribe to the onTransformChanged event of the Entity.
	// When the Entity changes its tranformation matrix, we need to
	// update the octree along with it. We store the connection object
	// so that we may disconnect from the entity once it leaves the bounding
	// box of this octree node.
	auto lTransformChangeConnection = entity->onTransformChange.connect( [this] (std::shared_ptr<Entity> thisEntity)
	{
		assert(this == thisEntity->mOctree);
		// PRINT_VAR(this);
		auto lUpmostParent = this->erase(thisEntity);
		// PRINT_VAR(lUpmostParent);
		assert(lUpmostParent);
		assert(nullptr == thisEntity->mOctree);
		lUpmostParent->backRecursiveInsert(thisEntity);
	});

	// DEBUG_PRINT;

	// Subscribe to the onDie event of the Entity.
	// When the Entity dies (i.e. destructor is called), we need to
	// be notified of that event so that we remove the Entity from the octree node too.
	auto lDieConnection = entity->onDie.connect( [this] (std::shared_ptr<Entity> thisEntity)
	{
		assert(this == thisEntity->mOctree);
		auto lUpmostParent = this->erase(thisEntity);
		assert(lUpmostParent);
		assert(nullptr == thisEntity->mOctree);
	});

	// DEBUG_PRINT;

	mEntities.emplace_back(std::move(entity), std::move(lTransformChangeConnection), std::move(lDieConnection));
}

Octree* Octree::erase(std::shared_ptr<Entity> entity)
{
	// DEBUG_PRINT;
	if (entity->mOctree != this) return nullptr;
	// DEBUG_PRINT;
	for (auto lIter = mEntities.begin(); lIter != mEntities.end(); ++lIter)
	{
		// DEBUG_PRINT;
		if (entity == lIter->entity.lock())
		{
			// DEBUG_PRINT;
			lIter->transformChangeConnection.disconnect();
			lIter->destructConnection.disconnect();

			// DEBUG_PRINT;
			assert(lIter->transformChangeConnection.connected() == false);
			assert(lIter->destructConnection.connected() == false);
			
			// DEBUG_PRINT;
			mEntities.erase(lIter);
			entity->mOctree = nullptr;

			// DEBUG_PRINT;
			break; 
		}
	}
	// DEBUG_PRINT;
	if (mParent) return mParent->backRecursiveDelete();
	else return this;
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
			delete lChildNode;
			lChildNode = nullptr;
		}
	}
	// DEBUG_PRINT;

	if (mParent) return mParent->backRecursiveDelete();
	else return this; // We're the root node. Don't delete that!
}

// void Octree::notify(std::shared_ptr<Entity> entity)
// {
// 	// Make sure that this Octree node is actually the Octree node
// 	// that's stored in the Entity.
// 	assert(this == entity->mOctree);

// 	// If the entity moved but still fits entirely inside
// 	// the bounds of this quad, then we do nothing at all.
// 	// It's already part of the list of entities of this quad.
// 	if (mBounds.contains(entity->globalBoundingBox()))
// 	{
// 		return;
// 	}

// 	// At this point, the entity has moved and does not
// 	// fit anymore in the current box's bounds. So we erase
// 	// this entity from the current box's Entity list.
// 	for (auto lIter = mEntities.begin(); lIter != mEntities.end(); ++lIter)
// 	{
// 		if (lIter->entity.lock() == entity)
// 		{
// 			lIter->transformChangeConnection.disconnect();
// 			lIter->destructConnection.disconnect();

// 			// Make sure we are not subscribed to the events of the Entity.
// 			assert(lIter->transformChangeConnection.connected() == false);
// 			assert(lIter->destructConnection.connected() == false);
			
// 			mEntities.erase(lIter);
// 			entity->mOctree = nullptr;

// 			break;
// 		}
// 	}

// 	// Make sure the Entity was actually removed from this Octree node.
// 	assert(entity->mOctree == nullptr);

// 	// If the quad has a parent, we let the parent handle
// 	// the moved entity.
// 	if (mParent)
// 	{
// 		mParent->notifyHelper(std::move(entity));
// 	}
// 	// If the box has no parent, then this quad is the root node.
// 	// But the entity doesn't fit anymore; i.e. it doesn't fit
// 	// in the Octree at all.
// 	// We handle this by just adding this entity to the root node.
// 	else
// 	{
// 		// mEntities.emplace_back(std::move(entity));
// 	}
// }

void Octree::notifyHelper(std::shared_ptr<Entity> entity)
{
	if (mBounds.contains(entity->globalBoundingBox()))
	{
		insert(entity);
	}
	else if (mParent)
	{
		mParent->notifyHelper(entity);
	}
	else
	{
		// This node is the root node, but the entity
		// still does not fit.
		// We handle this by just adding this entity to the root node.
		entity->mOctree = this;
		// mEntities.push_back(entity);
	}
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
	auto min = mBounds.minCorner;
	// DEBUG_PRINT;
	auto max = mBounds.maxCorner;
	// DEBUG_PRINT;
	const auto half = (max - min) / 2.0f;
	// DEBUG_PRINT;
	if (half.x <= this->subdivisionThreshold 
		|| half.y <= this->subdivisionThreshold 
		|| half.z <= this->subdivisionThreshold)
	{
		// DEBUG_PRINT;
		return;
	}

	// DEBUG_PRINT;
	mChild[0] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.x += half.x;
	// DEBUG_PRINT;
	mChild[1] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.y += half.y;
	// DEBUG_PRINT;
	mChild[2] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.x -= half.x;
	// DEBUG_PRINT;
	mChild[3] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.y -= half.y;
	// at this point, we are at the original location of min
	min.z += half.z;
	// DEBUG_PRINT;
	mChild[4] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.x += half.x;
	// DEBUG_PRINT;
	mChild[5] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.y += half.y;
	// DEBUG_PRINT;
	mChild[6] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
	min.x -= half.x;
	// DEBUG_PRINT;
	mChild[7] = new Octree(subdivisionThreshold, this, min, min + half);
	// DEBUG_PRINT;
}

} // namespace gintonic