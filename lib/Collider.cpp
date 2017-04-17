#include "Collider.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

using namespace gintonic;

Collider::Collider(const Kind kind, Entity& owner) : Component(kind, owner)
{
    mTransform = entity.add<Transform>();
}
