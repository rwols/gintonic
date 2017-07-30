#pragma once

#include "Asset.hpp"
#include "Component.hpp"
#include <boost/serialization/base_object.hpp>
#include <vector>

namespace gintonic {

class Entity;
class Material;

class RendererComp : public Component
{
  public:
    std::vector<std::shared_ptr<Material>> materials;

  protected:
    inline RendererComp(const Kind kind) : Component(kind) {}
    inline RendererComp(const Kind kind, experimental::Entity* owner)
        : Component(kind, owner)
    {
    }

  public:
    ~RendererComp() noexcept override = default;

  private:
    template <class Archive>
    void serialize(Archive& archive, const unsigned int /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component) &
            BOOST_SERIALIZATION_NVP(materials);
    }
};

} // namespace gintonic
