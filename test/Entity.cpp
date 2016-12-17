#define BOOST_TEST_MODULE Entity test
#include <boost/test/unit_test.hpp>

#include "gintonic.hpp"

#include "Foundation/portable_iarchive.hpp"
#include "Foundation/portable_oarchive.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

using namespace gintonic;

BOOST_AUTO_TEST_CASE( parents_and_children )
{
	auto lEnt1 = Entity::create("ent1");
	auto lEnt2 = Entity::create("ent2");
	auto lEnt3 = Entity::create("ent3");
	auto lEnt4 = Entity::create("ent4");

	lEnt1->addChild(lEnt2);
	lEnt2->addChild(lEnt3);
	lEnt3->addChild(lEnt4);

	auto lUpperEnt = lEnt4->parent().lock()->parent().lock()->parent().lock();
	BOOST_CHECK(lUpperEnt == lEnt1);
	BOOST_CHECK(lEnt4->parent().lock()->parent().lock()->parent().lock() == lEnt1);
	BOOST_CHECK(lEnt4->parent().lock()->parent().lock()->parent().lock().get() == lEnt1.get());

	auto comp = lEnt1->add<Component>();
	BOOST_CHECK_EQUAL(&comp->getEntity(), lEnt1.get());
}
