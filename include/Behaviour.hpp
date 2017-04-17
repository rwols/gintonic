#pragma once

#include "Component.hpp"

namespace gintonic
{

class Script;

class Behaviour : public Component
{
    GT_COMPONENT_BOILERPLATE(Behaviour, Component);

  public:
    std::shared_ptr<Script> script;

  protected:
    void update() override;
    void lateUpdate() override;
    void onParentChange() override;

  private:
    std::unique_ptr<Component> clone(Entity& newOwner) const override;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component) &
            BOOST_SERIALIZATION_NVP(script);
    }
};

} // gintonic
