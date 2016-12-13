#define BOOST_TEST_MODULE Octree test
#include <boost/test/unit_test.hpp>

#include "Entity.hpp"
#include "Foundation/Octree.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( signals_test )
{
	namespace bs = boost::signals2;
	bs::signal<void(void)> sig;
	
}

BOOST_AUTO_TEST_CASE( main_test )
{

	{
		DEBUG_PRINT;
		DEBUG_PRINT;
		auto lEntity = Entity::create();
		DEBUG_PRINT;
		const box3f lBoundingBox(vec3f(-128.0f, -128.0f, -128.0f), vec3f(128.0f, 128.0f, 128.0f));
		DEBUG_PRINT;
		Octree lRoot(lBoundingBox);
		DEBUG_PRINT;
		lRoot.insert(lEntity);
		DEBUG_PRINT;
		std::cerr << "Setting translation to [10 10 10] ...\n";
		DEBUG_PRINT;
		lEntity->setTranslation(vec3f(10.0f, 10.0f, 1.0f));
		DEBUG_PRINT;
	}
	DEBUG_PRINT;
}
