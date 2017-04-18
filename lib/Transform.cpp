#include "Transform.hpp"
#include "Entity.hpp"

using namespace gintonic;

std::unique_ptr<Component> Transform::clone(EntityBase* newOwner) const
{
    auto transform = std::make_unique<Transform>(newOwner);
    transform->local() = local();
    return std::move(transform);
}

void Transform::onParentChange() { mIsUpdated = false; }

const SQT& Transform::local() const noexcept { return mLocal; }

SQT& Transform::local() noexcept
{
    mIsUpdated = false;
    return mLocal;
}

const mat4f& Transform::global() const noexcept
{
    updateImpl();
    return mGlobalMatrix;
}

const vec3f& Transform::getGlobalPosition() const noexcept
{
    updateImpl();
    return mGlobal.translation;
}

const quatf& Transform::getGlobalRotation() const noexcept
{
    updateImpl();
    return mGlobal.rotation;
}

const vec3f& Transform::getGlobalScale() const noexcept
{
    updateImpl();
    return mGlobal.scale;
}

void Transform::setGlobalPosition(const vec3f& position) noexcept
{
    // TODO!!! INCORRECT!
    SQT sqt;
    global().decompose(sqt);
    local().translation = sqt.translation;
}

void Transform::setGlobalRotation(const quatf& rotation) noexcept
{
    // TODO!!! INCORRECT!
    SQT sqt;
    global().decompose(sqt);
    local().rotation = sqt.rotation;
}

void Transform::setGlobalScale(const vec3f& scale) noexcept
{
    // TODO!!! INCORRECT!
    SQT sqt;
    global().decompose(sqt);
    local().scale = sqt.scale;
}

void Transform::updateImpl() const noexcept
{
    if (mIsUpdated)
    {
        return;
    }
    else if (auto ptr = getEntity().getParent())
    {
        if (const auto parentTransform = ptr->get<Transform>())
        {
            mGlobalMatrix = parentTransform->global() * mat4f(mLocal);
        }
        else
        {
            mGlobalMatrix = mat4f(mLocal);
        }
    }
    else
    {
        mGlobalMatrix = mat4f(mLocal);
    }
    mGlobalMatrix.decompose(mGlobal);
    mIsUpdated = true;
}

void Transform::update() { updateImpl(); }
