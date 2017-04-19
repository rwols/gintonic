#include "Scene.hpp"

using namespace gintonic;

Scene::Scene(std::string name)
    : experimental::Asset<Scene>(std::forward<std::string>(name))
{
}

void Scene::update()
{
    for (auto& entity : entities) entity.update();
}

void Scene::lateUpate()
{
    for (auto& entity : entities) entity.lateUpdate();
}
