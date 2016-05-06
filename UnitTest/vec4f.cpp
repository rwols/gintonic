#define BOOST_TEST_MODULE vec4f test
#include <boost/test/unit_test.hpp>

#include "../Source/Math/vec2f.hpp"
#include "../Source/Math/vec3f.hpp"
#include "../Source/Math/vec4f.hpp"
#include "../Source/Math/Vector.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	constexpr Vector<4, float> lhs{0.0f, 1.0f, 2.0f, 3.0f};
	constexpr Vector<4, float> rhs{3.0f, 2.0f, 1.0f, 0.0f};

	constexpr auto r = lhs + rhs;
	constexpr Vector<4, float> result(r);

	static_assert(lhs[0] == 0.0f, "Should be true!");

	// std::cout << vec[0] << '\n';

	std::cout << lhs << '\n';
	std::cout << rhs << '\n';
	std::cout << result << '\n';
}

BOOST_AUTO_TEST_CASE ( operators_test )
{

}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{

}