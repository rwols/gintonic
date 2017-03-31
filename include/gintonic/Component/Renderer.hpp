#pragma once

#include <boost/serialization/base_object.hpp>
#include <gintonic/Asset.hpp>
#include <gintonic/Component.hpp>
#include <vector>

#include <gintonic/nsbegin.hpp>

class Entity;
class Material;

class RendererComp : public Component
{
  public:
    std::vector<std::shared_ptr<Material>> materials;

    inline RendererComp(Entity& entity) : Component(entity) {}

    ~RendererComp() noexcept override = default;

    inline Component::Kind getKind() const noexcept override
    {
        return Component::Kind::RendererComp;
    }
    inline const char* getKindAsString() const noexcept override
    {
        return "RendererComp";
    }

  private:
    template <class Archive>
    void serialize(Archive& archive, const unsigned int /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component) &
            BOOST_SERIALIZATION_NVP(materials);
    }
};

#include <gintonic/nsend.hpp>
