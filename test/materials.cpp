#define BOOST_TEST_MODULE materials test
#include <boost/test/unit_test.hpp>

#include "../lib/gintonic.hpp"

#include "../lib/Foundation/portable_iarchive.hpp"
#include "../lib/Foundation/portable_oarchive.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( serialization_of_materials )
{
	renderer::init_dummy(false);
	boost::filesystem::current_path(get_executable_path());
	const boost::filesystem::path filename = "delete_me";
	{
		auto mat = std::make_shared<Material>(
			vec4f(1.0f, 0.2f, 0.3f, 0.45f),
			vec4f(0.2f, 0.2f, 0.2f, 20.0f)
		);
		{
			std::ofstream output(filename.c_str(), std::ios::binary);
			eos::portable_oarchive oa(output);
			oa << *mat;
		}
		{
			std::ifstream input(filename.c_str(), std::ios::binary);
			eos::portable_iarchive ia(input);
			ia >> *mat;
		}
	}

	// clean up ...
	boost::filesystem::remove(filename);
}

float randfloat()
{
	return static_cast<float>(std::rand()) / static_cast<float>(std::numeric_limits<decltype(std::rand())>::max());
}

template <class OutputIter>
void add_a_material(OutputIter material_iter)
{
	vec4f diffcolor(randfloat(), randfloat(), randfloat(), 1.0f);
	vec4f speccolor(randfloat(), randfloat(), randfloat(), 20.0f);
	auto lMaterial = std::make_shared<Material>(diffcolor, speccolor);
	*material_iter = lMaterial;
	++material_iter;
}

BOOST_AUTO_TEST_CASE( reference_counting_of_textures )
{
	vec4f diffcolor(1.0f, 1.0f, 1.0f, 1.0f);
	vec4f speccolor(1.0f, 1.0f, 1.0f, 1.0f);
	renderer::init_dummy(false);
	boost::filesystem::current_path(get_executable_path());
	std::vector<std::shared_ptr<Material>> materials;
	{
		auto mat1 = std::make_shared<Material>(diffcolor, speccolor, 
			"../../examples/bricks.jpg", 
			"../../examples/bricks_SPEC.png", 
			"../../examples/bricks_NRM.png");

		materials.push_back(mat1);
		{
			auto mat2 = mat1;
			mat2->diffuseColor.x = 0.0f;
			materials.push_back(mat2);
			{
				auto mat3 = std::make_shared<Material>(diffcolor, speccolor);
				materials.push_back(mat3);
			}
		}
	}
	std::cout << "Number of materials: " << materials.size() << '\n';
	add_a_material(std::back_inserter(materials));
	std::cout << "Number of materials: " << materials.size() << '\n';
	add_a_material(std::back_inserter(materials));
	std::cout << "Number of materials: " << materials.size() << '\n';
	add_a_material(std::back_inserter(materials));
	std::cout << "Number of materials: " << materials.size() << '\n';
	add_a_material(std::back_inserter(materials));
	std::cout << "Number of materials: " << materials.size() << '\n';
	add_a_material(std::back_inserter(materials));
	std::cout << "Number of materials: " << materials.size() << '\n';
}