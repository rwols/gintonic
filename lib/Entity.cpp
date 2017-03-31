#include <gintonic/Entity.hpp>

#include <gintonic/Component.hpp>

// #include "Foundation/Octree.hpp"

#include "Graphics/Mesh.hpp"
#include "Graphics/ShadowBuffer.hpp"

#include "Math/SQTstack.hpp"
#include "Math/mat4fstack.hpp"
#include "Math/vec4f.hpp"

#include <fbxsdk.h>

using namespace gintonic;

Entity::Entity(std::string name) : name(std::move(name))
{
    /* Empty on purpose. */
}

Entity::Entity(const FbxNode* pFbxNode)
    : Entity(std::string(pFbxNode->GetName()))
{
    /* Empty on purpose. */
}

Entity::Entity(const Entity& other)
    : name(other.name), mChildren(other.mChildren)
{
    for (const auto& comp : other.mComponents)
    {
        mComponents.emplace_back(comp->clone(*this));
    }
}

Entity& Entity::operator=(const Entity& other)
{
    name = other.name;
    mChildren = other.mChildren;
    for (const auto& comp : other.mComponents)
    {
        mComponents.emplace_back(comp->clone(*this));
    }
    return *this;
}

void Entity::update()
{
    for (auto& comp : mComponents) comp->update();
}

void Entity::lateUpdate()
{
    for (auto& comp : mComponents) comp->lateUpdate();
}

void Entity::onParentChange()
{
    for (auto& comp : mComponents) comp->onParentChange();
}

void Entity::addChild(const Entity& child)
{
    mChildren.push_back(child);
    mChildren.back().mParent = this;
    mChildren.back().onParentChange();
}

void Entity::addChild(Entity&& child)
{
    mChildren.emplace_back(std::move(child));
    mChildren.back().mParent = this;
    mChildren.back().onParentChange();
}

Entity Entity::removeChild(const Entity& child)
{
    for (auto i = begin(); i != end(); ++i)
    {
        if (*i == child) return removeChild(i);
    }
    throw std::out_of_range("No such child entity present");
}

Entity Entity::removeChild(iterator iter) noexcept
{
    Entity entity(std::move(*iter));
    mChildren.erase(iter);
    return entity;
}

void Entity::moveChild(Entity& oldParent, Entity& child)
{
    addChild(oldParent.removeChild(child));
}

void Entity::moveChild(Entity& oldParent, Entity::iterator child)
{
    addChild(oldParent.removeChild(child));
}

// void Entity::removeChild(std::shared_ptr<Entity> child)
// {
// #ifndef NDEBUG
//     bool lChildWasRemoved = false;
// #endif

//     for (auto i = begin(); i != end(); ++i)
//     {
//         if (i->get() == child.get())
//         {
//             mChildren.erase(i);

// #ifndef NDEBUG
//             lChildWasRemoved = true;
// #endif

//             break;
//         }
//     }

// #ifndef NDEBUG
//     if (lChildWasRemoved)
//         child->mParent = std::shared_ptr<Entity>(nullptr);
//     else
//         throw std::logic_error("Entity was not a child.");
// #else
//     child->mParent = std::shared_ptr<Entity>(nullptr);
// #endif
// }

// void Entity::setParent(Entity* parent)
// {
//  if (!parent) return;
//  if (mParent) mParent->removeChild(*this);
//  parent->addChild(std::move(*this));
// }

// void Entity::unsetParent()
// {
//     if (auto lParent = mParent.lock())
//     {
//         lParent->removeChild(shared_from_this());
//     }
//     else
//     {
//         mParent = std::shared_ptr<Entity>(nullptr);
//     }
// }

// void Entity::getViewMatrix(mat4f& result) const noexcept
// {
//     const auto lRight = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
//     const auto lUp = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
//     const auto lForward = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
//     const auto lEye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

//     result.m00 = lRight.x;
//     result.m10 = lUp.x;
//     result.m20 = -lForward.x;
//     result.m30 = 0.0f;

//     result.m01 = lRight.y;
//     result.m11 = lUp.y;
//     result.m21 = -lForward.y;
//     result.m31 = 0.0f;

//     result.m02 = lRight.z;
//     result.m12 = lUp.z;
//     result.m22 = -lForward.z;
//     result.m32 = 0.0f;

//     result.m03 = -dot(lRight, lEye);
//     result.m13 = -dot(lUp, lEye);
//     result.m23 = dot(lForward, lEye);
//     result.m33 = 1.0f;
// }

// void Entity::updateViewMatrix(mat4f& alreadyAffineMatrix) const noexcept
// {
//     const auto lRight = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
//     const auto lUp = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
//     const auto lForward = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
//     const auto lEye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

//     alreadyAffineMatrix.m00 = lRight.x;
//     alreadyAffineMatrix.m10 = lUp.x;
//     alreadyAffineMatrix.m20 = -lForward.x;

//     alreadyAffineMatrix.m01 = lRight.y;
//     alreadyAffineMatrix.m11 = lUp.y;
//     alreadyAffineMatrix.m21 = -lForward.y;

//     alreadyAffineMatrix.m02 = lRight.z;
//     alreadyAffineMatrix.m12 = lUp.z;
//     alreadyAffineMatrix.m22 = -lForward.z;

//     alreadyAffineMatrix.m03 = -dot(lRight, lEye);
//     alreadyAffineMatrix.m13 = -dot(lUp, lEye);
//     alreadyAffineMatrix.m23 = dot(lForward, lEye);
// }

// mat4f Entity::getViewMatrix() const noexcept
// {
//     const auto lRight = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
//     const auto lUp = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
//     const auto lForward = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
//     const auto lEye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

//     mat4f lResult;

//     lResult.m00 = lRight.x;
//     lResult.m10 = lUp.x;
//     lResult.m20 = -lForward.x;
//     lResult.m30 = 0.0f;

//     lResult.m01 = lRight.y;
//     lResult.m11 = lUp.y;
//     lResult.m21 = -lForward.y;
//     lResult.m31 = 0.0f;

//     lResult.m02 = lRight.z;
//     lResult.m12 = lUp.z;
//     lResult.m22 = -lForward.z;
//     lResult.m32 = 0.0f;

//     lResult.m03 = -dot(lRight, lEye);
//     lResult.m13 = -dot(lUp, lEye);
//     lResult.m23 = dot(lForward, lEye);
//     lResult.m33 = 1.0f;

//     return lResult;
// }

// Entity::~Entity() noexcept
// {
//     try
//     {
//         onDie(this);
//     }
//     catch (...)
//     {
//         // Absorb exceptions.
//     }
//     // DEBUG_PRINT;
//     for (auto lChild : mChildren)
//     {
//         if (lChild.use_count()) // Not thread-safe :(
//         {
//             // DEBUG_PRINT;
//             // std::cerr << "\tChild refcount: " << lChild.use_count() <<
//             '\n';
//             // DEBUG_PRINT;
//             // std::cerr << "\t" << lChild->name << "\n";
//             // DEBUG_PRINT;
//             lChild->mParent = SharedPtr(nullptr);
//             // DEBUG_PRINT;
//             lChild->mGlobalTransform.decompose(lChild->mLocalTransform);
//             // DEBUG_PRINT;
//         }
//     }
//     // DEBUG_PRINT;
//     if (auto lParent = mParent.lock())
//     {
//         for (auto lIter = lParent->mChildren.begin();
//              lIter != lParent->mChildren.end(); ++lIter)
//         {
//             if (lIter->get() == this)
//             {
//                 lParent->mChildren.erase(lIter);
//                 return;
//             }
//         }
//     }
//     // If no lock can be acquired, the parent is dead.
//     // In that case we don't have to care about removing
//     // ourselves from the children list of the parent
//     // since that's gone.
// }

// std::shared_ptr<Entity> Entity::cloneRecursive() const
// {
//     // Invoke the copy constructor.
//     std::shared_ptr<Entity> lClone(new Entity(*this));
//     for (auto lChild : mChildren)
//     {
//         lClone->addChild(lChild->cloneRecursive());
//     }
//     return lClone;
// }
