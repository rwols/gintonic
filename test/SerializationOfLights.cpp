#define BOOST_TEST_MODULE materials test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

#include <eos/portable_iarchive.hpp>
#include <eos/portable_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>

BOOST_CLASS_EXPORT(gintonic::DirectionalLight);
BOOST_CLASS_EXPORT(gintonic::AmbientLight);

#define GINTONIC_CHECK_VECTOR4_EQUAL(left, right)\
BOOST_CHECK_EQUAL(left.x, right.x);\
BOOST_CHECK_EQUAL(left.y, right.y);\
BOOST_CHECK_EQUAL(left.z, right.z);\
BOOST_CHECK_EQUAL(left.w, right.w);

using namespace gintonic;

BOOST_AUTO_TEST_CASE( Serialization )
{
	std::cout << "Initializing renderer.\n";
	Renderer::initialize("Dummy Context", nullptr, false, 1, 1, false, false, false);
	boost::filesystem::current_path(get_executable_path());
	std::cout << "Done.\n";

	std::srand((int)std::clock());
	for (int i = 0; i < 10; ++i)
	{

		boost::filesystem::path lTemporaryFilename = boost::filesystem::unique_path();

		auto lEntity = Entity::create("JustSomeRandomEntity");
		lEntity->light = AmbientLight::create(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		lEntity->light->name = "JustSomeRandomLight";

		// auto lLight = std::shared_ptr<Light>(new DirectionalLight(vec4f(1.0f, 1.0f, 1.0f, 1.0f)));

		{
			std::cout << "Serializing to " << lTemporaryFilename << '\n';
			std::ofstream lOutput(lTemporaryFilename.c_str(), std::ios::binary);
			eos::portable_oarchive lOutputArchive(lOutput);
			lOutputArchive << lEntity;
		}
		{
			std::cout << "Deserializing from " << lTemporaryFilename << '\n';
			std::shared_ptr<Entity> lDeserializedEntity;
			std::ifstream lInput(lTemporaryFilename.c_str(), std::ios::binary);
			eos::portable_iarchive lInputArchive(lInput);
			lInputArchive >> lDeserializedEntity;

			const auto lOriginalIntensity = lEntity->light->getIntensity();
			const auto lDeserializedIntensity = lDeserializedEntity->light->getIntensity();

			GINTONIC_CHECK_VECTOR4_EQUAL(lOriginalIntensity, lDeserializedIntensity);
		}
		boost::filesystem::remove(lTemporaryFilename);
	}
}

float randfloat()
{
	return static_cast<float>(std::rand()) / static_cast<float>(std::numeric_limits<decltype(std::rand())>::max());
}

