#define BOOST_TEST_MODULE Entity test
#include <boost/test/unit_test.hpp>

#include "Entity.hpp"
#include "Transform.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
// #include <boost/serialization/export.hpp>
// BOOST_CLASS_EXPORT(gintonic::Transform);

using namespace gintonic;

BOOST_AUTO_TEST_CASE(parents_and_children)
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
    BOOST_CHECK(lEnt4->parent().lock()->parent().lock()->parent().lock() ==
                lEnt1);
    BOOST_CHECK(
        lEnt4->parent().lock()->parent().lock()->parent().lock().get() ==
        lEnt1.get());
}

BOOST_AUTO_TEST_CASE(exp_entity)
{
    {
        experimental::Entity ent("TestGuy");
        // auto                 comp = ent.add<Transform>();
        ent.saveToPersistentMedia<boost::archive::xml_oarchive>();
        // BOOST_CHECK(comp == ent.get<Transform>());
    }
    {
        try
        {
            auto ent =
                experimental::Entity::request<boost::archive::xml_iarchive>(
                    "TestGuy");
            BOOST_CHECK(ent->name == "TestGuy");
        }
        catch (const boost::archive::archive_exception& e)
        {
            std::cerr << e.what() << '\n';
            BOOST_CHECK(false);
        }
    }
}
