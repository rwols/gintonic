#pragma once

#include <gintonic/Component/Collider.hpp>

#include <gintonic/nsbegin.hpp>

class BoxCollider : public Collider
{
    GT_COMPONENT_BOILERPLATE(BoxCollider, Collider);

  public:
    box3f localBounds;
    box3f getGlobalBounds() const noexcept override;

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  protected:
    void onEnable() override;

  private:
    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collider) &
            BOOST_SERIALIZATION_NVP(localBounds);
    }
    std::unique_ptr<Component> clone(Entity& newOwner) const override;
};

#include <gintonic/nsend.hpp>
