#define BOOST_TEST_MODULE materials test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

#include "portable_iarchive.hpp"
#include "portable_oarchive.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( serialization_of_materials )
{
	init_all("serialization_of_materials");
	const boost::filesystem::path filename = "delete_me";
	{
		material mat(
			vec4f(1.0f, 0.2f, 0.3f, 0.45f),
			vec4f(0.2f, 0.2f, 0.2f, 20.0f)
		);
		{
			std::ofstream output(filename.c_str(), std::ios::binary);
			eos::portable_oarchive oa(output);
			oa << mat;
		}
		{
			std::ifstream input(filename.c_str(), std::ios::binary);
			eos::portable_iarchive ia(input);
			ia >> mat;
		}
	}

	// clean up ...
	boost::filesystem::remove(filename);
}