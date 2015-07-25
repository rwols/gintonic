#define BOOST_TEST_MODULE materials test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

BOOST_AUTO_TEST_CASE( constructors_test )
{
	gintonic::init_all("dummy");

	BOOST_CHECK_EQUAL(1, 1);
	BOOST_CHECK_EQUAL(1, 2);
}