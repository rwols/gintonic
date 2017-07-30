#include "Collider.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

using namespace gintonic;

Collider::Collider(const Kind kind, experimental::Entity* owner)
    : Component(kind, owner)
{
    mTransform = mOwner->template add<Transform>();
}
