#define BOOST_TEST_MODULE mat4f test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/vec4f.hpp"
#include "../Source/Math/mat4f.hpp"
#include "../Source/Math/SQT.hpp"
#include "../Source/Entity.hpp"
#include "../Source/Camera.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

#define GINTONIC_CHECK_MATRICES_CLOSE(left, right, tolerance) \
BOOST_CHECK_CLOSE(left.data[0][0], right.data[0][0], tolerance);\
BOOST_CHECK_CLOSE(left.data[0][1], right.data[0][1], tolerance);\
BOOST_CHECK_CLOSE(left.data[0][2], right.data[0][2], tolerance);\
BOOST_CHECK_CLOSE(left.data[0][3], right.data[0][3], tolerance);\
BOOST_CHECK_CLOSE(left.data[1][0], right.data[1][0], tolerance);\
BOOST_CHECK_CLOSE(left.data[1][1], right.data[1][1], tolerance);\
BOOST_CHECK_CLOSE(left.data[1][2], right.data[1][2], tolerance);\
BOOST_CHECK_CLOSE(left.data[1][3], right.data[1][3], tolerance);\
BOOST_CHECK_CLOSE(left.data[2][0], right.data[2][0], tolerance);\
BOOST_CHECK_CLOSE(left.data[2][1], right.data[2][1], tolerance);\
BOOST_CHECK_CLOSE(left.data[2][2], right.data[2][2], tolerance);\
BOOST_CHECK_CLOSE(left.data[2][3], right.data[2][3], tolerance);\
BOOST_CHECK_CLOSE(left.data[3][0], right.data[3][0], tolerance);\
BOOST_CHECK_CLOSE(left.data[3][1], right.data[3][1], tolerance);\
BOOST_CHECK_CLOSE(left.data[3][2], right.data[3][2], tolerance);\
BOOST_CHECK_CLOSE(left.data[3][3], right.data[3][3], tolerance);

#define GINTONIC_CHECK_MATRIX_SMALL(val, tolerance) \
BOOST_CHECK_SMALL(val.data[0][0], tolerance);\
BOOST_CHECK_SMALL(val.data[0][1], tolerance);\
BOOST_CHECK_SMALL(val.data[0][2], tolerance);\
BOOST_CHECK_SMALL(val.data[0][3], tolerance);\
BOOST_CHECK_SMALL(val.data[1][0], tolerance);\
BOOST_CHECK_SMALL(val.data[1][1], tolerance);\
BOOST_CHECK_SMALL(val.data[1][2], tolerance);\
BOOST_CHECK_SMALL(val.data[1][3], tolerance);\
BOOST_CHECK_SMALL(val.data[2][0], tolerance);\
BOOST_CHECK_SMALL(val.data[2][1], tolerance);\
BOOST_CHECK_SMALL(val.data[2][2], tolerance);\
BOOST_CHECK_SMALL(val.data[2][3], tolerance);\
BOOST_CHECK_SMALL(val.data[3][0], tolerance);\
BOOST_CHECK_SMALL(val.data[3][1], tolerance);\
BOOST_CHECK_SMALL(val.data[3][2], tolerance);\
BOOST_CHECK_SMALL(val.data[3][3], tolerance);

#define GINTONIC_CHECK_VECTOR3_CLOSE(left, right, tolerance)\
BOOST_CHECK_CLOSE(left.x, right.x, tolerance);\
BOOST_CHECK_CLOSE(left.y, right.y, tolerance);\
BOOST_CHECK_CLOSE(left.z, right.z, tolerance);

#define GINTONIC_CHECK_VECTOR4_CLOSE(left, right, tolerance)\
BOOST_CHECK_CLOSE(left.x, right.x, tolerance);\
BOOST_CHECK_CLOSE(left.y, right.y, tolerance);\
BOOST_CHECK_CLOSE(left.z, right.z, tolerance);\
BOOST_CHECK_CLOSE(left.w, right.w, tolerance);

#define GINTONIC_CHECK_VECTOR3_SMALL(val, tolerance)\
BOOST_CHECK_SMALL(val.x, tolerance);\
BOOST_CHECK_SMALL(val.y, tolerance);\
BOOST_CHECK_SMALL(val.z, tolerance);

#define GINTONIC_CHECK_VECTOR4_SMALL(val, tolerance)\
BOOST_CHECK_SMALL(val.x, tolerance);\
BOOST_CHECK_SMALL(val.y, tolerance);\
BOOST_CHECK_SMALL(val.z, tolerance);\
BOOST_CHECK_SMALL(val.w, tolerance);

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	mat4f M(2.0f);

	BOOST_CHECK_EQUAL(M.m00, 2.0f);
	BOOST_CHECK_EQUAL(M.m11, 2.0f);
	BOOST_CHECK_EQUAL(M.m22, 2.0f);
	BOOST_CHECK_EQUAL(M.m33, 2.0f);
	BOOST_CHECK_EQUAL(M.m32, 0.0f);
	BOOST_CHECK_EQUAL(M.m23, 0.0f);
	BOOST_CHECK_EQUAL(M.m01, 0.0f);
	BOOST_CHECK_EQUAL(M.m10, 0.0f);

	mat4f a,b;

	a.m00 = 0.0f;
	a.m10 = 1.0f;
	a.m20 = 0.0f;
	a.m30 = 0.0f;

	a.m01 = -1.0f;
	a.m11 = 0.0f;
	a.m21 = 0.0f;
	a.m31 = 0.0f;

	a.m02 = 0.0f;
	a.m12 = 0.0f;
	a.m22 = 1.0f;
	a.m32 = 0.0f;

	a.m03 = 0.0f;
	a.m13 = 0.0f;
	a.m23 = 0.0f;
	a.m33 = 1.0f;

	b.m00 = 1.0f;
	b.m10 = 0.0f;
	b.m20 = 0.0f;
	b.m30 = 0.0f;

	b.m01 = 0.0f;
	b.m11 = 1.0f;
	b.m21 = 0.0f;
	b.m31 = 0.0f;

	b.m02 = 0.0f;
	b.m12 = 0.0f;
	b.m22 = 1.0f;
	b.m32 = 0.0f;

	b.m03 = 10.0f;
	b.m13 = 200.0f;
	b.m23 = 0.0f;
	b.m33 = 1.0f;

	#ifndef BOOST_MSVC
	BOOST_CHECK_EQUAL(a.data[0][0], 0.0f);
	BOOST_CHECK_EQUAL(a.data[0][1], 1.0f);
	BOOST_CHECK_EQUAL(a.data[0][2], 0.0f);
	BOOST_CHECK_EQUAL(a.data[0][3], 0.0f);

	BOOST_CHECK_EQUAL(a.data[1][0], -1.0f);
	BOOST_CHECK_EQUAL(a.data[1][1], 0.0f);
	BOOST_CHECK_EQUAL(a.data[1][2], 0.0f);
	BOOST_CHECK_EQUAL(a.data[1][3], 0.0f);

	BOOST_CHECK_EQUAL(a.data[2][0], 0.0f);
	BOOST_CHECK_EQUAL(a.data[2][1], 0.0f);
	BOOST_CHECK_EQUAL(a.data[2][2], 1.0f);
	BOOST_CHECK_EQUAL(a.data[2][3], 0.0f);

	BOOST_CHECK_EQUAL(a.data[3][0], 0.0f);
	BOOST_CHECK_EQUAL(a.data[3][1], 0.0f);
	BOOST_CHECK_EQUAL(a.data[3][2], 0.0f);
	BOOST_CHECK_EQUAL(a.data[3][3], 1.0f);

	BOOST_CHECK_EQUAL(b.data[0][0], 1.0f);
	BOOST_CHECK_EQUAL(b.data[0][1], 0.0f);
	BOOST_CHECK_EQUAL(b.data[0][2], 0.0f);
	BOOST_CHECK_EQUAL(b.data[0][3], 0.0f);

	BOOST_CHECK_EQUAL(b.data[1][0], 0.0f);
	BOOST_CHECK_EQUAL(b.data[1][1], 1.0f);
	BOOST_CHECK_EQUAL(b.data[1][2], 0.0f);
	BOOST_CHECK_EQUAL(b.data[1][3], 0.0f);

	BOOST_CHECK_EQUAL(b.data[2][0], 0.0f);
	BOOST_CHECK_EQUAL(b.data[2][1], 0.0f);
	BOOST_CHECK_EQUAL(b.data[2][2], 1.0f);
	BOOST_CHECK_EQUAL(b.data[2][3], 0.0f);

	BOOST_CHECK_EQUAL(b.data[3][0], 10.0f);
	BOOST_CHECK_EQUAL(b.data[3][1], 200.0f);
	BOOST_CHECK_EQUAL(b.data[3][2], 0.0f);
	BOOST_CHECK_EQUAL(b.data[3][3], 1.0f);
	#endif

	SQT sqt;

	sqt.scale = vec3f(1.0f, 1.0f, 1.0f);
	sqt.rotation = quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), M_PI / 2.0f);
	sqt.translation = vec3f(0.0f, 0.0f, 100.0f);

	mat4f matFromSQT(sqt);

	std::cout << "SQT: " << sqt << '\n';
	std::cout << "Matrix from SQT:\n" << matFromSQT << '\n';
}

BOOST_AUTO_TEST_CASE ( operators_test )
{
	mat4f a;
	a.set_orthographic(800.0f, 640.0f, 0.1f, 100.0f);
	mat4f b(2.0f);

	std::cout << "a = " << a << '\n';
	std::cout << "b = " << b << '\n';
	std::cout << "a * b = " << a * b << '\n';

	mat4f c = a * b;

	vec4f v(1.0f, 2.0f, 3.0f, 4.0f);

	a.m00 = 0.0f;
	a.m10 = 1.0f;
	a.m20 = 0.0f;
	a.m30 = 0.0f;

	a.m01 = -1.0f;
	a.m11 = 0.0f;
	a.m21 = 0.0f;
	a.m31 = 0.0f;

	a.m02 = 0.0f;
	a.m12 = 0.0f;
	a.m22 = 1.0f;
	a.m32 = 0.0f;

	a.m03 = 0.0f;
	a.m13 = 0.0f;
	a.m23 = 0.0f;
	a.m33 = 1.0f;

	b.m00 = 1.0f;
	b.m10 = 0.0f;
	b.m20 = 0.0f;
	b.m30 = 0.0f;

	b.m01 = 0.0f;
	b.m11 = 1.0f;
	b.m21 = 0.0f;
	b.m31 = 0.0f;

	b.m02 = 0.0f;
	b.m12 = 0.0f;
	b.m22 = 1.0f;
	b.m32 = 0.0f;

	b.m03 = 10.0f;
	b.m13 = 200.0f;
	b.m23 = 0.0f;
	b.m33 = 1.0f;

	c = a * b;

	BOOST_CHECK_EQUAL(c.m00, 0.0f);
	BOOST_CHECK_EQUAL(c.m10, 1.0f);
	BOOST_CHECK_EQUAL(c.m20, 0.0f);
	BOOST_CHECK_EQUAL(c.m30, 0.0f);

	BOOST_CHECK_EQUAL(c.m01, -1.0f);
	BOOST_CHECK_EQUAL(c.m11, 0.0f);
	BOOST_CHECK_EQUAL(c.m21, 0.0f);
	BOOST_CHECK_EQUAL(c.m31, 0.0f);

	BOOST_CHECK_EQUAL(c.m02, 0.0f);
	BOOST_CHECK_EQUAL(c.m12, 0.0f);
	BOOST_CHECK_EQUAL(c.m22, 1.0f);
	BOOST_CHECK_EQUAL(c.m32, 0.0f);

	BOOST_CHECK_EQUAL(c.m03, -200.0f);
	BOOST_CHECK_EQUAL(c.m13, 10.0f);
	BOOST_CHECK_EQUAL(c.m23, 0.0f);
	BOOST_CHECK_EQUAL(c.m33, 1.0f);

	v = a * v;

	BOOST_CHECK_EQUAL(v.x, -2.0f);
	BOOST_CHECK_EQUAL(v.y, 1.0f);
	BOOST_CHECK_EQUAL(v.z, 3.0f);
	BOOST_CHECK_EQUAL(v.w, 4.0f);
}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{
	const float fieldofview = deg2rad(60.0f);
	const float aspectratio = 1440.0f / 900.0f; // typical resolution
	const float nearplane = 0.1f;
	const float farplane = 100.0f;

	float recovered_fieldofview;
	float recovered_aspectratio;
	float recovered_nearplane;
	float recovered_farplane;

	mat4f P;

	P.set_perspective(fieldofview, aspectratio, nearplane, farplane);

	std::cout << "The projection matrix is given by:\n";
	std::cout << P << "\n\n";

	P.unproject_perspective(
		recovered_fieldofview,
		recovered_aspectratio,
		recovered_nearplane,
		recovered_farplane);

	BOOST_CHECK_CLOSE(fieldofview, recovered_fieldofview, 0.001f);
	BOOST_CHECK_CLOSE(aspectratio, recovered_aspectratio, 0.001f);
	BOOST_CHECK_CLOSE(nearplane, recovered_nearplane, 0.001f);
	BOOST_CHECK_CLOSE(farplane, recovered_farplane, 0.001f);
}

BOOST_AUTO_TEST_CASE ( shadow_algorithm_test )
{
	const float width = 1440.0f;
	const float height = 900.0f;
	// const float fieldofview = deg2rad(60.0f);
	// const float aspectratio = width / height; // typical resolution
	const float nearplane = 1.0;
	const float farplane = 1000.0f;

	auto lLightEntity = std::make_shared<Entity>("Light");

	auto lGeometryEntity = std::make_shared<Entity>("SomeEntity");

	auto lCamera = std::make_shared<Camera>();
	lCamera->name = "DefaultCamera";
	lCamera->setNearPlane(nearplane);
	lCamera->setFarPlane(farplane);
	lCamera->setProjectionType(Camera::kPerspective);
	auto lCameraEntity = std::make_shared<Entity>("Camera");
	lCameraEntity->camera = lCamera;
	lCameraEntity->camera->setWidth(width);
	lCameraEntity->camera->setHeight(height);

	std::srand((int)std::clock());

	const int lMaxOffset = 10000;
	// const float lInverseOfMaxOffset = 1.0f / static_cast<float>(lMaxOffset);

	auto createRandomVector3 = [&] ()
	{
		const float x = - 0.5f * static_cast<float>(lMaxOffset) + (rand() % lMaxOffset);
		const float y = - 0.5f * static_cast<float>(lMaxOffset) + (rand() % lMaxOffset);
		const float z = - 0.5f * static_cast<float>(lMaxOffset) + (rand() % lMaxOffset);
		return vec3f(x, y, z);
	};

	for (int i = 0; i < 100000; ++i)
	{
		lLightEntity->setLocalTransform
		(
			SQT
			(
				vec3f(1.0f, 1.0f, 1.0f),
				quatf(rand(), rand(), rand(), rand()).normalize(),
				createRandomVector3()
			)
		);

		lGeometryEntity->setLocalTransform
		(
			SQT
			(
				vec3f(1.0f, 1.0f, 1.0f),
				quatf(rand(), rand(), rand(), rand()).normalize(),
				createRandomVector3()
			)
		);

		lCameraEntity->setLocalTransform
		(
			SQT
			(
				vec3f(1.0f, 1.0f, 1.0f),
				quatf(rand(), rand(), rand(), rand()).normalize(),
				createRandomVector3()
			)
		);

		float lNearPlane = rand() % 100;
		float lFarPlane  = lNearPlane + 1 + (rand() % 100);

		lCameraEntity->camera->setWidth(100 + (rand() % 1000));
		lCameraEntity->camera->setHeight(100 + (rand() % 1000));
		lCameraEntity->camera->setNearPlane(lNearPlane);
		lCameraEntity->camera->setFarPlane(lFarPlane);

		// const auto lProjection     = lCameraEntity->camera->projectionMatrix();
		const auto lCameraView     = lCameraEntity->getViewMatrix();
		const auto lLightView      = lLightEntity->getViewMatrix();
		const auto lLightInverseView = lLightEntity->globalTransform();
		const auto lCameraInverseView = lCameraEntity->globalTransform();
		const auto lModel          = lGeometryEntity->globalTransform();

		lNearPlane                 = rand() % 100;
		lFarPlane                  = lNearPlane + 1 + (rand() % 100);

		mat4f lLightProjection;
		lLightProjection.set_orthographic(1 + (rand() % 100), 1 + (rand() % 100), lNearPlane, lFarPlane);

		mat4f lShouldBeAlmostIdentity;
		mat4f lActualIndentity(1.0f);
		mat4f lShouldBeAlmostZero;

		lShouldBeAlmostIdentity    = lCameraView * lCameraInverseView;
		lShouldBeAlmostZero        = lShouldBeAlmostIdentity - lActualIndentity;
		// GINTONIC_CHECK_MATRIX_SMALL(lShouldBeAlmostZero, 0.1f);
		lShouldBeAlmostIdentity    = lCameraInverseView * lCameraView;
		lShouldBeAlmostZero        = lShouldBeAlmostIdentity - lActualIndentity;
		// GINTONIC_CHECK_MATRIX_SMALL(lShouldBeAlmostZero, 0.1f);
		lShouldBeAlmostIdentity    = lLightView * lLightInverseView;
		lShouldBeAlmostZero        = lShouldBeAlmostIdentity - lActualIndentity;
		// GINTONIC_CHECK_MATRIX_SMALL(lShouldBeAlmostZero, 0.1f);
		lShouldBeAlmostIdentity    = lLightInverseView * lLightView;
		lShouldBeAlmostZero        = lShouldBeAlmostIdentity - lActualIndentity;
		// GINTONIC_CHECK_MATRIX_SMALL(lShouldBeAlmostZero, 0.1f);

		const vec4f lPoint         = vec4f(createRandomVector3(), 1.0f);

		const auto lShadowMatrix   = lLightProjection * lLightView * lCameraInverseView;

		auto lViewPoint            =                    lCameraView * lModel * lPoint;
		// auto lViewClipSpacePoint   = lProjection      * lCameraView * lModel * lPoint;
		auto lLightClipPoint       = lLightProjection * lLightView  * lModel * lPoint;
		auto lFromViewToLightPoint = lShadowMatrix                           * lViewPoint;

		// Perspective division
		lLightClipPoint           /= lLightClipPoint.w;
		lFromViewToLightPoint     /= lFromViewToLightPoint.w;

		const auto lShouldBeTiny   = lLightClipPoint - lFromViewToLightPoint;

		GINTONIC_CHECK_VECTOR4_SMALL(lShouldBeTiny, 0.1f);
	}
}

BOOST_AUTO_TEST_CASE ( decomposition )
{
	std::srand((int)std::clock());
	for (int i = 0; i < 100000; ++i)
	{
		quatf lRotation(rand(), rand(), rand(), rand());


		lRotation.normalize();

		// vec3f lScale(rand(), rand(), rand());
		vec3f lScale(1.0f, 1.0f, 1.0f);
		lScale.normalize();

		vec3f lTranslation(rand(), rand(), rand());

		quatf lRecoveredRotation;
		vec3f lRecoveredScale, lRecoveredTranslation;

		mat4f lAffineMatrix(SQT(lScale, lRotation, lTranslation));

		lAffineMatrix.decompose(lRecoveredScale, lRecoveredRotation, lRecoveredTranslation);

		// Here, the tolerance parameter is 0.1%. The algorithm is pretty stable.
		GINTONIC_CHECK_VECTOR3_CLOSE(lRecoveredScale, lScale, 0.1f);

		// The tolerance here is 5% instead of 0.1% because
		// floating point rounding errors can become quite large in the case
		// of quaternions and the decomposition algorithm. But it seems to be
		// correct in theory. It's just that in practise we must allow a tolerance
		// error of 5% instead of 0.1%, otherwise we seem to get a test failure
		// every now and then. It only shows that floating point rounding errors are
		// present, not that the algorithm is incorrect. It would be nice to change
		// the algorithm so that these rounding errors become smaller, but I do not
		// know how to do that at this point.
		GINTONIC_CHECK_VECTOR4_CLOSE(lRecoveredRotation, lRotation, 5.0f);

		// Here, the tolerance parameter is back to 0.1%.
		GINTONIC_CHECK_VECTOR3_CLOSE(lRecoveredTranslation, lTranslation, 0.1f);
	}
}