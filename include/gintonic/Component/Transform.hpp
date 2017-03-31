#pragma once

#include <boost/serialization/base_object.hpp>
#include <gintonic/Component.hpp>
#include <gintonic/Math/SQT.hpp>
#include <gintonic/Math/mat4f.hpp>

#include <gintonic/nsbegin.hpp>

class Transform : public Component
{
    GT_COMPONENT_BOILERPLATE(Transform, Component)

  public:
    const SQT& local() const noexcept;
    SQT& local() noexcept;

    const mat4f& global() const noexcept;

    const vec3f& getGlobalPosition() const noexcept;
    const quatf& getGlobalRotation() const noexcept;
    const vec3f& getGlobalScale() const noexcept;

    void setGlobalPosition(const vec3f& position) noexcept;
    void setGlobalRotation(const quatf& rotation) noexcept;
    void setGlobalScale(const vec3f& scale) noexcept;

    void update() override;
    void onParentChange() override;

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    SQT mLocal;
    mutable SQT mGlobal;
    mutable mat4f mGlobalMatrix;
    mutable bool mIsUpdated = false;
    void updateImpl() const noexcept;

    std::unique_ptr<Component> clone(Entity& newOwner) const override;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
        archive& boost::serialization::make_nvp("local", mLocal);
    }
};

#include <gintonic/nsend.hpp>
