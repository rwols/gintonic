/**
 * @file camera.hpp
 * @brief Defines a camera component.
 * @author Raoul Wols
 */

#pragma once

#include "ForwardDeclarations.hpp"
#include "Foundation/Object.hpp"
#include "Math/vec2f.hpp"
#include "Math/mat4f.hpp"

namespace gintonic {

/**
 * @brief Camera component class.
 */
class Camera : public Object<Camera, std::string>
{
public:

	Camera() = default;
	Camera(const FBX::FbxCamera*);

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

	inline float width() const noexcept
	{
		return mWidth;
	}

	inline float height() const noexcept
	{
		return mHeight;
	}

	inline float nearPlane() const noexcept
	{
		return mNearPlane;
	}

	inline float farPlane() const noexcept
	{
		return mFarPlane;
	}

	inline float fieldOfView() const noexcept
	{
		return mFieldOfView;
	}

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

	inline const vec2f& angles() const noexcept
	{
		return mAngles;
	}

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

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

};

} // namespace gintonic
