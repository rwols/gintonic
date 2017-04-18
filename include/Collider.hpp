#pragma once

#include "Component.hpp"
#include "Math/box3f.hpp"

namespace gintonic
{

class Transform;

class Collider : public Component
{
  protected:
    Collider(const Kind kind, EntityBase* owner);

  public:
    ~Collider() noexcept override = default;
    virtual box3f getGlobalBounds() const noexcept = 0;
    vec3f localOffset;

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  protected:
    Transform* mTransform = nullptr;

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component) &
            BOOST_SERIALIZATION_NVP(localOffset);
    }
};

} // gintonic
