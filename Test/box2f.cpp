#define BOOST_TEST_MODULE box2f test
#include <boost/test/unit_test.hpp>

#include "../lib/Math/box2f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	vec2f min_corner(-1.0f, -2.0f);
	vec2f max_corner(123.0f, 14.431f);

	box2f box(min_corner, max_corner);

	BOOST_CHECK_EQUAL(box.min_corner.x, -1.0f);
	BOOST_CHECK_EQUAL(box.min_corner.y, -2.0f);
	BOOST_CHECK_EQUAL(box.max_corner.x, 123.0f);
	BOOST_CHECK_EQUAL(box.max_corner.y, 14.431f);

	min_corner = { -0.5f, -1.0f };
	max_corner = {  2.0f,  9.0f };

	box.min_corner = min_corner;
	box.max_corner = max_corner;

	BOOST_CHECK_EQUAL(box.min_corner.x, -0.5f);
	BOOST_CHECK_EQUAL(box.min_corner.y, -1.0);
	BOOST_CHECK_EQUAL(box.max_corner.x,  2.0f);
	BOOST_CHECK_EQUAL(box.max_corner.y,  9.0f);

}

BOOST_AUTO_TEST_CASE ( collisions_test )
{
	vec2f min_corner(-1.0f, -1.0f);
	vec2f max_corner( 1.0f,  1.0f);

	box2f a(min_corner, max_corner);

	min_corner = {-0.5f, -0.5f };
	max_corner = { 0.5f,  0.5f };

	box2f b(min_corner, max_corner);

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

	min_corner = { 3.0f, -1.0f };
	max_corner = { 5.0f,  1.0f };

	b.min_corner = min_corner;
	b.max_corner = max_corner;

	/* Picture of the situation:

	+---+   +---+
	| a |   | b |
	+---+   +---+            */

	BOOST_CHECK_EQUAL(a.contains(b), false);
	BOOST_CHECK_EQUAL(b.contains(a), false);
	BOOST_CHECK_EQUAL(intersects(a, b), false);

	min_corner = { 0.0f, 0.0f };
	max_corner = { 2.0f, 2.0f };

	b.min_corner = min_corner;
	b.max_corner = max_corner;

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

	b.min_corner = { -2.0f, -2.0f };
	b.max_corner = {  2.0f,  0.0f };

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

	b.min_corner = { -2.0f,  0.0f };
	b.max_corner = {  2.0f,  2.0f };

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

	b.min_corner = { 0.0f, -2.0f };
	b.max_corner = { 2.0f,  2.0f };

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

	b.min_corner = { -2.0f, -2.0f };
	b.max_corner = {  0.0f,  2.0f };

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