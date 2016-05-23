#define BOOST_TEST_MODULE Octree test
#include <boost/test/unit_test.hpp>

#include "../Source/Entity.hpp"
#include "../Source/Foundation/Octree.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( main_test )
{
	auto lEntity = Entity::create();
	const box3f lBoundingBox(vec3f(-128.0f, -128.0f, -128.0f), vec3f(128.0f, 128.0f, 128.0f));
	Octree lRoot(lBoundingBox);
	lRoot.insert(lEntity);
	std::cerr << "Setting translation to [10 10 10] ...\n";
	lEntity->setTranslation(vec3f(10.0f, 10.0f, 1.0f));
}
