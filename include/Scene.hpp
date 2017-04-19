#pragma once

#include "Asset.hpp"
#include "Entity.hpp"

namespace gintonic
{

class PrefabScene : public experimental::Asset<PrefabScene>
{
  public:
    PrefabScene(std::string name);
    static const char* extension() { return ".scene"; }
    static const char* prefixFolder() { return "scenes"; }

    Scene instantiate();

    std::vector<std::unique_ptr<EntityBase>> entities;
};

class Scene : public experimental::Asset<Scene>
{
  public:
    Scene(std::string name);
    static const char* extension() { return ".scene"; }
    static const char* prefixFolder() { return "scenes"; }
    void update();
    void lateUpate();
    std::vector<experimental::Entity> entities;

  private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& archive, const unsigned int /*version*/)
    {
        using boost::serialization::base_object;
        using boost::serialization::make_nvp;
        archive& make_nvp("Asset",
                          base_object<experimental::Asset<Scene>>(*this)) &
            make_nvp("entities", entities);
    }
};

} // gintonic
