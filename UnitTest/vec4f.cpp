#define BOOST_TEST_MODULE vec4f test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/vec2f.hpp"
#include "../Source/Math/vec3f.hpp"
#include "../Source/Math/vec4f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	vec4f a{1,2,3,4};
	vec4f b{4,3,2,1};

	std::cout << a + b << '\n';
}

BOOST_AUTO_TEST_CASE ( operators_test )
{

}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{

}