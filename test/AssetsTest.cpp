#define BOOST_TEST_MODULE AssetsTest
#include "Foundation/filesystem.hpp"
#include "SDLRenderContext.hpp"
#include "SDLWindow.hpp"
#include "Texture2D.hpp"
#include <algorithm>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <iostream>
#include <vector>

using namespace gintonic;

struct Mock : public experimental::Asset<Mock>
{
    Mock() = default;

    Mock(std::string name) : experimental::Asset<Mock>(std::move(name)) {}

    static const char* extension() { return ".mock"; }
    static const char* prefixFolder() { return "mock"; }

    int data;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        using boost::serialization::make_nvp;
        using boost::serialization::base_object;
        archive& make_nvp("super",
                          base_object<experimental::Asset<Mock>>(*this));
        archive& make_nvp("data", data);
    }
};

BOOST_AUTO_TEST_CASE(mock_asset)
{
    gintonic::initExecutablePath();
    Mock mock("foo");
    mock.data = 42;
    std::cerr << "Saving...\n";
    try
    {
        mock.saveToPersistentMedia<boost::archive::xml_oarchive>();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        BOOST_TEST(false, "exception was thrown");
        return;
    }
    try
    {
        auto ptr = Mock::request<boost::archive::xml_iarchive>("foo");
        BOOST_CHECK(ptr != nullptr);
        BOOST_CHECK(ptr->name == "foo");
        BOOST_CHECK(ptr->data == 42);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        BOOST_TEST(false, "exception was thrown");
    }
}

BOOST_AUTO_TEST_CASE(images)
{
    gintonic::initExecutablePath();
    int a, b, c;
    try
    {
        gintonic::experimental::Image image("bricks.jpg");
        a = image.getWidth();
        b = image.getHeight();
        c = image.getComponents();
        image.saveToPersistentMedia<boost::archive::xml_oarchive>();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    try
    {
        auto ptr = gintonic::experimental::Image::request<
            boost::archive::xml_iarchive>("bricks.jpg");
        BOOST_CHECK(ptr != nullptr);
        BOOST_CHECK(ptr->name == "bricks.jpg");
        BOOST_CHECK(ptr->getWidth() == a);
        BOOST_CHECK(ptr->getHeight() == b);
        BOOST_CHECK(ptr->getComponents() == c);
        auto another = gintonic::experimental::Image::request<
            boost::archive::xml_iarchive>("bricks.jpg");
        BOOST_CHECK(another != nullptr);
        BOOST_CHECK(ptr->name == another->name);
        BOOST_CHECK(ptr->getWidth() == another->getWidth());
        BOOST_CHECK(ptr->getHeight() == another->getHeight());
        BOOST_CHECK(ptr->getComponents() == another->getComponents());
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

BOOST_AUTO_TEST_CASE(textures)
{
    gintonic::initExecutablePath();
    gintonic::SDLWindow dummy("dummy", 10, 10);
    dummy.context.reset(new gintonic::SDLRenderContext(
        static_cast<gintonic::SDLWindow&>(dummy), 4, 1));
    try
    {
        gintonic::experimental::Image image("bricks.jpg");
        image.saveToPersistentMedia<boost::archive::xml_oarchive>();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    gintonic::IntrusivePtr<const gintonic::experimental::Image> img;

    GLint mipmaps;

    try
    {
        auto ptr = gintonic::experimental::Image::request<
            boost::archive::xml_iarchive>("bricks.jpg");
        gintonic::experimental::Texture2D texture("bricks", ptr);
        img = ptr;
        texture.saveToPersistentMedia<boost::archive::xml_oarchive>();
        mipmaps = texture.getMipMaps();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    try
    {
        auto ptr = gintonic::experimental::Texture2D::request<
            boost::archive::xml_iarchive>("bricks");
        BOOST_TEST(img->getComponents() == ptr->getImage()->getComponents());
        BOOST_TEST(img->getWidth() == ptr->getImage()->getWidth());
        BOOST_TEST(img->getHeight() == ptr->getImage()->getHeight());
        BOOST_TEST(mipmaps == ptr->getMipMaps());
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
