#define BOOST_TEST_MODULE math test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

using namespace gintonic;

BOOST_AUTO_TEST_CASE( vector2_test )
{
	vec2f a(1.0f, 0.0f), b(0.0f, 1.0f);
	BOOST_CHECK_EQUAL(a - a, vec2f(0.0f, 0.0f));
	BOOST_CHECK_EQUAL(b - b, vec2f(0.0f, 0.0f));
	BOOST_CHECK_EQUAL(a + b, vec2f(1.0f, 1.0f));
	BOOST_CHECK_EQUAL(2.0f * a + 3.0f * b, vec2f(2.0f, 3.0f));
	BOOST_CHECK_EQUAL(a * 2.0f + b * 3.0f, vec2f(2.0f, 3.0f));
	BOOST_CHECK_EQUAL(a / 2.0f + b / 4.0f, vec2f(0.5f, 0.25f));
	BOOST_CHECK_EQUAL(dot(a,b), 0.0f);
	BOOST_CHECK_EQUAL(dot(vec2f(1.0f, 2.0f), vec2f(-3.0f, 8.0f)), 13.0f);
	// a += b;
	// BOOST_CHECK_EQUAL(a, vec2f(1.0f, 1.0f));
	// a -= 2.0f * b;
	// BOOST_CHECK_EQUAL(a, vec2f(1.0f, -1.0f));
	// a /= 2.0f;
	// BOOST_CHECK_EQUAL(a, vec2f(0.5f, -0.5f));
	// vec2f c = b - a;
	// BOOST_CHECK_EQUAL(c, vec2f(-0.5f, 1.5f));
	// a = vec2f(1.0f, 0.0f);
	// auto dist = distance2(a, b);
	// BOOST_CHECK_EQUAL(dist, 2.0f);
	// dist = distance(a, b);
	// BOOST_CHECK_CLOSE(dist, 1.41421356237f, 0.0001);
}

BOOST_AUTO_TEST_CASE( vector3_test )
{
	vec3f a(1.0f, 0.0f, 0.0f), b(0.0f, 1.0f, 0.0f), c(0.0f, 0.0f, 1.0f);
	BOOST_CHECK_EQUAL(dot(a,b), 0.0f);
	BOOST_CHECK_EQUAL(dot(a,c), 0.0f);
	BOOST_CHECK_EQUAL(dot(b,c), 0.0f);
	BOOST_CHECK_EQUAL(a % b, c); // cross product
	BOOST_CHECK_EQUAL(b % c, a); // cross product
	BOOST_CHECK_EQUAL(c % a, b); // cross product
}

BOOST_AUTO_TEST_CASE ( quaternion_test )
{
	quatf a = quatf::from_angle_axis(M_PI / 2.0f, vec3f(0.0f, 1.0f, 0.0f));
	quatf b = quatf::from_angle_axis(M_PI / 2.0f, vec3f(0.0f, 0.0f, -1.0f));
	auto dir = a.direction();
	BOOST_CHECK_CLOSE(dir[0], -1.0f, 0.001);
	BOOST_CHECK_CLOSE(dir[1], 0.0f, 0.001);
	BOOST_CHECK_CLOSE(dir[2], 0.0f, 0.001);

	// start with the vector [0,0,-1]. rotate it CCW 90 degrees around [0,0,-1]
	// (so that does nothing), then rotate it CCW 90 degrees around [0,1,0],
	// which results in the vector [-1,0,0].
	dir = (a * b).direction();
	BOOST_CHECK_CLOSE(dir[0], -1.0f, 0.001); // note that quaternion multiplication is not commutative ...
	BOOST_CHECK_CLOSE(dir[1], 0.0f, 0.001);
	BOOST_CHECK_CLOSE(dir[2], 0.0f, 0.001);

	// start with the vector [0,0,-1], rotate it CCW 90 degrees around [0,1,0],
	// which results in the vector [-1,0,0], then rotate it CCW 90 degrees around
	// [0,0,-1]. This is the same as rotating CW 90 degrees around [0,0,1], which
	// results in the vector [0,1,0].
	dir = (b * a).direction();
	BOOST_CHECK_CLOSE(dir[0], 0.0f, 0.001); // ... as is demonstrated here
	BOOST_CHECK_CLOSE(dir[1], 1.0f, 0.001);
	BOOST_CHECK_CLOSE(dir[2], 0.0f, 0.001);
}