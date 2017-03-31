#pragma once

#include <gintonic/Asset.hpp>
#include <gintonic/Entity.hpp>

#include <fbxsdk/fbxsdk_nsbegin.h>
class FbxScene;
#include <fbxsdk/fbxsdk_nsend.h>

#include <gintonic/nsbegin.hpp>

class Renderer;

class Scene : public Asset, public Entity
{
    GINTONIC_ASSET_BOILERPLATE(Scene, "scenes", ".scene");

  public:
    static Scene fromFbxScene(FBXSDK_NAMESPACE::FbxScene* scene);

    Scene(const char* name);
    Scene(const std::string& name);
    Scene(std::string&& name);

    Renderer* renderer = nullptr;

  private:
    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Asset) &
            BOOST_SERIALIZATION_BASE_OBJECT_NVP(Entity) &
            BOOST_SERIALIZATION_NVP(renderer);
    }
};

#include <gintonic/nsend.hpp>
