#pragma once

#include "Collider.hpp"
namespace gintonic
{

class BoxCollider : public Collider
{
    GT_COMPONENT_BOILERPLATE(BoxCollider, Collider);

  public:
    box3f localBounds;
    box3f getGlobalBounds() const noexcept override;

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

    static bool classOf(const Component* comp)
    {
        return comp->getKind() == Kind::BoxCollider;
    }

  protected:
    void onEnable() override;

  private:
    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collider) &
            BOOST_SERIALIZATION_NVP(localBounds);
    }
    std::unique_ptr<Component> clone(EntityBase* newOwner) const override;
};

} // namespace gintonic
