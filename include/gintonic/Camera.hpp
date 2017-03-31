/**
 * @file Camera.hpp
 * @brief Defines a camera component.
 * @author Raoul Wols
 */

#pragma once

#include "Foundation/Object.hpp"
#include "Math/mat4f.hpp"
#include "Math/vec2f.hpp"
#include <gintonic/ForwardDeclarations.hpp>

namespace gintonic
{

/**
 * @brief Camera component class.
 */
class Camera : public Object<Camera, std::string>
{
  public:
    template <class... Args> inline static SharedPtr create(Args&&... args)
    {
        return SharedPtr(new Camera(std::forward<Args>(args)...));
    }

  private:
    Camera() = default;
    Camera(std::string name);
    Camera(const FBXSDK_NAMESPACE::FbxCamera*);

  public:
    virtual ~Camera() noexcept = default;

    /// Projection type. Either orthographic or perspective.
    enum ProjectionType
    {
        kOrthographic,
        kPerspective
    };

    void setWidth(const float width);

    void setHeight(const float height);

    void setNearPlane(const float nearPlane);

    void setFarPlane(const float farPlane);

    void setFieldOfView(const float fieldOfView);

    inline float width() const noexcept { return mWidth; }

    inline float height() const noexcept { return mHeight; }

    inline float nearPlane() const noexcept { return mNearPlane; }

    inline float farPlane() const noexcept { return mFarPlane; }

    inline float fieldOfView() const noexcept { return mFieldOfView; }

    inline ProjectionType projectionType() const noexcept
    {
        return mProjection;
    }

    void setProjectionType(const ProjectionType type)
    {
        mProjection = type;
        mProjectionMatrixIsDirty = true;
    }

    /**
     * @brief Add mouse angles.
     * @param mouseAngles New mouse angles.
     */
    void addMouse(const vec2f& mouseAngles) noexcept;

    const mat4f& projectionMatrix() const noexcept;

    inline const vec2f& angles() const noexcept { return mAngles; }

    /**
     * @brief Set the angle explicitly.
     * @param angles The angles.
     */
    inline void setAngles(const vec2f& angles) noexcept { mAngles = angles; }

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    mutable bool mProjectionMatrixIsDirty = true;

    ProjectionType mProjection = kPerspective;

    float mFieldOfView = 1.22173048f;

    float mWidth;

    float mHeight;

    float mNearPlane;

    float mFarPlane;

    mutable mat4f mProjectionMatrix;

    vec2f mAngles;

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& archive, const unsigned int /*version*/)
    {
        archive& mProjectionMatrixIsDirty& mProjection& mFieldOfView& mWidth&
            mHeight& mNearPlane& mProjectionMatrix& mAngles;
    }
};

} // namespace gintonic
