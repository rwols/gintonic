#define BOOST_TEST_MODULE Simd test
#include <boost/test/unit_test.hpp>

#include "Foundation/simd.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

#define SHUF(v,a,b,c,d) _mm_shuffle_ps(v,v,_MM_SHUFFLE(a,b,c,d))

BOOST_AUTO_TEST_CASE( main_test )
{
	using std::cout;

	auto v = _mm_setr_ps(0,1,2,3);
	cout << v << '\n';

	auto r = SHUF(v,3,2,1,0); // identity
	cout << r << '\n';

	r = SHUF(v,0,0,0,0); // broadcast first coordinate
	cout << r << '\n';

	r = SHUF(v,1,1,1,1); // broadcast second coordinate
	cout << r << '\n';

	r = SHUF(v,2,2,2,2); // broadcast third coordinate
	cout << r << '\n';

	r = SHUF(v,3,3,3,3); // broadcast fourth coordinate
	cout << r << '\n';

	r = SHUF(v,0,3,2,1); // shift 1 left == shift 3 right
	cout << r << '\n';

	r = SHUF(v,1,0,3,2); // shift 2 left == shift 2 right
	cout << r << '\n';

	r = SHUF(v,2,1,0,3); // shift 3 left == shift 1 right
	cout << r << '\n';

	// Take the negative
	r = _mm_negate(v); cout << r << '\n';

	// single out x
	cout << '\n';
	r = _mm_x000_ps(v); cout << r << '\n';
	r = _mm_0x00_ps(v); cout << r << '\n';
	r = _mm_00x0_ps(v); cout << r << '\n';
	r = _mm_000x_ps(v); cout << r << '\n';

	// single out y
	cout << '\n';
	r = _mm_y000_ps(v); cout << r << '\n';
	r = _mm_0y00_ps(v); cout << r << '\n';
	r = _mm_00y0_ps(v); cout << r << '\n';
	r = _mm_000y_ps(v); cout << r << '\n';

	// single out z
	cout << '\n';
	r = _mm_z000_ps(v); cout << r << '\n';
	r = _mm_0z00_ps(v); cout << r << '\n';
	r = _mm_00z0_ps(v); cout << r << '\n';
	r = _mm_000z_ps(v); cout << r << '\n';

	// single out w
	cout << '\n';
	r = _mm_w000_ps(v); cout << r << '\n';
	r = _mm_0w00_ps(v); cout << r << '\n';
	r = _mm_00w0_ps(v); cout << r << '\n';
	r = _mm_000w_ps(v); cout << r << '\n';
}
