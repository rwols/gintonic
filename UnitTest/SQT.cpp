#define BOOST_TEST_MODULE SQT test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/SQT.hpp"
#include "../Source/Math/mat4f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{

}

BOOST_AUTO_TEST_CASE ( operators_test )
{
	SQT a;
	SQT b;

	a.scale = 1.0f;
	a.rotation = quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.0f);
	a.translation = {10.0f, 20.0f, -40.0f};

	b.scale = 2.0f;
	b.rotation = quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.0f);
	b.translation = {200.0f, -50.0f, 1000.0f};

	auto c = a % b;

	BOOST_CHECK_EQUAL(c.scale, vec3f(2.0f, 2.0f, 2.0f));
	BOOST_CHECK_EQUAL(c.rotation, quatf(1.0f, 0.0f, 0.0f, 0.0f));
	BOOST_CHECK_EQUAL(c.translation, vec3f(210.0f, -30.0f, 960.0f));
}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{
	SQT sqt(2.0f, quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.0f), vec3f(200.0f, 10.0f, 50.0f));

	mat4f M(sqt);

	BOOST_CHECK_CLOSE(M.m00, 2.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m10, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m20, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m30, 0.0f, 0.1f);

	BOOST_CHECK_CLOSE(M.m01, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m11, 2.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m21, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m31, 0.0f, 0.1f);

	BOOST_CHECK_CLOSE(M.m02, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m12, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m22, 2.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m32, 0.0f, 0.1f);

	BOOST_CHECK_CLOSE(M.m03, 200.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m13, 10.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m23, 50.0f, 0.1f);
	BOOST_CHECK_CLOSE(M.m33, 1.0f, 0.1f);
}