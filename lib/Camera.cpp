#include "Camera.hpp"

#include "Math/mat4f.hpp"

#include "Entity.hpp"

namespace gintonic
{

#define M_PIf2 static_cast<float>(M_PI) * 0.5f
#define M_2PIf static_cast<float>(M_PI) * 2.0f

Camera::Camera(std::string inputName)
    : Object<Camera, std::string>(std::move(inputName))
{
    mProjectionMatrixIsDirty = true;
}

void Camera::setWidth(const float width)
{
    mWidth = width;
    mProjectionMatrixIsDirty = true;
}

void Camera::setHeight(const float height)
{
    mHeight = height;
    mProjectionMatrixIsDirty = true;
}

void Camera::setNearPlane(const float nearPlane)
{
    mNearPlane = nearPlane;
    mProjectionMatrixIsDirty = true;
}

void Camera::setFarPlane(const float farPlane)
{
    mFarPlane = farPlane;
    mProjectionMatrixIsDirty = true;
}

void Camera::setFieldOfView(const float fieldOfView)
{
    mFieldOfView = fieldOfView;
    mProjectionMatrixIsDirty = true;
}

void Camera::addMouse(const vec2f& mouseAngles) noexcept
{
    mAngles += mouseAngles;

    mAngles.x = std::remainder(mAngles.x, M_2PIf);

    if (mAngles.y < -M_PIf2)
        mAngles.y = -M_PIf2;
    else if (mAngles.y > M_PIf2)
        mAngles.y = M_PIf2;
}

const mat4f& Camera::projectionMatrix() const noexcept
{
    if (mProjectionMatrixIsDirty)
    {
        switch (mProjection)
        {
        case kOrthographic:
            mProjectionMatrix.set_orthographic(mWidth, mHeight, mNearPlane,
                                               mFarPlane);
            break;
        case kPerspective:
            mProjectionMatrix.set_perspective(mFieldOfView, mWidth / mHeight,
                                              mNearPlane, mFarPlane);
            break;
        default:
            mProjectionMatrix.set_perspective(mFieldOfView, mWidth / mHeight,
                                              mNearPlane, mFarPlane);
        }
        mProjectionMatrixIsDirty = false;
    }
    return mProjectionMatrix;
}

} // namespace gintonic
