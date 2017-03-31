/**
 * @file Entity.hpp
 * @brief Defines the Entity class.
 * @author Raoul Wols
 */

#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>
#include <string>
#include <vector>

#include <fbxsdk/fbxsdk_nsbegin.h>
class FbxNode;
#include <fbxsdk/fbxsdk_nsend.h>

#include <gintonic/nsbegin.hpp>

class Component;

/**
 * @brief Represents an Entity in the world. An Entity can only be
 * instantiated on the heap, using the static method Entity::create.
 * This method is (variadic) templated, simply calling an appropriate
 * Entity constructor. It returns a shared pointer to an Entity.
 * An Entity carries various components, mostly via shared pointers.
 * Examples of components are Material, Mesh and Light.
 */
class Entity
{
  public:
    /// The type of datastructure for the list of children.
    using ChildrenDatastructureType = std::vector<Entity>;

    /// The iterator type for the list of children.
    using iterator = ChildrenDatastructureType::iterator;

    /// The constant iterator type for the list of children.
    using const_iterator = ChildrenDatastructureType::const_iterator;

    using ComponentDatastructureType = std::vector<std::unique_ptr<Component>>;

    using ParentType = Entity*;

  private:
    ComponentDatastructureType mComponents;
    ChildrenDatastructureType mChildren;
    ParentType mParent = nullptr;

  public:
    std::string name = "Entity";

    template <class TComp> TComp* get() noexcept
    {
        for (auto& comp : mComponents)
        {
            if (auto* ptr = dynamic_cast<TComp*>(comp.get()))
            {
                return ptr;
            }
        }
        return nullptr;
    }

    template <class TComp> const TComp* get() const noexcept
    {
        for (const auto& comp : mComponents)
        {
            if (const auto* ptr = dynamic_cast<TComp*>(comp.get()))
            {
                return ptr;
            }
        }
        return nullptr;
    }

    template <class TComp> TComp* add()
    {
        auto comp = new TComp(*this);
        mComponents.emplace_back(comp);
        return comp;
    }

    template <class TComp> bool remove()
    {
        for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
        {
            if (dynamic_cast<TComp*>(iter->get()))
            {
                mComponents.erase(iter);
                return true;
            }
        }
        return false;
    }

    /**
     * @name Constructors and destructor
     */

    //@{

    Entity() = default;

    Entity(std::string name);

    Entity(const FBXSDK_NAMESPACE::FbxNode* pFbxNode);

    Entity(const Entity& other);

    Entity(Entity&& other) noexcept = default;

    Entity& operator=(const Entity& other);

    Entity& operator=(Entity&& other) noexcept = default;

    //@}

    /**
     * @name Updaters
     */

    void update();
    void lateUpdate();

  private:
    void onParentChange();

  public:
    /**
     * @name Children entities and parent Entity management
     */

    //@{

    void addChild(const Entity& child);
    void addChild(Entity&& child);
    void removeChild(const Entity& child);
    void removeChild(iterator iter) noexcept;

    void moveChild(Entity& oldParent, Entity& child);

    void moveChild(Entity& oldParent, iterator child);

    inline ParentType parent() noexcept { return mParent; }
    inline const ParentType parent() const noexcept { return mParent; }

    /**
     * @brief Get an iterator to the beginning of children list.
     * @return An iterator to the beginning of the children list.
     */
    inline iterator begin() noexcept { return mChildren.begin(); }

    /**
     * @brief Get an iterator to one-past-the-end of children list.
     * @return An iterator to one-past-the-end of the children list.
     */
    inline iterator end() noexcept { return mChildren.end(); }

    /**
     * @brief Get a const iterator to the beginning of children list.
     * @return A const iterator to the beginning of the children list.
     */
    inline const_iterator begin() const noexcept { return mChildren.begin(); }

    /**
     * @brief Get a const iterator to one-past-the-end of children list.
     * @return a const iterator to one-past-the-end of the children list.
     */
    inline const_iterator end() const noexcept { return mChildren.end(); }

    /**
     * @brief Get a const iterator to the beginning of children list.
     * @return A const iterator to the beginning of the children list.
     */
    inline const_iterator cbegin() const noexcept { return mChildren.cbegin(); }

    /**
     * @brief Get a const iterator to one-past-the-end of children list.
     * @return a const iterator to one-past-the-end of the children list.
     */
    inline const_iterator cend() const noexcept { return mChildren.cend(); }

    //@}

    /**
     * @name Comparison operators
     */

    //@{

    /**
     * @brief Two entities compare equal if they live at the same memory
     * address.
     * @param other Another Entity.
     * @return True if they live at the same memory address, false otherwise.
     */
    inline bool operator==(const Entity& other) const noexcept
    {
        return this == &other;
    }

    /**
     * @brief Two entities compare unequal if they don't live at the same
     * memory address.
     * @param other Another Entity.
     * @return True if they live don't at the same memory address,
     * false otherwise.
     */
    inline bool operator!=(const Entity& other) const noexcept
    {
        return !operator==(other);
    }

    /**
     * @brief Compares less via the memory addresses.
     * @param other Another Entity.
     * @return True if this Entity's memory address is less than the other
     * Entity's memory address. False otherwise.
     */
    inline bool operator<(const Entity& other) const noexcept
    {
        return this < &other;
    }

    /**
     * @brief Compares greater-than-or-equal via the memory addresses.
     * @param other Another Entity.
     * @return True if this Entity's memory address is greater than or equal
     * to the other Entity's memory address. False otherwise.
     */
    inline bool operator>=(const Entity& other) const noexcept
    {
        return !operator<(other);
    }

    /**
     * @brief Compares greater-than via the memory addresses.
     * @param other Another Entity.
     * @return True if this Entity's memory address is greater than the other
     * Entity's memory address. False otherwise.
     */
    inline bool operator>(const Entity& other) const noexcept
    {
        return this > &other;
    }

    /**
     * @brief Compares less-than-or-equal-to via the memory addresses.
     * @param other Another Entity.
     * @return True if this Entity's memory address is less than or equal to
     * the other Entity's memory address. False otherwise.
     */
    inline bool operator<=(const Entity& other) const noexcept
    {
        return !operator>(other);
    }

    //@}

  private:
    friend boost::serialization::access;

    template <class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        using namespace boost::serialization;
        archive& make_nvp("name", name) & make_nvp("components", mComponents) &
            make_nvp("parent", mParent) & make_nvp("children", mChildren);
    }
};

#include <gintonic/nsend.hpp>

// BOOST_CLASS_TRACKING(gintonic::Entity, boost::serialization::track_always);
