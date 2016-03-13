#include "camera.hpp"
#include "Math/mat4f.hpp"
#include "entity.hpp"

namespace gintonic {

#define M_PIf2 static_cast<float>(M_PI) * 0.5f
#define M_2PIf static_cast<float>(M_PI) * 2.0f

Camera::Camera(const FbxCamera* pFbxCamera)
: Object<Camera, std::string>(pFbxCamera)
{
	// auto lGlobalName = boost::filesystem::path(pFbxCamera->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
	// if (std::strcmp(pFbxCamera->GetName(), "") == 0)
	// {
	// 	setName(std::move(lGlobalName), pFbxCamera->GetName());
	// }
	// else
	// {
	// 	setName(std::move(lGlobalName), pFbxCamera->GetNode()->GetName());
	// }
	
	mNearPlane = static_cast<float>(pFbxCamera->GetNearPlane());
	mFarPlane = static_cast<float>(pFbxCamera->GetFarPlane());
	if (pFbxCamera->ProjectionType.Get() == FbxCamera::ePerspective) mProjection = kPerspectiveProjection;
	switch (pFbxCamera->ProjectionType.Get())
	{
		case FbxCamera::ePerspective:
			mProjection = kPerspectiveProjection;
			break;
		case FbxCamera::eOrthogonal:
			mProjection = kOrthographicProjection;
			break;
		default:
			mProjection = kPerspectiveProjection;
	}
	mFieldOfView = deg2rad(static_cast<float>(pFbxCamera->FieldOfView.Get()));
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

	if (mAngles.y < -M_PIf2) mAngles.y = -M_PIf2;
	else if (mAngles.y > M_PIf2) mAngles.y = M_PIf2;

	// for (auto e = begin(); e != end(); ++e) (*e)->set_rotation(quatf::mouse(mAngles));
}

const mat4f& Camera::projectionMatrix() const noexcept
{
	if (mProjectionMatrixIsDirty)
	{
		switch (mProjection)
		{
			case kOrthographicProjection:
				mProjectionMatrix.set_orthographic(
					mWidth, 
					mHeight, 
					mNearPlane,
					mFarPlane);
				break;
			case kPerspectiveProjection:
				mProjectionMatrix.set_perspective(
					mFieldOfView, 
					mWidth / mHeight, 
					mNearPlane, 
					mFarPlane);
				break;
			default:
				mProjectionMatrix.set_perspective(
					mFieldOfView, 
					mWidth / mHeight, 
					mNearPlane, 
					mFarPlane);
		}
		mProjectionMatrixIsDirty = false;
	}
	return mProjectionMatrix;
}

} // namespace gintonic