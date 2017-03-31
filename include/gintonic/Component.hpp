#pragma once

#include <boost/serialization/nvp.hpp>
#include <memory>

/**
 * @brief      Boilerplate macro for load_construct_data and
 *             save_construct_data for use in a class declaration.
 *
 * @param      compname The name of the component that you are declaring.
 */
#define GT_COMPONENT_SERIALIZATION_BOILERPLATE(compname)                       \
  private:                                                                     \
    friend class boost::serialization::access;                                 \
    template <class Archive>                                                   \
    inline friend void load_construct_data(                                    \
        Archive& archive, compname* component, const unsigned int version)     \
    {                                                                          \
        Entity* entity_ptr;                                                    \
        archive >> BOOST_SERIALIZATION_NVP(entity_ptr);                        \
        new (component) compname(*entity_ptr);                                 \
    }                                                                          \
    template <class Archive>                                                   \
    inline friend void save_construct_data(Archive& archive,                   \
                                           const compname* component,          \
                                           const unsigned int version)         \
    {                                                                          \
        const auto entity_ptr = &component->entity;                            \
        archive << BOOST_SERIALIZATION_NVP(entity_ptr);                        \
    }

/**
 * @brief      Boilerplate macro for a new class derived from Component.
 *
 * @param      derivedcomp  The name of the new component class that you are
 *                          declaring.
 * @param      basecomp     The base component that you are deriving from.
 */
#define GT_COMPONENT_BOILERPLATE(derivedcomp, basecomp)                        \
  public:                                                                      \
    inline derivedcomp(Entity& entity) : basecomp(entity) {}                   \
    ~derivedcomp() noexcept override = default;                                \
    Component::Kind getKind() const noexcept override                          \
    {                                                                          \
        return Component::Kind::derivedcomp;                                   \
    }                                                                          \
    const char* getKindAsString() const noexcept override                      \
    {                                                                          \
        return #derivedcomp;                                                   \
    }                                                                          \
    GT_COMPONENT_SERIALIZATION_BOILERPLATE(derivedcomp);

#include <gintonic/nsbegin.hpp>

class Entity;

/**
 * @brief      Abstract base class for all component types.
 */
class Component
{
  public:
    enum class Kind : int
    {
        Camera = 0,
        OctreeComp,
        Collider,
        BoxCollider,
        RendererComp,
        MeshRenderer,
        Transform,
        Light,
        AmbientLight,
        DirectionalLight,
        PointLight,
        SpotLight,
        Behaviour,
        Count
    };

    /**
     * A reference to the Entity that owns this component.
     */
    Entity& entity;

    virtual ~Component() noexcept = default;

    /**
     * @brief      Get the kind of this component.
     *
     * @return     The kind.
     */
    virtual Kind getKind() const noexcept = 0;

    /**
     * @brief      Get the kind of this component as a string literal.
     *
     * @return     The kind as string literal.
     */
    virtual const char* getKindAsString() const noexcept = 0;

    inline bool isEnabled() const noexcept { return mEnabled; }
    virtual void setEnabled(const bool b);

  protected:
    Component(Entity& owner);

    virtual void update() { /* Empty. */}
    virtual void lateUpdate() { /* Empty. */}
    virtual void onParentChange() { /* Empty. */}
    virtual void onEnable() { /*Empty */}
    virtual void onDisable() { /* Empty */}

  private:
    bool mEnabled = false;
    friend class Entity; // for the update methods and cloning method.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& archive, const unsigned int version) const
    {
        archive& boost::serialization::make_nvp("enabled", mEnabled);
    }

    virtual std::unique_ptr<Component> clone(Entity& newOwner) const = 0;
};

#include <gintonic/nsend.hpp>

BOOST_CLASS_TRACKING(NAMESPACE_GINTONIC::Component,
                     boost::serialization::track_always);
