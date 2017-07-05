#define BOOST_TEST_MODULE filesystem
#include "Foundation/filesystem.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE(filesystem_operations)
{
    gintonic::Path path = "assets/../assets/images";
    gintonic::Path exe = gintonic::get_executable_path();
    std::cout << exe << " -> " << path << '\n';
    std::cout << gintonic::normalize(path) << '\n';
}
