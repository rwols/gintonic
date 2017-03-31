#pragma once

#include <gintonic/Component/Renderer.hpp>

#include <gintonic/nsbegin.hpp>

class Mesh;

class MeshRenderer : public RendererComp
{
    GT_COMPONENT_BOILERPLATE(MeshRenderer, RendererComp)

  public:
    std::shared_ptr<Mesh> mesh;

    void lateUpdate() override;

  private:
    std::unique_ptr<Component> clone(Entity& newOwner) const override;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(RendererComp) &
            BOOST_SERIALIZATION_NVP(mesh);
    }
};

#include <gintonic/nsend.hpp>
