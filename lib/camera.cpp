#include "camera.hpp"
#include "mat4f.hpp"
#include "entity.hpp"

namespace gintonic {

#define M_PIf2 static_cast<float>(M_PI) * 0.5f
#define M_2PIf static_cast<float>(M_PI) * 2.0f

void camera::setWidth(const float width)
{
	mWidth = width;
	mProjectionMatrixIsDirty = true;
}

void camera::setHeight(const float height)
{
	mHeight = height;
	mProjectionMatrixIsDirty = true;
}

void camera::setNearplane(const float nearplane)
{
	mNearPlane = nearplane;
	mProjectionMatrixIsDirty = true;
}

void camera::setFarplane(const float farplane)
{
	mFarPlane = farplane;
	mProjectionMatrixIsDirty = true;
}

void camera::setFieldOfView(const float fieldOfView)
{
	mFieldOfView = fieldOfView;
	mProjectionMatrixIsDirty = true;
}

void camera::addMouse(const vec2f& mouseAngles) noexcept
{
	mAngles += mouseAngles;

	mAngles.x = std::remainder(mAngles.x, M_2PIf);

	if (mAngles.y < -M_PIf2) mAngles.y = -M_PIf2;
	else if (mAngles.y > M_PIf2) mAngles.y = M_PIf2;

	// for (auto e = begin(); e != end(); ++e) (*e)->set_rotation(quatf::mouse(mAngles));
}

const mat4f& camera::projectionMatrix() const noexcept
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