#define BOOST_TEST_MODULE quatf test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/quatf.hpp"
#include "../Source/Math/vec2f.hpp"
#include "../Source/Math/vec3f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <ctime>

#define GINTONIC_CHECK_VECTOR4_CLOSE(left, right, tolerance)\
BOOST_CHECK_CLOSE(left.x, right.x, tolerance);\
BOOST_CHECK_CLOSE(left.y, right.y, tolerance);\
BOOST_CHECK_CLOSE(left.z, right.z, tolerance);\
BOOST_CHECK_CLOSE(left.w, right.w, tolerance);

#define GINTONIC_CHECK_VECTOR4_SMALL(val, tolerance)\
BOOST_CHECK_SMALL(val.x, tolerance);\
BOOST_CHECK_SMALL(val.y, tolerance);\
BOOST_CHECK_SMALL(val.z, tolerance);\
BOOST_CHECK_SMALL(val.w, tolerance);

using namespace gintonic;

float randf()
{
	return static_cast<float>(rand());
}

BOOST_AUTO_TEST_CASE( constructor_test )
{

}

BOOST_AUTO_TEST_CASE ( operators_test )
{
	quatf a = quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), static_cast<float>(M_PI) / 2.0f);
	quatf b = quatf::axis_angle(vec3f(0.0f, 0.0f, -1.0f), static_cast<float>(M_PI) / 2.0f);
	auto dir = a.direction();
	BOOST_CHECK_CLOSE(dir.x, -1.0f, 0.001f);
	BOOST_CHECK_CLOSE(dir.y,  0.0f, 0.001f);
	BOOST_CHECK_CLOSE(dir.z,  0.0f, 0.001f);

	// start with the vector [0,0,-1]. rotate it CCW 90 degrees around [0,0,-1]
	// (so that does nothing), then rotate it CCW 90 degrees around [0,1,0],
	// which results in the vector [-1,0,0].
	dir = (a * b).direction();
	BOOST_CHECK_CLOSE(dir.x, -1.0f, 0.001f); // note that quaternion multiplication is not commutative ...
	BOOST_CHECK_CLOSE(dir.y,  0.0f, 0.001f);
	BOOST_CHECK_CLOSE(dir.z,  0.0f, 0.001f);

	// start with the vector [0,0,-1], rotate it CCW 90 degrees around [0,1,0],
	// which results in the vector [-1,0,0], then rotate it CCW 90 degrees around
	// [0,0,-1]. This is the same as rotating CW 90 degrees around [0,0,1], which
	// results in the vector [0,1,0].
	dir = (b * a).direction();
	BOOST_CHECK_CLOSE(dir.x, 0.0f, 0.001f); // ... as is demonstrated here
	BOOST_CHECK_CLOSE(dir.y, 1.0f, 0.001f);
	BOOST_CHECK_CLOSE(dir.z, 0.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE ( inverses_and_conjugates )
{
	std::srand((int)std::clock());
	for (int i = 0; i < 1000000; ++i)
	{
		quatf lRotation(randf(), randf(), randf(), randf());
		quatf lCopy0 = lRotation;
		
		const float lLengthSquared = lRotation.length2();
		const float lLength = lRotation.length();
		lCopy0.normalize();

		quatf lCopy1 = lRotation;
		lCopy1.x /= lLength;
		lCopy1.y /= lLength;
		lCopy1.z /= lLength;
		lCopy1.w /= lLength;

		const float lShouldBeOne0 = lCopy0.length();
		const float lShouldBeOne1 = lCopy1.length();

		BOOST_CHECK_CLOSE(lShouldBeOne0, 1.0f, 0.01f);
		BOOST_CHECK_CLOSE(lShouldBeOne1, 1.0f, 0.01f);

		GINTONIC_CHECK_VECTOR4_CLOSE(lCopy0, lCopy1, 0.01f);

		lCopy0 = lRotation;
		lCopy0.invert();
		lCopy1 = lRotation;
		lCopy1.x = -lCopy1.x;
		lCopy1.y = -lCopy1.y;
		lCopy1.z = -lCopy1.z;
		lCopy1.x /= lLengthSquared;
		lCopy1.y /= lLengthSquared;
		lCopy1.z /= lLengthSquared;
		lCopy1.w /= lLengthSquared;

		GINTONIC_CHECK_VECTOR4_CLOSE(lCopy0, lCopy1, 0.01f);
	}
}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{
	vec2f mousedelta(0.0f, 0.0f);
	quatf rotation(1.0f, 0.0f, 0.0f, 0.0f);

	vec3f forward_dir = rotation.forward_direction();
	vec3f right_dir = rotation.right_direction();
	vec3f up_dir = rotation.up_direction();

	BOOST_CHECK_CLOSE(forward_dir.x, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(forward_dir.y, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(forward_dir.z, -1.0f, 0.1f);

	BOOST_CHECK_CLOSE(right_dir.x, 1.0f, 0.1f);
	BOOST_CHECK_CLOSE(right_dir.y, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(right_dir.z, 0.0f, 0.1f);

	BOOST_CHECK_CLOSE(up_dir.x, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(up_dir.y, 1.0f, 0.1f);
	BOOST_CHECK_CLOSE(up_dir.z, 0.0f, 0.1f);

	rotation.add_mousedelta(mousedelta);

	forward_dir = rotation.forward_direction();
	right_dir = rotation.right_direction();
	up_dir = rotation.up_direction();

	BOOST_CHECK_CLOSE(forward_dir.x, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(forward_dir.y, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(forward_dir.z, -1.0f, 0.1f);

	BOOST_CHECK_CLOSE(right_dir.x, 1.0f, 0.1f);
	BOOST_CHECK_CLOSE(right_dir.y, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(right_dir.z, 0.0f, 0.1f);

	BOOST_CHECK_CLOSE(up_dir.x, 0.0f, 0.1f);
	BOOST_CHECK_CLOSE(up_dir.y, 1.0f, 0.1f);
	BOOST_CHECK_CLOSE(up_dir.z, 0.0f, 0.1f);
}

BOOST_AUTO_TEST_CASE ( mix_test )
{
	auto a = quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), static_cast<float>(M_PI) / 2.0f);
	auto b = quatf(1.0f, 0.0f, 0.0f, 0.0f);

	quatf prev_q = a;
	for (float lambda = 0.0f; lambda <= 1.0f; lambda += 0.001f)
	{
		auto q = mix(a, b, lambda);
		GINTONIC_CHECK_VECTOR4_CLOSE(q, prev_q, 110.0f);
		prev_q = q;
	}
}

BOOST_AUTO_TEST_CASE ( slerp_test )
{
	auto a = quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), static_cast<float>(M_PI) / 2.0f);
	auto b = quatf(1.0f, 0.0f, 0.0f, 0.0f);

	quatf prev_q = a;
	for (float lambda = 0.0f; lambda <= 1.0f; lambda += 0.001f)
	{
		auto q = slerp(a, b, lambda);
		GINTONIC_CHECK_VECTOR4_CLOSE(q, prev_q, 110.0f);
		prev_q = q;
	}
}