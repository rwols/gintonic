#include "BoxCollider.hpp"
#include "Transform.hpp"

using namespace gintonic;

box3f BoxCollider::getGlobalBounds() const noexcept
{
    const auto pos = mTransform->getGlobalPosition() +
                     mTransform->global().apply_to_point(localOffset);
    return box3f(pos + localBounds.minCorner, pos + localBounds.maxCorner);
}
