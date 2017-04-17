#include "Component.hpp"

using namespace gintonic;

Component::Component(const Kind kind, Entity& owner)
    : mKind(kind), entity(owner)
{
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
