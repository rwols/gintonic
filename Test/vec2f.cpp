#define BOOST_TEST_MODULE vec2f test
#include <boost/test/unit_test.hpp>

#include "../lib/Math/vec2f.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( constructor_test )
{
	vec2f a(1.0f, 0.0f);
	vec2f b(0.0f, 1.0f);

	std::cout << a << '\n';
	std::cout << b << '\n';

	BOOST_CHECK_EQUAL(a.x, 1.0f);
	BOOST_CHECK_EQUAL(a.y, 0.0f);
	BOOST_CHECK_EQUAL(b.x, 0.0f);
	BOOST_CHECK_EQUAL(b.y, 1.0f);
}

BOOST_AUTO_TEST_CASE ( operators_test )
{
	vec2f a(1.0f, 2.0f);
	vec2f b(2.0f, 3.0f);
	vec2f c = a + b;
	vec2f d = a - b;

	std::cout << c << '\n';
	std::cout << d << '\n';

	BOOST_CHECK_EQUAL(c.x, 3.0f);
	BOOST_CHECK_EQUAL(c.y, 5.0f);
	BOOST_CHECK_EQUAL(d.x, -1.0f);
	BOOST_CHECK_EQUAL(d.y, -1.0f);
}

BOOST_AUTO_TEST_CASE ( various_functions_test )
{
	std::cout << "Generating array...\n";
	const int count = 4 * 10000000;
	std::vector<float> array(count);
	std::vector<float> result_array1(count / 2);
	std::vector<float> result_array2(count / 2);
	std::generate(array.begin(), array.end(), std::rand);
	
	vec2f test_vec1, test_vec2, test_vec3;
	// float x, y;

	// auto now = std::chrono::high_resolution_clock::now();
	// for (std::size_t i = 0; i < count; i += 4)
	// {
	// 	test_vec1.set(array[i + 0], array[i + 1]);
	// 	test_vec2.set(array[i + 2], array[i + 3]);
	// 	test_vec3 = test_vec1 + test_vec2;
	// 	test_vec3.get(x,y);
	// 	result_array1[i/2] = x;
	// 	result_array1[(i/2)+1] = y;
	// }
	// auto later = std::chrono::high_resolution_clock::now();
	// auto duration1 = later - now;

	auto now = std::chrono::high_resolution_clock::now();
	for (std::size_t i = 0; i < count; i += 4)
	{
		test_vec1.x = array[i + 0];
		test_vec1.y = array[i + 1];
		test_vec2.x = array[i + 2];
		test_vec2.y = array[i + 3];
		test_vec3 = test_vec1 + test_vec2;
		result_array2[i/2] = test_vec3.x;
		result_array2[(i/2)+1] = test_vec3.y;
	}
	auto later = std::chrono::high_resolution_clock::now();
	auto duration2 = later - now;

	using std::chrono::milliseconds;
	using std::chrono::duration_cast;

	std::cout << "Method 2: "
		<< duration_cast<milliseconds>(duration2).count() << " ms\n";
}