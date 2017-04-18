#include "Collider.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

using namespace gintonic;

Collider::Collider(const Kind kind, EntityBase* owner) : Component(kind, owner)
{
    mTransform = mEntityBase->add<Transform>();
}
