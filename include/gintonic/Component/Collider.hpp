#pragma once

#include <gintonic/Component.hpp>
#include <gintonic/Math/box3f.hpp>

#include <gintonic/nsbegin.hpp>

class Transform;

class Collider : public Component
{
  public:
    Collider(Entity& entity);
    ~Collider() noexcept override = default;
    Component::Kind getKind() const noexcept override
    {
        return Component::Kind::Collider;
    }
    const char* getKindAsString() const noexcept override { return "Collider"; }
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

#include <gintonic/nsend.hpp>
