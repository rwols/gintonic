#include "BoxCollider.hpp"
#include "Transform.hpp"

using namespace gintonic;

box3f BoxCollider::getGlobalBounds() const noexcept
{
    const auto pos = mTransform->getGlobalPosition() +
                     mTransform->global().apply_to_point(localOffset);
    return box3f(pos + localBounds.minCorner, pos + localBounds.maxCorner);
}

void BoxCollider::onEnable() { /* empty */}

std::unique_ptr<Component> BoxCollider::clone(EntityBase* newOwner) const
{
    auto boxcoll = std::make_unique<BoxCollider>(newOwner);
    boxcoll->localBounds = localBounds;
    return std::move(boxcoll);
}
