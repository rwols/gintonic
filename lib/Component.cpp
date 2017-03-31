#include <gintonic/Component.hpp>

using namespace gintonic;

Component::Component(Entity& holder) : mHolder(holder) {}

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
