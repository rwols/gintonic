#define BOOST_TEST_MODULE mat4f test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/vec4f.hpp"
#include "../Source/Math/mat4f.hpp"
#include "../Source/Math/SQT.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

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