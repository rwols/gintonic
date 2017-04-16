#pragma once

#include "Entity.hpp"

namespace gintonic
{

class Scene
{
  public:
    void update();
    void lateUpate();
    std::vector<Entity> entities;
};

} // gintonic
