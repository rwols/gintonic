#pragma once

#include "Component.hpp"
#include "Math/box3f.hpp"
#include <vector>

namespace gintonic {

class Octree;
class OctreeNode;
class Transform;
class Collider;

class OctreeComp : public Component
{
    GT_COMPONENT_SERIALIZATION_BOILERPLATE(OctreeComp);

  public:
    class Node
    {
      public:
        /**
         * @brief      Apply a unary function to all OctreeComp in the given
         *             volume.
         *
         * @param[in]  volume  The volume.
         * @param[in]  f       The unary function. The parameter must be of type
         *                     `OctreeComp*`. Its return value must be `void`.
         *
         * @tparam     F       Automatically deduced.
         */
        template <class F> void query(const box3f& volume, F f);

        /**
         * @brief      Apply a unary function to all OctreeComp in the given
         *             volume.
         *
         * @param[in]  volume  The volume.
         * @param[in]  f       The unary function. The parameter must be of type
         *                     `const OctreeComp*`. Its return value must be
         * `void`.
         *
         * @tparam     F       Automatically deduced.
         */
        template <class F> void query(const box3f& volume, F f) const;

        Node*       getRoot() noexcept;
        const Node* getRoot() const noexcept;

        const box3f& getBounds() const noexcept;

        Node(const vec3f& min, const vec3f& max);
        Node(const box3f& bounds);

        ~Node() noexcept;

        bool isLeaf() const noexcept;
        bool hasNoOctreeComponents() const noexcept;

      private:
        box3f                    mBounds;
        void*                    mAllocPlace = nullptr;
        Node*                    mParent = nullptr;
        Node*                    mChildren[8] = {};
        std::vector<OctreeComp*> mComps;

        friend class OctreeComp;

        Node(Node* parent, const vec3f& min, const vec3f& max);

        void insert(OctreeComp*);

        OctreeComp::Node* remove(OctreeComp*) noexcept;

        void update(OctreeComp*);

        void insertRecursive(OctreeComp*);

        OctreeComp::Node* removeRecursive() noexcept;

        void subdivide();

        template <class F> void apply(F f);

        template <class F> void apply(F f) const;
    };

    OctreeComp(experimental::Entity* owner);
    ~OctreeComp() noexcept override;

    Node&       getRoot() noexcept;
    const Node& getRoot() const noexcept;

    void setNode(Node& node);

    static bool classOf(const Component* comp)
    {
        return comp->getKind() == Kind::OctreeComp;
    }

  protected:
    void update() override;

  private:
    friend class Node;

    Node* mNode = nullptr;

    Transform* mTransform = nullptr;

    Collider* mCollider = nullptr;

    std::unique_ptr<Component>
    clone(experimental::Entity* newOwner) const override;

    box3f getBounds() const noexcept;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        using namespace boost::serialization;
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
    }
};

template <class F> void OctreeComp::Node::query(const box3f& volume, F f)
{
    for (auto* comp : mComps)
    {
        if (intersects(volume, comp->getBounds())) f(comp);
    }

    // Case 0: If this is a leaf node, return now.
    if (isLeaf()) return;

    for (auto* child : mChildren)
    {
        // Case 1: search volume completely contained by sub-quad
        // if a node completely contains the query volume, go down that
        // branch and skip the remaining nodes (break this loop)
        //
        // +-----+-----+
        // | +-+ | skip|
        // | |s| | skip|
        // | +-+ | skip|
        // +-----+-----+
        // |skip | skip|
        // |skip | skip|
        // |skip | skip|
        // +-----+-----+
        //
        if (child->mBounds.contains(volume))
        {
            child->query(volume, f);
            break;
        }
        // Case 2: Sub-quad completely contained by search volume
        // if the query volume completely contains a sub-quad,
        // just add all the contents of that quad and its children
        // to the result set. You need to continue the loop to test
        // the other quads
        //
        // +---------+
        // |sssssssss|
        // |s+-----+-+---+
        // |s|sssss|s| ? |
        // |s|sssss|s| ? |
        // |s|sssss|s| ? |
        // |s+-----+-+---+
        // |s|sssss|s| ? |
        // +-+-----+-+ ? |
        //   | ??? | ??? |
        //   +-----+-----+
        //
        else if (volume.contains(child->mBounds))
        {
            child->apply(f);
        }
        // Case 3: search volume intersects with sub-quad
        // traverse into this quad, continue the loop to search other
        // quads
        //
        // +-----+
        // |sssss|
        // |s+---+-+-----+
        // |s|sss| | ??? |
        // +-+---+ | ??? |
        //   |     | ??? |
        //   +-----+-----+
        //   | ??? | ??? |
        //   | ??? | ??? |
        //   | ??? | ??? |
        //   +-----+-----+
        //
        else if (intersects(volume, child->mBounds))
        {
            child->query(volume, f);
        }
        // Case 4: The search volume and sub-quad are disjoint.
    }
}

template <class F> void OctreeComp::Node::query(const box3f& volume, F f) const
{
    for (const auto* comp : mComps)
    {
        if (intersects(volume, comp->getBounds())) f(comp);
    }
    if (isLeaf()) return;
    for (const auto* child : mChildren)
    {
        if (child->mBounds.contains(volume))
        {
            child->query(volume, f);
            break;
        }
        else if (volume.contains(child->mBounds))
        {
            child->apply(f);
            continue;
        }
        else if (intersects(volume, child->mBounds))
        {
            child->query(volume, f);
            continue;
        }
    }
}

template <class F> void OctreeComp::Node::apply(F f)
{
    for (auto* comp : mComps) f(comp);
    if (isLeaf()) return;
    for (auto* child : mChildren) child->apply(f);
}

template <class F> void OctreeComp::Node::apply(F f) const
{
    for (const auto* comp : mComps) f(comp);
    if (isLeaf()) return;
    for (const auto* child : mChildren) child->apply(f);
}

} // gintonic
