#define BOOST_TEST_MODULE SDLRenderContext test
#include "SDLRenderContext.hpp"
#include "SDLWindow.hpp"
#include <boost/test/unit_test.hpp>

using namespace gintonic;

BOOST_AUTO_TEST_CASE(construct_and_destruct_1) { SDLWindow test("test", 1, 1); }

BOOST_AUTO_TEST_CASE(construct_and_destruct_2)
{
    // fullscreen
    SDLWindow test("test");
}

BOOST_AUTO_TEST_CASE(rendercontext_1)
{
    SDLWindow test("test", 1, 1);
    test.context.reset(new SDLRenderContext(test, 3, 3));
    test.context->focus();
    std::cout << test.context->getName() << '\n'
              << test.context->getVersion() << '\n';
}
