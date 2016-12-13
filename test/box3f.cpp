#define BOOST_TEST_MODULE box3f test
#include <boost/test/unit_test.hpp>

#include "Foundation/allocator.hpp"
#include "Math/box3f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <set>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	vec3f minCorner(-1.0f, -2.0f, 100.0f);
	vec3f maxCorner(123.0f, 14.431f, 110.0f);

	box3f box(minCorner, maxCorner);

	BOOST_CHECK_EQUAL(box.minCorner.x, -1.0f);
	BOOST_CHECK_EQUAL(box.minCorner.y, -2.0f);
	BOOST_CHECK_EQUAL(box.minCorner.z, 100.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.x, 123.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.y, 14.431f);
	BOOST_CHECK_EQUAL(box.maxCorner.z, 110.0f);

	minCorner = { -0.5f, -1.0f, -10.0f };
	maxCorner = {  2.0f,  9.0f,  10.0f };

	box.minCorner = minCorner;
	box.maxCorner = maxCorner;

	BOOST_CHECK_EQUAL(box.minCorner.x, -0.5f);
	BOOST_CHECK_EQUAL(box.minCorner.y, -1.0);
	BOOST_CHECK_EQUAL(box.minCorner.z, -10.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.x,  2.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.y,  9.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.z, 10.0f);

}

BOOST_AUTO_TEST_CASE ( getting_the_right_corners )
{
	vec3f minCorner(-1.0f, -1.0f, -1.0f);
	vec3f maxCorner( 1.0f,  1.0f,  1.0f);

	box3f lTheBox(minCorner, maxCorner);

	auto comparator = [](const vec3f& lhs, const vec3f& rhs)
	{
		return std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z);
	};

	using Vector3Set = std::set<vec3f, decltype(comparator), allocator<vec3f>>;

	Vector3Set lCornersFromMethod(comparator);

	lTheBox.getCorners(std::inserter(lCornersFromMethod, lCornersFromMethod.end()));

	vec3f lDimension = lTheBox.maxCorner - lTheBox.minCorner;

	Vector3Set lActualCorners(comparator);
	lActualCorners.insert(lTheBox.minCorner);
	lActualCorners.insert(lTheBox.minCorner + vec3f(lDimension.x, 0.0f, 0.0f)); 
	lActualCorners.insert(lTheBox.minCorner + vec3f(0.0f, lDimension.y, 0.0f));
	lActualCorners.insert(lTheBox.minCorner + vec3f(0.0f, 0.0f, lDimension.z));
	lActualCorners.insert(lTheBox.minCorner + vec3f(0.0f, lDimension.y, lDimension.z));
	lActualCorners.insert(lTheBox.minCorner + vec3f(lDimension.x, 0.0f, lDimension.z));
	lActualCorners.insert(lTheBox.minCorner + vec3f(lDimension.x, lDimension.y, 0.0f));
	lActualCorners.insert(lTheBox.maxCorner);

	BOOST_CHECK_EQUAL(lCornersFromMethod == lActualCorners, true);
}

BOOST_AUTO_TEST_CASE ( adding_points_test )
{
	box3f mybox{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

	vec3f point{1.0f, 1.0f, 1.0f};

	BOOST_CHECK_EQUAL(mybox.contains(point), false);

	mybox.addPoint(point);

	BOOST_CHECK_EQUAL(mybox.contains(point), true);

	BOOST_CHECK_EQUAL(mybox.maxCorner == point, true);
	BOOST_CHECK_EQUAL(mybox.minCorner == vec3f(0.0f, 0.0f, 0.0f), true);
}

BOOST_AUTO_TEST_CASE ( collisions_test )
{
	vec3f minCorner(-1.0f, -1.0f, -1.0f);
	vec3f maxCorner( 1.0f,  1.0f,  1.0f);

	box3f a(minCorner, maxCorner);

	minCorner = {-0.5f, -0.5f, -1.0f };
	maxCorner = { 0.5f,  0.5f,  1.0f };

	box3f b(minCorner, maxCorner);

	/* Picture of the situation:

	+-----------+
	| a         |
	|   +---+   |
	|   | b |   |
	|   +---+   |
	|           |
	+-----------+             */

	BOOST_CHECK_EQUAL(a.contains(b), true);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), true);

	minCorner = { 3.0f, -1.0f, -1.0f };
	maxCorner = { 5.0f,  1.0f,  1.0f };

	b.minCorner = minCorner;
	b.maxCorner = maxCorner;

	/* Picture of the situation:

	+---+   +---+
	| a |   | b |
	+---+   +---+            */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), false);

	minCorner = { 0.0f, 0.0f, -1.0f };
	maxCorner = { 2.0f, 2.0f,  1.0f };

	b.minCorner = minCorner;
	b.maxCorner = maxCorner;

	/* Picture of the situation:

	    +-------+
	    |     b |
	+---+---+   |
	|   |   |   |
	|   +---+---+
	| a     |
	+-------+                */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), true);

	b.minCorner = { -2.0f, -2.0f, -1.0f };
	b.maxCorner = {  2.0f,  0.0f,  1.0f };

	/* Picture of the situation:

	    +-------+
	    | a     |
	+---+-------+---+
	| b |       |   |
	|   +-------+   |
	|               |
	+---------------+       */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), true);

	b.minCorner = { -2.0f,  0.0f, -1.0f };
	b.maxCorner = {  2.0f,  2.0f,  1.0f };

	/* Picture of the situation:

	+---------------+
	| b             |
	|   +-------+   |
	|   |       |   |
	+---+-------+---+
	    | a     |
	    +-------+            */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), true);

	b.minCorner = { 0.0f, -2.0f, -1.0f };
	b.maxCorner = { 2.0f,  2.0f,  1.0f };

	/* Picture of the situation:

	    +-------+
	    | b     |
	+---+---+   |
	| a |   |   |
	|   |   |   |
	+---+---+   |
	    |       |
	    +-------+     */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), true);

	b.minCorner = { -2.0f, -2.0f, -1.0f };
	b.maxCorner = {  0.0f,  2.0f,  1.0f };

	/* Picture of the situation:

	+-------+
	| b     |
	|   +---+---+
	|   |   | a |
	|   |   |   |
	|   +---+---+
	|       |
	+-------+        */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), true);

	// More various tests

	b.minCorner = { -10.0f, -10.0f, -10.0f };
	b.maxCorner = {  10.0f,  10.0f,  10.0f };

	a.minCorner = { -1.0f, 6.0f, 2.0f };
	a.maxCorner = a.minCorner; // a degenerates to a point.

	BOOST_CHECK_EQUAL(b.contains(a), true);

	// Some more tests

	b.minCorner = {  0.0f, -100.0f, -100.0f};
	b.maxCorner = {100.0f,    0.0f,    0.0f};

	a.minCorner = { -10.0f, -10.0f, -10.0f };
	a.maxCorner = {  10.0f,  10.0f,  10.0f };

	BOOST_CHECK_EQUAL(intersects(a, b), true);

	b.minCorner = { 0.0f, 6.0f, -2.0f };
	b.maxCorner = { 0.0f, 6.0f, -2.0f };

	a.minCorner = { -12.0f,  4.0f, 15.0f };
	a.maxCorner = {  -4.0f, 12.0f, 23.0f };

	BOOST_CHECK_EQUAL(intersects(a, b), false);

}