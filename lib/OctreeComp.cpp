#include "OctreeComp.hpp"
#include "Collider.hpp"
#include "Entity.hpp"
#include "Transform.hpp"
#include <cassert>

#define GT_OCTREE_SUBDIV_THRESHOLD 1.0f

using namespace gintonic;

OctreeComp::OctreeComp(EntityBase* owner) : Component(Kind::OctreeComp, owner)
{
    mTransform = mEntityBase->add<Transform>();
    mCollider = mEntityBase->get<Collider>();
    if (!mCollider) throw std::runtime_error("Missing component: Collider");
}

OctreeComp::~OctreeComp() noexcept
{
    if (mNode) mNode->remove(this);
}

void OctreeComp::setNode(Node& node)
{
    if (mNode) mNode->remove(this);
    mNode = &node;
    mNode->insert(this);
}

void OctreeComp::update()
{
    if (mNode) mNode->update(this);
}

box3f OctreeComp::getBounds() const noexcept
{
    return mCollider->getGlobalBounds();
}

std::unique_ptr<Component> OctreeComp::clone(EntityBase* newOwner) const
{
    auto octree = std::make_unique<OctreeComp>(newOwner);
    octree->mNode = mNode;
    if (mNode) mNode->insert(octree.get());
    return std::move(octree);
}

OctreeComp::Node::Node(Node* parent, const vec3f& min, const vec3f& max)
    : mBounds(min, max), mParent(parent)
{
}

OctreeComp::Node* OctreeComp::Node::getRoot() noexcept
{
    Node* result = this;
    while (result->mParent) result = result->mParent;
    return result;
}

const OctreeComp::Node* OctreeComp::Node::getRoot() const noexcept
{
    const Node* result = this;
    while (result->mParent) result = result->mParent;
    return result;
}

const box3f& OctreeComp::Node::getBounds() const noexcept { return mBounds; }

void OctreeComp::Node::insert(OctreeComp* comp)
{
    if (!mBounds.contains(comp->getBounds()))
    {
        // FIXME: Make a proper exception type.
        throw std::runtime_error(
            "entity not contained in octree bounding box :-(");
    }

    // If we are at a leaf node, subdivide the space into eight octants. If the
    // subdivision threshold is reached, no more subdivision will take place. In
    // that case, isLeaf() will still return true, so we have to check for that
    // when we want to iterate over the children.
    if (isLeaf()) subdivide();

    // If the entity's bounds are contained within one of the children's bounds,
    // then insert the entity in that children node.
    if (!isLeaf())
    {
        for (auto* child : mChildren)
        {
            if (child->mBounds.contains(comp->getBounds()))
            {
                child->insert(comp);
                return;
            }
        }
    }

    // When we arrive here then that means that we reached the subdivision
    // threshold, or we reached a point at which the OctreeComp bounds are so
    // large that further subdivision would cause the boxes of the Nodes to be
    // smaller than the bounds of the OctreeComp. Hence in all cases we may
    // insert the OctreeComp into our array.
    mComps.emplace_back(comp);
    comp->mNode = this;
}

void OctreeComp::Node::insertRecursive(OctreeComp* comp)
{
    if (mBounds.contains(comp->getBounds()))
    {
        insert(comp);
    }
    else if (mParent)
    {
        mParent->insertRecursive(comp);
    }
    else
    {
        // we are the root ... but the entity is too big!
        // FIXME: Substitute with a proper exception type.
        throw std::runtime_error("entity too big :-(");
    }
}

OctreeComp::Node* OctreeComp::Node::remove(OctreeComp* comp) noexcept
{
    for (auto iter = mComps.begin(); iter != mComps.end(); ++iter)
    {
        if (*iter == comp)
        {
            mComps.erase(iter);
            comp->mNode = nullptr;
            return mParent ? mParent->removeRecursive() : this;
        }
    }
    if (isLeaf())
    {
        return nullptr;
    }
    else
    {
        for (auto* child : mChildren)
        {
            auto maybe = child->remove(comp);
            if (maybe) return maybe;
        }
        return nullptr;
    }
}

OctreeComp::Node* OctreeComp::Node::removeRecursive() noexcept
{
    int eraseChildren{0};
    for (const auto* child : mChildren)
    {
        eraseChildren += child->hasNoOctreeComponents() ? 1 : 0;
        eraseChildren += child->isLeaf() ? 1 : 0;
    }
    if (eraseChildren == 16)
    {
        for (auto*& child : mChildren)
        {
            child->~Node();
            // (Node::operator delete)(child, mAllocPlace);
            child = nullptr;
        }
        _mm_free(mAllocPlace);
        mAllocPlace = nullptr;
    }
    return mParent ? mParent->removeRecursive() : this;
}

void OctreeComp::Node::update(OctreeComp* comp)
{
    auto upmostParent = remove(comp);
    upmostParent->insertRecursive(comp);
}

bool OctreeComp::Node::isLeaf() const noexcept
{
    return mAllocPlace != nullptr;
}

bool OctreeComp::Node::hasNoOctreeComponents() const noexcept
{
    return mComps.empty();
}

void OctreeComp::Node::subdivide()
{
    auto lMin = mBounds.minCorner;
    const auto lHalf = (mBounds.maxCorner - lMin) / 2.0f;
    if (lHalf.x <= GT_OCTREE_SUBDIV_THRESHOLD ||
        lHalf.y <= GT_OCTREE_SUBDIV_THRESHOLD ||
        lHalf.z <= GT_OCTREE_SUBDIV_THRESHOLD)
    {
        return;
    }

    assert(!isLeaf());

    mAllocPlace = _mm_malloc(sizeof(Node) * 8, 16);

    mChildren[0] = new ((Node*)mAllocPlace + 0) Node(this, lMin, lMin + lHalf);

    lMin.x += lHalf.x;
    mChildren[1] = new ((Node*)mAllocPlace + 1) Node(this, lMin, lMin + lHalf);

    lMin.y += lHalf.y;
    mChildren[2] = new ((Node*)mAllocPlace + 2) Node(this, lMin, lMin + lHalf);

    lMin.x -= lHalf.x;
    mChildren[3] = new ((Node*)mAllocPlace + 3) Node(this, lMin, lMin + lHalf);

    lMin.y -= lHalf.y;
    // at this point, we are at the original location of lMin
    lMin.z += lHalf.z;
    mChildren[4] = new ((Node*)mAllocPlace + 4) Node(this, lMin, lMin + lHalf);

    lMin.x += lHalf.x;
    mChildren[5] = new ((Node*)mAllocPlace + 5) Node(this, lMin, lMin + lHalf);

    lMin.y += lHalf.y;
    mChildren[6] = new ((Node*)mAllocPlace + 6) Node(this, lMin, lMin + lHalf);

    lMin.x -= lHalf.x;
    mChildren[7] = new ((Node*)mAllocPlace + 7) Node(this, lMin, lMin + lHalf);
}

OctreeComp::Node::~Node() noexcept
{
    if (mAllocPlace)
    {
        for (auto*& child : mChildren)
        {
            // (Node::operator delete)(child, mAllocPlace);
            child->~Node();
            child = nullptr;
        }
        _mm_free(mAllocPlace);
        mAllocPlace = nullptr;
    }
    for (auto* comp : mComps) comp->mNode = nullptr;
}
