#pragma once

#include "Asset.hpp"
#include "Entity.hpp"
#include <memory>

namespace gintonic
{

class Prefab : public experimental::Asset<Prefab>,
               public EntityBase,
               private std::enable_shared_from_this<Prefab>
{

  public:
    static const char* extension() noexcept { return ".prefab"; }
    static const char* prefixFolder() noexcept { return "prefabs"; }
    Prefab(std::string name);
    ~Prefab() override final;

    /// \brief Instantiate a new Entity from this Prefab.
    experimental::Entity instantiate() const;

    static bool classOf(const EntityBase* ent)
    {
        return ent->getKind() == Kind::Prefab;
    }

  private:
    std::vector<std::shared_ptr<Prefab>> mChildren;
    std::weak_ptr<Prefab> mParent;
};

} // gintonic
