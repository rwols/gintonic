#include "EntityBase.hpp"
#include "Component.hpp"

using namespace gintonic;

EntityBase::EntityBase(const Kind kind) : mKind(kind) {}

EntityBase::EntityBase(const EntityBase& other) : mKind(other.mKind)
{
    clone(other);
}

EntityBase::EntityBase(EntityBase&& other)
    : mKind(other.mKind), mComponents(std::move(other.mComponents))
{
}

EntityBase& EntityBase::operator=(const EntityBase& other)
{
    mComponents.clear();
    clone(other);
    return *this;
}

EntityBase& EntityBase::operator=(EntityBase&& other)
{
    assertSameType(other);
    mComponents = std::move(other.mComponents);
    return *this;
}

void EntityBase::clone(const EntityBase& other)
{
    assertSameType(other);
    for (const auto& ptr : other.mComponents)
    {
        mComponents.emplace_back(ptr->clone(this));
    }
}

void EntityBase::assertSameType(const EntityBase& other) const
{
    if (mKind != other.mKind)
    {
        throw std::logic_error("Copying entities of different derived types.");
    }
}
