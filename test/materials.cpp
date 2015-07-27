#define BOOST_TEST_MODULE materials test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

#include "portable_iarchive.hpp"
#include "portable_oarchive.hpp"
#include <fstream>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( serialization_of_materials )
{
	init_all("serialization_of_materials");
	const boost::filesystem::path filename = "delete_me";
	{
		material mat;
		{
			std::ofstream output(filename.c_str());
			eos::portable_oarchive oa(output);
			oa << mat;
		}
		{
			std::ifstream input(filename.c_str());
			eos::portable_iarchive ia(input);
			ia >> mat;
		}
	}
	{
		material_c mat;
		mat.diffuse_color = vec4f(1.0f, 0.5f, 0.25f, 1.0f);
		{
			std::ofstream output(filename.c_str());
			eos::portable_oarchive oa(output);
			oa << mat;
		}
		{
			material_c deserialized_mat;
			std::ifstream input(filename.c_str());
			eos::portable_iarchive ia(input);
			ia >> deserialized_mat;
			BOOST_CHECK_EQUAL(mat.diffuse_color, deserialized_mat.diffuse_color);
		}
	}

	// clean up ...
	boost::filesystem::remove(filename);
}