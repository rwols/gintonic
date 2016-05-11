#define BOOST_TEST_MODULE vec3f test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/vec2f.hpp"
#include "../Source/Math/vec3f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	vec3f a(1.0f, 0.0f, 0.0f);
	vec3f b(0.0f, 1.0f, 0.0f);
	vec3f c(0.0f, 0.0f, 1.0f);

	std::cout << a << '\n';
	std::cout << b << '\n';
	std::cout << c << '\n';

	BOOST_CHECK_EQUAL(a.x, 1.0f);
	BOOST_CHECK_EQUAL(a.y, 0.0f);
	BOOST_CHECK_EQUAL(a.z, 0.0f);

	BOOST_CHECK_EQUAL(b.x, 0.0f);
	BOOST_CHECK_EQUAL(b.y, 1.0f);
	BOOST_CHECK_EQUAL(b.z, 0.0f);

	BOOST_CHECK_EQUAL(c.x, 0.0f);
	BOOST_CHECK_EQUAL(c.y, 0.0f);
	BOOST_CHECK_EQUAL(c.z, 1.0f);

	vec2f v(2.0f, -4.0f);
	vec3f d(v, 1.0f);

	BOOST_CHECK_EQUAL(d.x, 2.0f);
	BOOST_CHECK_EQUAL(d.y, -4.0f);
	BOOST_CHECK_EQUAL(d.z, 1.0f);
}

BOOST_AUTO_TEST_CASE ( operators_test )
{
	vec3f a(1.0f, 2.0f, -3.0f);
	vec3f b(2.0f, 3.0f, 2.5f);
	vec3f c = a + b;
	vec3f d = a - b;

	std::cout << c << '\n';
	std::cout << d << '\n';

	BOOST_CHECK_EQUAL(c.x, 3.0f);
	BOOST_CHECK_EQUAL(c.y, 5.0f);
	BOOST_CHECK_EQUAL(c.z, -0.5f);

	BOOST_CHECK_EQUAL(d.x, -1.0f);
	BOOST_CHECK_EQUAL(d.y, -1.0f);
	BOOST_CHECK_EQUAL(d.z, -5.5f);

	auto dotproduct = dot(a,b);
	BOOST_CHECK_EQUAL(dotproduct, 0.5f);

	auto pointwisemultiply = a * b;
	BOOST_CHECK_EQUAL(pointwisemultiply.x, 2.0f);
	BOOST_CHECK_EQUAL(pointwisemultiply.y, 6.0f);
	BOOST_CHECK_EQUAL(pointwisemultiply.z, -7.5f);

	a = vec3f(1.0f, 0.0f, 0.0f);
	b = vec3f(0.0f, 1.0f, 0.0f);
	c = vec3f(0.0f, 0.0f, 1.0f);

	d = cross(a,b);

	BOOST_CHECK_EQUAL(d.x, 0.0f);
	BOOST_CHECK_EQUAL(d.y, 0.0f);
	BOOST_CHECK_EQUAL(d.z, 1.0f);

	d = cross(b,a);

	BOOST_CHECK_EQUAL(d.x, 0.0f);
	BOOST_CHECK_EQUAL(d.y, 0.0f);
	BOOST_CHECK_EQUAL(d.z, -1.0f);

	d = cross(b,c);

	BOOST_CHECK_EQUAL(d.x, 1.0f);
	BOOST_CHECK_EQUAL(d.y, 0.0f);
	BOOST_CHECK_EQUAL(d.z, 0.0f);

	d = cross(c,b);

	BOOST_CHECK_EQUAL(d.x, -1.0f);
	BOOST_CHECK_EQUAL(d.y, 0.0f);
	BOOST_CHECK_EQUAL(d.z, 0.0f);

	d = cross(c,a);

	BOOST_CHECK_EQUAL(d.x, 0.0f);
	BOOST_CHECK_EQUAL(d.y, 1.0f);
	BOOST_CHECK_EQUAL(d.z, 0.0f);

	d = cross(a,c);

	BOOST_CHECK_EQUAL(d.x, 0.0f);
	BOOST_CHECK_EQUAL(d.y, -1.0f);
	BOOST_CHECK_EQUAL(d.z, 0.0f);
}

BOOST_AUTO_TEST_CASE ( comparators_test )
{
	vec3f a(2.0f, 6.0f, 1.0f);
	vec3f b(-10.0f, -10.0f, -10.0f);

	BOOST_CHECK_EQUAL(b <= a, true);

	b.x = 10.0f;

	BOOST_CHECK_EQUAL(b <= a, false);

	b.x = -10.0f;

	BOOST_CHECK_EQUAL(b <= a, true);

	b.y = 10.0f;

	BOOST_CHECK_EQUAL(b <= a, false);

	b.y = -10.0f;

	BOOST_CHECK_EQUAL(b <= a, true);

	b.y = 6.0f;
	b.z = 2.0f;

	BOOST_CHECK_EQUAL(b <= a, false);
}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{
	// std::cout << "Nothing to do, really...\n";
}