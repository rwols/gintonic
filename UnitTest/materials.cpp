#define BOOST_TEST_MODULE materials test
#include <boost/test/unit_test.hpp>

#include "../Source/gintonic.hpp"

#include "../Source/Foundation/portable_iarchive.hpp"
#include "../Source/Foundation/portable_oarchive.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

#define GINTONIC_CHECK_EQUAL_MATERIAL(lhs, rhs) \
BOOST_CHECK_EQUAL(lhs->diffuseColor.x, rhs->diffuseColor.x); \
BOOST_CHECK_EQUAL(lhs->diffuseColor.y, rhs->diffuseColor.y); \
BOOST_CHECK_EQUAL(lhs->diffuseColor.z, rhs->diffuseColor.z); \
BOOST_CHECK_EQUAL(lhs->specularColor.x, rhs->specularColor.x); \
BOOST_CHECK_EQUAL(lhs->specularColor.y, rhs->specularColor.y); \
BOOST_CHECK_EQUAL(lhs->specularColor.z, rhs->specularColor.z); \
BOOST_CHECK_EQUAL(lhs->diffuseTexture->name, rhs->diffuseTexture->name); \
BOOST_CHECK_EQUAL(lhs->specularTexture->name, rhs->specularTexture->name); \
BOOST_CHECK_EQUAL(lhs->normalTexture->name, rhs->normalTexture->name);

using namespace gintonic;

BOOST_AUTO_TEST_CASE( serialization_of_materials )
{
	Renderer::initialize("Dummy Context", nullptr, false, 1, 1, false, false, false);
	boost::filesystem::current_path(get_executable_path());

	std::srand((int)std::clock());
	for (int i = 0; i < 10; ++i)
	{

		boost::filesystem::path lTemporaryFilename = boost::filesystem::unique_path();
		auto lMaterial = Material::create
		(
			vec4f(rand(), rand(), rand(), rand()),
			vec4f(rand(), rand(), rand(), rand()),
			"Resources/bricks.jpg", 
			"Resources/bricks_SPEC.png", 
			"Resources/bricks_NRM.png"
		);
		lMaterial->name = "JustSomeRandomMaterial";
		{
			std::ofstream lOutput(lTemporaryFilename.c_str(), std::ios::binary);
			eos::portable_oarchive lOutputArchive(lOutput);
			lOutputArchive << lMaterial;
		}
		{
			std::shared_ptr<Material> lDeserializedMaterial;
			std::ifstream lInput(lTemporaryFilename.c_str(), std::ios::binary);
			eos::portable_iarchive lInputArchive(lInput);
			lInputArchive >> lDeserializedMaterial;
			GINTONIC_CHECK_EQUAL_MATERIAL(lMaterial, lDeserializedMaterial);
		}
		boost::filesystem::remove(lTemporaryFilename);
	}
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
	auto lMaterial = Material::create(diffcolor, speccolor);
	*material_iter = lMaterial;
	++material_iter;
}

BOOST_AUTO_TEST_CASE( reference_counting_of_textures )
{
	vec4f diffcolor(1.0f, 1.0f, 1.0f, 1.0f);
	vec4f speccolor(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::initDummy(false);
	boost::filesystem::current_path(get_executable_path());
	std::vector<std::shared_ptr<Material>> materials;
	{
		auto mat1 = Material::create(diffcolor, speccolor, 
			"Resources/bricks.jpg", 
			"Resources/bricks_SPEC.png", 
			"Resources/bricks_NRM.png");

		materials.push_back(mat1);
		{
			auto mat2 = mat1;
			mat2->diffuseColor.x = 0.0f;
			materials.push_back(mat2);
			{
				auto mat3 = Material::create(diffcolor, speccolor);
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