#include "Component.hpp"
#include "Entity.hpp"
#include "Prefab.hpp"

using namespace gintonic;

Component::Component(const Kind kind, EntityBase* owner)
    : mEntityBase(owner), mKind(kind)
{
}

experimental::Entity& Component::getEntity()
{
    return *static_cast<experimental::Entity*>(mEntityBase);
}

const experimental::Entity& Component::getEntity() const
{
    return *static_cast<const experimental::Entity*>(mEntityBase);
}

Prefab& Component::getPrefab() { return *static_cast<Prefab*>(mEntityBase); }

const Prefab& Component::getPrefab() const
{
    return *static_cast<const Prefab*>(mEntityBase);
}

void Component::setEnabled(const bool b)
{
    if (b && !mEnabled)
    {
        mEnabled = b;
        onEnable();
    }
    else if (!b && mEnabled)
    {
        mEnabled = b;
        onDisable();
    }
}
