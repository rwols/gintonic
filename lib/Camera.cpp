#include <gintonic/Component/Camera.hpp>

// #include <fbxsdk.h>

// namespace gintonic
// {

#define M_PIf2 static_cast<float>(M_PI) * 0.5f
#define M_2PIf static_cast<float>(M_PI) * 2.0f

using namespace gintonic;

void Camera::Camera(Entity& owner) : Component(owner) {}

void Camera::setProjectionKind(
    const gintonic::Camera::ProjectionKind kind) noexcept
{
    mProjectionKind = kind;
    mMatrixIsDirty = true;
}

void Camera::setClearColor(const vec4f& color) noexcept
{
    mClearColor = color;
    mMatrixIsDirty = true;
}

void Camera::setWidth(const float width) noexcept
{
    mWidth = width;
    mMatrixIsDirty = true;
}

void Camera::setHeight(const float height) noexcept
{
    mHeight = height;
    mMatrixIsDirty = true;
}

void Camera::setNearPlane(const float nearPlane) noexcept
{
    mNearPlane = nearPlane;
    mMatrixIsDirty = true;
}

void Camera::setFarPlane(const float farPlane) noexcept
{
    mFarPlane = farPlane;
    mMatrixIsDirty = true;
}

void Camera::setFieldOfViewInRadians(const float fieldOfViewInRadians) noexcept
{
    mFieldOfView = fieldOfViewInRadians;
    mMatrixIsDirty = true;
}

void Camera::setFieldOfViewInDegrees(const float fieldOfViewInDegrees) noexcept
{
    mFieldOfView = deg2rad(fieldOfViewInDegrees);
    mMatrixIsDirty = true;
}

const mat4f& Camera::getProjectionMatrix() const noexcept
{
    if (mMatrixIsDirty)
    {
        switch (mProjectionKind)
        {
        case ProjectionKind::Orthographic:
            mProjectionMatrix.set_orthographic(mWidth, mHeight, mNearPlane,
                                               mFarPlane);
            break;
        case ProjectionKind::Perspective:
            mProjectionMatrix.set_perspective(mFieldOfView, mWidth / mHeight,
                                              mNearPlane, mFarPlane);
            break;
        }
        mMatrixIsDirty = false;
    }
    return mProjectionMatrix;
}

std::unique_ptr<Component> void Camera::clone(Entity& newOwner) const
{
    auto camera = std::make_unique<Camera>(newOwner);
    camera->mWidth = mWidth;
    camera->mHeight = mHeight;
    camera->mNearPlane = mNearPlane;
    camera->mFarPlane = mFarPlane;
    camera->mFieldOfView = mFieldOfView;
    camera->mProjectionKind = mProjectionKind;
    camera->mClearColor = mClearColor;
    return camera;
}

// Camera::Camera(std::string inputName)
//     : Object<Camera, std::string>(std::move(inputName))
// {
//     mProjectionMatrixIsDirty = true;
// }

// Camera::Camera(const FbxCamera* pFbxCamera)
//     : Object<Camera, std::string>(pFbxCamera)
// {
//     mNearPlane = static_cast<float>(pFbxCamera->GetNearPlane());
//     mFarPlane = static_cast<float>(pFbxCamera->GetFarPlane());
//     switch (pFbxCamera->ProjectionType.Get())
//     {
//     case FbxCamera::ePerspective:
//         mProjection = kPerspective;
//         break;
//     case FbxCamera::eOrthogonal:
//         mProjection = kOrthographic;
//         break;
//     default:
//         mProjection = kPerspective;
//     }
//     // mFieldOfView =
//     // deg2rad(static_cast<float>(pFbxCamera->FieldOfView.Get()));
//     mFieldOfView = static_cast<float>(pFbxCamera->FieldOfView.Get());
//     mProjectionMatrixIsDirty = true;
// }

// void Camera::setWidth(const float width)
// {
//     mWidth = width;
//     mProjectionMatrixIsDirty = true;
// }

// void Camera::setHeight(const float height)
// {
//     mHeight = height;
//     mProjectionMatrixIsDirty = true;
// }

// void Camera::setNearPlane(const float nearPlane)
// {
//     mNearPlane = nearPlane;
//     mProjectionMatrixIsDirty = true;
// }

// void Camera::setFarPlane(const float farPlane)
// {
//     mFarPlane = farPlane;
//     mProjectionMatrixIsDirty = true;
// }

// void Camera::setFieldOfView(const float fieldOfView)
// {
//     mFieldOfView = fieldOfView;
//     mProjectionMatrixIsDirty = true;
// }

// void Camera::addMouse(const vec2f& mouseAngles) noexcept
// {
//     mAngles += mouseAngles;

//     mAngles.x = std::remainder(mAngles.x, M_2PIf);

//     if (mAngles.y < -M_PIf2)
//         mAngles.y = -M_PIf2;
//     else if (mAngles.y > M_PIf2)
//         mAngles.y = M_PIf2;
// }

// const mat4f& Camera::projectionMatrix() const noexcept
// {
//     if (mProjectionMatrixIsDirty)
//     {
//         switch (mProjection)
//         {
//         case kOrthographic:
//             mProjectionMatrix.set_orthographic(mWidth, mHeight, mNearPlane,
//                                                mFarPlane);
//             break;
//         case kPerspective:
//             mProjectionMatrix.set_perspective(mFieldOfView, mWidth / mHeight,
//                                               mNearPlane, mFarPlane);
//             break;
//         default:
//             mProjectionMatrix.set_perspective(mFieldOfView, mWidth / mHeight,
//                                               mNearPlane, mFarPlane);
//         }
//         mProjectionMatrixIsDirty = false;
//     }
//     return mProjectionMatrix;
// }

// } // namespace gintonic
