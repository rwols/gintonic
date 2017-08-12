#pragma once

#include "gintonic_export.h"
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
    friend class boost::serialization::access;

/**
 * @brief      Boilerplate macro for a new class derived from Component.
 *
 * @param      derivedcomp  The name of the new component class that you are
 *                          declaring.
 * @param      basecomp     The base component that you are deriving from.
 */
#define GT_COMPONENT_BOILERPLATE(derivedcomp, basecomp)                        \
  public:                                                                      \
    derivedcomp() : basecomp(Kind::derivedcomp) {}                             \
    derivedcomp(experimental::Entity* entity)                                  \
        : basecomp(Kind::derivedcomp, entity)                                  \
    {                                                                          \
    }                                                                          \
    ~derivedcomp() noexcept override = default;                                \
    GT_COMPONENT_SERIALIZATION_BOILERPLATE(derivedcomp);

namespace gintonic {

namespace experimental {
class Entity;
} // namespace experimental

/**
 * @brief      Abstract base class for all component types.
 */
class GINTONIC_EXPORT Component
{
  public:
    enum class Kind : unsigned
    {
        // These enums must be placed in such a way that the order defines a
        // valid topological sorting of the graph that represents the class
        // hierarchy.
        Camera = 0,       // *
        OctreeComp,       // *
        Collider,         // *
        BoxCollider,      // **
        RendererComp,     // *
        MeshRenderer,     // **
        Transform,        // *
        Light,            // *
        AmbientLight,     // **
        DirectionalLight, // ***
        PointLight,       // **
        SpotLight,        // ***
        Behaviour,        // *
        Count
    };

    // entity:
    //   prefab_original: "asdf"
    //   parent: ~
    //   children:
    //     - "asdf1"
    //     - "asdf2"
    //   modifications:
    //     - index: 0
    //       modification:
    //         - name: "mLocalOffset"
    //         - value:
    //             x: 5.0
    //             y: 3.0
    //             z: 0.0
    //

    /// \brief Get a reference to the Entity that owns this Component.
    experimental::Entity& getEntity();

    /// \brief Get a reference to the Entity that owns this Component.
    const experimental::Entity& getEntity() const;

    virtual ~Component() noexcept = default;

    /**
     * @brief      Get the kind of component.
     *
     * @return     The kind.
     */
    Kind getKind() const noexcept { return mKind; }

    inline bool  isEnabled() const noexcept { return mEnabled; }
    virtual void setEnabled(const bool b);

  protected:
    Component(const Kind kind);
    Component(const Kind kind, experimental::Entity* owner);
    experimental::Entity* mOwner;

    virtual void update() { /* Empty. */}
    virtual void lateUpdate() { /* Empty. */}
    virtual void onParentChange() { /* Empty. */}
    virtual void onEnable() { /*Empty */}
    virtual void onDisable() { /* Empty */}

  private:
    Kind mKind;
    bool mEnabled = false;
    friend class experimental::Entity; // for the clone method, setting the
                                       // owner, and for the update methods.
    friend class Entity;               // for the update methods.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive& boost::serialization::make_nvp("enabled", mEnabled);
    }

    virtual std::unique_ptr<Component>
    clone(experimental::Entity* newOwner) const = 0;
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Component, boost::serialization::track_always);
