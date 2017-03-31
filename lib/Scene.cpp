#include <fbxsdk.h>
#include <gintonic/Scene.hpp>

using namespace gintonic;

Scene Scene::fromFbxScene(FbxScene* theirScene)
{
    Scene ourScene(theirScene->GetName());
    return ourScene;
}

Scene::Scene(const char* name) : Asset(name), Entity(name) {}

Scene::Scene(const std::string& name) : Asset(name), Entity(name) {}

Scene::Scene(std::string&& name) : Asset(name), Entity(std::move(name)) {}
