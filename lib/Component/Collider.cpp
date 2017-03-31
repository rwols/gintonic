#include <gintonic/Component/Collider.hpp>
#include <gintonic/Component/Transform.hpp>
#include <gintonic/Entity.hpp>

using namespace gintonic;

Collider::Collider(Entity& owner) : Component(owner)
{
    mTransform = entity.add<Transform>();
}
