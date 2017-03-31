#pragma once

#include <gintonic/Component.hpp>
#include <gintonic/Math/mat4f.hpp>
#include <gintonic/Math/vec4f.hpp>

#include <gintonic/nsbegin.hpp>

class Camera : public Component
{
    GT_COMPONENT_BOILERPLATE(Camera, Component)

  public:
    enum class ProjectionKind : unsigned char
    {
        Orthographic,
        Perspective
    };

    void setProjectionKind(const ProjectionKind kind) noexcept;
    void setClearColor(const vec4f& color) noexcept;
    void setWidth(const float width) noexcept;
    void setHeight(const float height) noexcept;
    void setNearPlane(const float nearPlane) noexcept;
    void setFarPlane(const float farPlane) noexcept;
    void setFieldOfViewInRadians(const float fieldOfViewInRadians) noexcept;
    void setFieldOfViewInDegrees(const float fieldOfViewInDegrees) noexcept;

    const mat4f& getProjectionMatrix() const noexcept;
    ProjectionKind getProjectionKind() const noexcept
    {
        return mProjectionKind;
    }
    inline vec4f getClearColor() const noexcept { return mClearColor; }
    inline float getWidth() const noexcept { return mWidth; }
    inline float getHeight() const noexcept { return mHeight; }
    inline float getNearPlane() const noexcept { return mNearPlane; }
    inline float getFarPlane() const noexcept { return mFarPlane; }
    inline float getFieldOfViewInRadians() const noexcept
    {
        return mFieldOfView;
    }
    inline float getFieldOfViewInDegrees() const noexcept
    {
        return rad2deg(mFieldOfView);
    }

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    float mWidth = 1.0f;
    float mHeight = 1.0f;
    float mNearPlane = 0.1f;
    float mFarPlane = 100.0f;
    float mFieldOfView = 1.57079632679f; // pi/2
    ProjectionKind mProjectionKind = ProjectionKind::Perspective;
    mutable bool mMatrixIsDirty = true;
    vec4f mClearColor = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    mutable mat4f mProjectionMatrix;

    std::unique_ptr<Component> clone(Entity& newOwner) const override;

    template <class Archive>
    void serialize(Archive& archive, const unsigned int /*version*/)
    {
        using namespace boost::serialization;
        archive& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component) &
            make_nvp("width", mWidth) & make_nvp("height", mHeight) &
            make_nvp("near_plane", mNearPlane) &
            make_nvp("far_plane", mFarPlane) &
            make_nvp("field_of_view", mFieldOfView) &
            make_nvp("clear_color", mClearColor);
    }
};

#include <gintonic/nsend.hpp>
