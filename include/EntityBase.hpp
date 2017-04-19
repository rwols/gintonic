#pragma once

#include "Casting.hpp"
#include <vector>

namespace gintonic
{

class Component;

class EntityBase
{
  public:
    enum class Kind : unsigned
    {
        Entity,
        Prefab
    };

    Kind getKind() const noexcept { return mKind; }

    /**
     * @brief Add a new Component to this Entity.
     * @tparam T A type which derives from Component.
     * @return The newly added Component.
     */
    template <class T> T* add();

    /**
     * @brief Remove a Component from this Entity.
     * @tparam T A type which derives from Component.
     * @return True if a Component subclassed from T was removed, false if not.
     */
    template <class T> bool remove();

    /**
     * @brief Get a Component.
     * @tparam T A type which derives from Component.
     * @return A pointer to the wanted Component, or nullptr if no such type is
     * present.
     */
    template <class T> T* get() noexcept;

    /**
     * @brief Get a Component.
     * @tparam T A type which derives from Component.
     * @return A pointer to the wanted Component, or nullptr if no such type is
     * present.
     */
    template <class T> const T* get() const noexcept;

  protected:
    EntityBase(const Kind kind);
    virtual ~EntityBase() = default;
    EntityBase(EntityBase&&);
    EntityBase(const EntityBase&);
    EntityBase& operator=(const EntityBase&);
    EntityBase& operator=(EntityBase&&);

    void update();
    void lateUpdate();

  private:
    const Kind mKind;
    std::vector<std::unique_ptr<Component>> mComponents;
    void clone(const EntityBase&);
    void assertSameType(const EntityBase&) const;
};

template <class T> T* EntityBase::add()
{
    auto t = new T(this);
    mComponents.emplace_back(t);
    return t;
}

template <class T> bool EntityBase::remove()
{
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        if (auto ptr = dynCast<T>(iter->get()))
        {
            mComponents.erase(iter);
            return true;
        }
    }
    return false;
}

template <class T> T* EntityBase::get() noexcept
{
    for (auto& comp : mComponents)
    {
        if (auto ptr = dynCast<T>(comp.get()))
        {
            return ptr;
        }
    }
    return nullptr;
}

template <class T> const T* EntityBase::get() const noexcept
{
    for (const auto& comp : mComponents)
    {
        if (const auto ptr = dynCast<const T>(comp.get()))
        {
            return ptr;
        }
    }
    return nullptr;
}

} // gintonic
