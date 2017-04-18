#include "Behaviour.hpp"

using namespace gintonic;

std::unique_ptr<Component> Behaviour::clone(EntityBase* newOwner) const
{
    auto behaviour = std::make_unique<Behaviour>(newOwner);
    behaviour->script = script;
    return std::move(behaviour);
}

void Behaviour::update() { /*TODO*/}

void Behaviour::lateUpdate() { /*TODO*/}

void Behaviour::onParentChange() { /*TODO*/}
