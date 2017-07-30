#include "Component.hpp"
#include "Entity.hpp"

using namespace gintonic;

Component::Component(const Kind kind) : mKind(kind) {}

Component::Component(const Kind kind, experimental::Entity* owner)
    : mOwner(owner), mKind(kind)
{
}

experimental::Entity& Component::getEntity() { return *mOwner; }

const experimental::Entity& Component::getEntity() const { return *mOwner; }

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
