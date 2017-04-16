#include "Scene.hpp"

using namespace gintonic;

void Scene::update()
{
    for (auto& entity : entities) entity.update();
}

void Scene::lateUpate()
{
    for (auto& entity : entities) entity.lateUpdate();
}
