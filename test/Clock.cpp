#define BOOST_TEST_MODULE Clock test
#include <boost/test/unit_test.hpp>

#include <Foundation/Clock.hpp>
#include <iostream>
#include <thread>

using namespace gintonic;

BOOST_AUTO_TEST_CASE(various_tests)
{
    Clock clock;
    auto a = clock.now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto b = clock.now();
    std::cout << "a: " << a << '\n';
    std::cout << "b: " << b << '\n';
    BOOST_CHECK(a == b);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    clock.update();
    b = clock.now();
    std::cout << "b: " << b << '\n';
    BOOST_CHECK(a != b);
    BOOST_CHECK(clock.deltaTime() == clock.deltaTimeUnscaled());
    std::cout << "clock.deltaTime(): " << clock.deltaTime() << '\n';
    std::cout << "clock.deltaTimeUnscaled(): " << clock.deltaTimeUnscaled()
              << '\n';
    BOOST_CHECK(clock.deltaTime() < MilliSeconds(50));
    BOOST_CHECK(clock.deltaTimeUnscaled() < MilliSeconds(50));
    clock.timeScale = 0.5f;
    clock.update();
    BOOST_CHECK(clock.deltaTime() != clock.deltaTimeUnscaled());
    std::cout << "clock.deltaTime(): " << clock.deltaTime() << '\n';
    std::cout << "clock.deltaTimeUnscaled(): " << clock.deltaTimeUnscaled()
              << '\n';
    BOOST_CHECK(clock.deltaTime() < MilliSeconds(50));
    BOOST_CHECK(clock.deltaTimeUnscaled() < MilliSeconds(50));
}
