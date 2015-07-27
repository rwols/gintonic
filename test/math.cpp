#define BOOST_TEST_MODULE math test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

using namespace gintonic;

BOOST_AUTO_TEST_CASE( vec2_test )
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
	a += b;
	BOOST_CHECK_EQUAL(a, vec2f(1.0f, 1.0f));
	a -= 2.0f * b;
	BOOST_CHECK_EQUAL(a, vec2f(1.0f, -1.0f));
	a /= 2.0f;
	BOOST_CHECK_EQUAL(a, vec2f(0.5f, -0.5f));
	vec2f c = b - a;
	BOOST_CHECK_EQUAL(c, vec2f(-0.5f, 1.5f));
	a = vec2f(1.0f, 0.0f);
	auto dist = distance2(a, b);
	BOOST_CHECK_EQUAL(dist, 2.0f);
	dist = distance(a, b);
	BOOST_CHECK_CLOSE(dist, 1.41421356237f, 0.0001);
}

BOOST_AUTO_TEST_CASE( vec3_test )
{
	vec3f a(1.0f, 0.0f, 0.0f), b(0.0f, 1.0f, 0.0f), c(0.0f, 0.0f, 1.0f);
	BOOST_CHECK_EQUAL(dot(a,b), 0.0f);
	BOOST_CHECK_EQUAL(dot(a,c), 0.0f);
	BOOST_CHECK_EQUAL(dot(b,c), 0.0f);
	BOOST_CHECK_EQUAL(a % b, c); // cross product
	BOOST_CHECK_EQUAL(b % c, a); // cross product
	BOOST_CHECK_EQUAL(c % a, b); // cross product
}