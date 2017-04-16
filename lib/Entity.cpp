#include "Entity.hpp"

// #include "Foundation/Octree.hpp"

#include "Graphics/Mesh.hpp"
#include "Graphics/ShadowBuffer.hpp"

#include "Math/SQTstack.hpp"
#include "Math/mat4fstack.hpp"
#include "Math/vec4f.hpp"

#include <fbxsdk.h>

namespace gintonic
{

Entity::Entity()
    : Entity("UntitledEntity",
             SQT(vec3f(1.0f, 1.0f, 1.0f), quatf(1.0f, 0.0f, 0.0f, 0.0f),
                 vec3f(0.0f, 0.0f, 0.0f)))
{
    /* Empty on purpose. */
}

Entity::Entity(std::string name)
    : Entity(std::move(name),
             SQT(vec3f(1.0f, 1.0f, 1.0f), quatf(1.0f, 0.0f, 0.0f, 0.0f),
                 vec3f(0.0f, 0.0f, 0.0f)))
{
    /* Empty on purpose. */
}

void Entity::update()
{
    for (const auto& comp : mComponents) comp->update();
}

void Entity::lateUpdate()
{
    for (const auto& comp : mComponents) comp->lateUpdate();
}

Entity::Entity(std::string name, const SQT& localTransform)
    : Super(std::move(name)), mLocalTransform(localTransform),
      mGlobalTransform(mLocalTransform)
{
    /* Empty on purpose. */
}

Entity::Entity(const FbxNode* pFbxNode)
    : Super(pFbxNode), mLocalTransform(pFbxNode),
      mGlobalTransform(mLocalTransform)
{
    /* Empty on purpose. */
}

Entity::Entity(const Entity& other)
    : Super(other), mLocalTransform(other.mLocalTransform),
      mGlobalTransform(other.mGlobalTransform)
      // , mOctree(other.mOctree)
      // , mOctreeListIter(other.mOctreeListIter)
      ,
      castShadow(other.castShadow), material(other.material), mesh(other.mesh),
      light(other.light), camera(other.camera)
{
    /* Do NOT copy mChildren */
    /* Do NOT copy mParent */
    /* Do NOT copy shadowBuffer */
}

Entity::Entity(Entity&& other) noexcept
    : Super(std::move(other)),
      mLocalTransform(std::move(other.mLocalTransform)),
      mGlobalTransform(std::move(other.mGlobalTransform)),
      mChildren(std::move(other.mChildren)), mParent(std::move(other.mParent))
      // , mOctree(std::move(other.mOctree))
      // , mOctreeListIter(std::move(other.mOctreeListIter))
      ,
      castShadow(std::move(other.castShadow)),
      material(std::move(other.material)), mesh(std::move(other.mesh)),
      light(std::move(other.light)), camera(std::move(other.camera)),
      shadowBuffer(std::move(other.shadowBuffer))
{
    /* DO move mChildren */
    /* DO move mParent */
    /* DO move shadowBuffer */
}

Entity& Entity::operator=(const Entity& other)
{
    Super::operator=(other);
    mLocalTransform = other.mLocalTransform;
    mGlobalTransform = other.mGlobalTransform;
    // mOctree = other.mOctree;
    // mOctreeListIter = other.mOctreeListIter;
    castShadow = other.castShadow;
    material = other.material;
    mesh = other.mesh;
    light = other.light;
    camera = other.camera;

    /* Do NOT copy mChildren */
    /* Do NOT copy mParent */
    /* Do NOT copy shadowBuffer */

    return *this;
}

Entity& Entity::operator=(Entity&& other) noexcept
{
    Super::operator=(std::move(other));
    mLocalTransform = std::move(other.mLocalTransform);
    mGlobalTransform = std::move(other.mGlobalTransform);
    mChildren = std::move(other.mChildren);
    mParent = std::move(other.mParent);
    // mOctree = std::move(other.mOctree);
    // mOctreeListIter = std::move(other.mOctreeListIter);
    castShadow = std::move(other.castShadow);
    material = std::move(other.material);
    mesh = std::move(other.mesh);
    light = std::move(other.light);
    camera = std::move(other.camera);
    shadowBuffer = std::move(other.shadowBuffer);

    /* DO move mChildren */
    /* DO move mParent */
    /* DO move shadowBuffer */

    return *this;
}

box3f Entity::globalBoundingBox() const noexcept
{
    box3f lGlobalBoundingBox(mGlobalTransform.data[3],
                             mGlobalTransform.data[3]);
    if (mesh)
    {
        const auto& lBBox = mesh->getLocalBoundingBox();
        lGlobalBoundingBox.minCorner += lBBox.minCorner;
        lGlobalBoundingBox.maxCorner += lBBox.maxCorner;
    }
    return lGlobalBoundingBox;
}

void Entity::updateGlobalInfo() noexcept
{
    if (auto lParent = mParent.lock())
    {
        mGlobalTransform = lParent->mGlobalTransform * mat4f(mLocalTransform);
    }
    else
    {
        mGlobalTransform = mat4f(mLocalTransform);
    }
    for (auto lChild : mChildren)
    {
        lChild->updateGlobalInfo();
    }
}

void Entity::setScale(const vec3f& scale) noexcept
{
    mLocalTransform.scale = scale;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::multiplyScale(const vec3f& scale) noexcept
{
    mLocalTransform.scale *= scale;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::setTranslation(const vec3f& translation) noexcept
{
    mLocalTransform.translation = translation;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::setTranslationX(const float x) noexcept
{
    mLocalTransform.translation.x = x;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}
void Entity::setTranslationY(const float y) noexcept
{
    mLocalTransform.translation.y = y;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::setTranslationZ(const float z) noexcept
{
    mLocalTransform.translation.z = z;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::addTranslation(const vec3f& translation) noexcept
{
    mLocalTransform.translation += translation;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::setRotation(const quatf& rotation) noexcept
{
    mLocalTransform.rotation = rotation;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::postMultiplyRotation(const quatf& rotation) noexcept
{
    mLocalTransform.rotation *= rotation;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::preMultiplyRotation(const quatf& rotation) noexcept
{
    mLocalTransform.rotation = rotation * mLocalTransform.rotation;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::setLocalTransform(const SQT& sqt) noexcept
{
    mLocalTransform = sqt;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::postAddLocalTransform(const SQT& sqt) noexcept
{
    mLocalTransform %= sqt;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::preAddLocalTransform(const SQT& sqt) noexcept
{
    mLocalTransform = sqt % mLocalTransform;
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::moveForward(const float amount) noexcept
{
    mLocalTransform.translation +=
        amount * mLocalTransform.rotation.forward_direction();
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::moveBackward(const float amount) noexcept
{
    mLocalTransform.translation -=
        amount * mLocalTransform.rotation.forward_direction();
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::moveRight(const float amount) noexcept
{
    mLocalTransform.translation +=
        amount * mLocalTransform.rotation.right_direction();
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::moveLeft(const float amount) noexcept
{
    mLocalTransform.translation -=
        amount * mLocalTransform.rotation.right_direction();
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::moveUp(const float amount) noexcept
{
    mLocalTransform.translation +=
        amount * mLocalTransform.rotation.up_direction();
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

void Entity::moveDown(const float amount) noexcept
{
    mLocalTransform.translation -=
        amount * mLocalTransform.rotation.up_direction();
    updateGlobalInfo();
    onTransformChange(shared_from_this());
}

// mat4f Entity::computeGlobalTransform() noexcept
// {
//  if (auto lParent = mParent.lock())
//  {
//      return lParent->globalTransform() * mat4f(mLocalTransform);
//  }
//  else
//  {
//      return mat4f(mLocalTransform);
//  }
// }

void Entity::addChild(std::shared_ptr<Entity> child)
{
    if (child->mParent.lock())
    {
        // child has a parent. Do we have to move or clone it?
        if (child.use_count() == 1)
        {
            // We can move it!
            child->mParent = shared_from_this();
            mChildren.push_front(child);
            child->updateGlobalInfo();
            child->onTransformChange(child);
        }
        else
        {
            // We must make a recursive copy.
            auto lCopy = child->cloneRecursive();
            lCopy->mParent = shared_from_this();
            mChildren.push_front(lCopy);
            lCopy->updateGlobalInfo();
            lCopy->onTransformChange(lCopy);
        }
    }
    else
    {
        child->mParent = shared_from_this();
        mChildren.push_front(child);
        child->updateGlobalInfo();
        child->onTransformChange(child);
    }
}

void Entity::removeChild(std::shared_ptr<Entity> child)
{
#ifndef NDEBUG
    bool lChildWasRemoved = false;
#endif

    for (auto i = begin(); i != end(); ++i)
    {
        if (i->get() == child.get())
        {
            mChildren.erase(i);

#ifndef NDEBUG
            lChildWasRemoved = true;
#endif

            break;
        }
    }

#ifndef NDEBUG
    if (lChildWasRemoved)
        child->mParent = std::shared_ptr<Entity>(nullptr);
    else
        throw std::logic_error("Entity was not a child.");
#else
    child->mParent = std::shared_ptr<Entity>(nullptr);
#endif
}

void Entity::setParent(std::shared_ptr<Entity> parent)
{
    if (auto lParent = mParent.lock())
    {
        lParent->removeChild(shared_from_this());
    }
    parent->addChild(shared_from_this()); // also sets mParent
}

void Entity::unsetParent()
{
    if (auto lParent = mParent.lock())
    {
        lParent->removeChild(shared_from_this());
    }
    else
    {
        mParent = std::shared_ptr<Entity>(nullptr);
    }
}

void Entity::getViewMatrix(mat4f& result) const noexcept
{
    const auto lRight = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
    const auto lUp = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
    const auto lForward = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
    const auto lEye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

    result.m00 = lRight.x;
    result.m10 = lUp.x;
    result.m20 = -lForward.x;
    result.m30 = 0.0f;

    result.m01 = lRight.y;
    result.m11 = lUp.y;
    result.m21 = -lForward.y;
    result.m31 = 0.0f;

    result.m02 = lRight.z;
    result.m12 = lUp.z;
    result.m22 = -lForward.z;
    result.m32 = 0.0f;

    result.m03 = -dot(lRight, lEye);
    result.m13 = -dot(lUp, lEye);
    result.m23 = dot(lForward, lEye);
    result.m33 = 1.0f;
}

void Entity::updateViewMatrix(mat4f& alreadyAffineMatrix) const noexcept
{
    const auto lRight = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
    const auto lUp = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
    const auto lForward = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
    const auto lEye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

    alreadyAffineMatrix.m00 = lRight.x;
    alreadyAffineMatrix.m10 = lUp.x;
    alreadyAffineMatrix.m20 = -lForward.x;

    alreadyAffineMatrix.m01 = lRight.y;
    alreadyAffineMatrix.m11 = lUp.y;
    alreadyAffineMatrix.m21 = -lForward.y;

    alreadyAffineMatrix.m02 = lRight.z;
    alreadyAffineMatrix.m12 = lUp.z;
    alreadyAffineMatrix.m22 = -lForward.z;

    alreadyAffineMatrix.m03 = -dot(lRight, lEye);
    alreadyAffineMatrix.m13 = -dot(lUp, lEye);
    alreadyAffineMatrix.m23 = dot(lForward, lEye);
}

mat4f Entity::getViewMatrix() const noexcept
{
    const auto lRight = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
    const auto lUp = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
    const auto lForward = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
    const auto lEye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

    mat4f lResult;

    lResult.m00 = lRight.x;
    lResult.m10 = lUp.x;
    lResult.m20 = -lForward.x;
    lResult.m30 = 0.0f;

    lResult.m01 = lRight.y;
    lResult.m11 = lUp.y;
    lResult.m21 = -lForward.y;
    lResult.m31 = 0.0f;

    lResult.m02 = lRight.z;
    lResult.m12 = lUp.z;
    lResult.m22 = -lForward.z;
    lResult.m32 = 0.0f;

    lResult.m03 = -dot(lRight, lEye);
    lResult.m13 = -dot(lUp, lEye);
    lResult.m23 = dot(lForward, lEye);
    lResult.m33 = 1.0f;

    return lResult;
}

Entity::~Entity() noexcept
{
    try
    {
        onDie(this);
    }
    catch (...)
    {
        // Absorb exceptions.
    }
    // DEBUG_PRINT;
    for (auto lChild : mChildren)
    {
        if (lChild.use_count()) // Not thread-safe :(
        {
            // DEBUG_PRINT;
            // std::cerr << "\tChild refcount: " << lChild.use_count() << '\n';
            // DEBUG_PRINT;
            // std::cerr << "\t" << lChild->name << "\n";
            // DEBUG_PRINT;
            lChild->mParent = SharedPtr(nullptr);
            // DEBUG_PRINT;
            lChild->mGlobalTransform.decompose(lChild->mLocalTransform);
            // DEBUG_PRINT;
        }
    }
    // DEBUG_PRINT;
    if (auto lParent = mParent.lock())
    {
        for (auto lIter = lParent->mChildren.begin();
             lIter != lParent->mChildren.end(); ++lIter)
        {
            if (lIter->get() == this)
            {
                lParent->mChildren.erase(lIter);
                return;
            }
        }
    }
    // If no lock can be acquired, the parent is dead.
    // In that case we don't have to care about removing
    // ourselves from the children list of the parent
    // since that's gone.
}

std::shared_ptr<Entity> Entity::cloneRecursive() const
{
    // Invoke the copy constructor.
    std::shared_ptr<Entity> lClone(new Entity(*this));
    for (auto lChild : mChildren)
    {
        lClone->addChild(lChild->cloneRecursive());
    }
    return lClone;
}

} // namespace gintonic
