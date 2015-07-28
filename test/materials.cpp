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
	{
		material* mat = new material_cd(
			vec4f(0.5f, 1.0f, 0.25f, 1.0f),
			"../examples/bricks.jpg");
		mat->bind(); // test if this method works
		{
			std::cout << "Opening file " << filename << " for writing.\n";
			std::ofstream output(filename.c_str(), std::ios::binary);
			std::cout << "Constructing output archive.\n";
			boost::archive::text_oarchive to_cout(std::cout);
			eos::portable_oarchive oa(output);
			oa << mat;
			std::cout << "The archive looks like this:\n";
			to_cout << mat;
		}
		{
			bool success = true;
			material* deserialized_mat = nullptr;
			std::cout << "Opening file " << filename << " for reading.\n";
			std::ifstream input(filename.c_str(), std::ios::binary);
			std::cout << "Constructing input archive.\n";
			eos::portable_iarchive ia(input);
			try
			{
				std::cout << "Deserializing.\n";
				ia >> deserialized_mat;
			}
			catch (const std::exception& e)
			{
				std::cout << "Failed!\n";
				success = false;
			}
			BOOST_REQUIRE_EQUAL(success, true);
			BOOST_REQUIRE_EQUAL(deserialized_mat == nullptr, false);
			std::cout << "Reinterpreting the pointer.\n";
			material_cd* mat2 = reinterpret_cast<material_cd*>(deserialized_mat);
			std::cout << "Reinterpreting the pointer.\n";
			material_cd* mat3 = reinterpret_cast<material_cd*>(mat);
			std::cout << "Checking for equality between the diffuse colors.\n";
			BOOST_CHECK_EQUAL(mat3->diffuse_color, mat2->diffuse_color);
			delete deserialized_mat;
		}
		delete mat;
	}

	// clean up ...
	boost::filesystem::remove(filename);
}