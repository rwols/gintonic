#define BOOST_TEST_MODULE box2f test
#include <boost/test/unit_test.hpp>

#include "Math/box2f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	vec2f minCorner(-1.0f, -2.0f);
	vec2f maxCorner(123.0f, 14.431f);

	box2f box(minCorner, maxCorner);

	BOOST_CHECK_EQUAL(box.minCorner.x, -1.0f);
	BOOST_CHECK_EQUAL(box.minCorner.y, -2.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.x, 123.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.y, 14.431f);

	minCorner = { -0.5f, -1.0f };
	maxCorner = {  2.0f,  9.0f };

	box.minCorner = minCorner;
	box.maxCorner = maxCorner;

	BOOST_CHECK_EQUAL(box.minCorner.x, -0.5f);
	BOOST_CHECK_EQUAL(box.minCorner.y, -1.0);
	BOOST_CHECK_EQUAL(box.maxCorner.x,  2.0f);
	BOOST_CHECK_EQUAL(box.maxCorner.y,  9.0f);

}

BOOST_AUTO_TEST_CASE ( collisions_test )
{
	vec2f minCorner(-1.0f, -1.0f);
	vec2f maxCorner( 1.0f,  1.0f);

	box2f a(minCorner, maxCorner);

	minCorner = {-0.5f, -0.5f };
	maxCorner = { 0.5f,  0.5f };

	box2f b(minCorner, maxCorner);

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

	minCorner = { 3.0f, -1.0f };
	maxCorner = { 5.0f,  1.0f };

	b.minCorner = minCorner;
	b.maxCorner = maxCorner;

	/* Picture of the situation:

	+---+   +---+
	| a |   | b |
	+---+   +---+            */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), false);

	minCorner = { 0.0f, 0.0f };
	maxCorner = { 2.0f, 2.0f };

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

	b.minCorner = { -2.0f, -2.0f };
	b.maxCorner = {  2.0f,  0.0f };

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

	b.minCorner = { -2.0f,  0.0f };
	b.maxCorner = {  2.0f,  2.0f };

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

	b.minCorner = { 0.0f, -2.0f };
	b.maxCorner = { 2.0f,  2.0f };

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

	b.minCorner = { -2.0f, -2.0f };
	b.maxCorner = {  0.0f,  2.0f };

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

}