#define BOOST_TEST_MODULE quatf test
#include <boost/test/unit_test.hpp>

#include "quatf.hpp"
#include "vec2f.hpp"
#include "vec3f.hpp"
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