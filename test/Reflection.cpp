#define BOOST_TEST_MODULE Reflection test
#include <boost/test/unit_test.hpp>

#include "Entity.hpp"
#include "Foundation/Reflection.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace gintonic;

// class Person
// {
// public:
// 	template <class StringType>
// 	Person(StringType&& name, const int age)
// 	: name(std::forward<StringType>(name))
// 	, age(age)
// 	{}

// private:

// 	GT_REFLECTABLE
// 	(
// 		(std::string) name,
// 		(int) age
// 	);

// };

// struct PrintVisitor
// {
//     template<class FieldData>
//     void operator()(FieldData f)
//     {
//         std::cout << f.name() << ": " << f.value() << std::endl;
//     }
// };

// template<class T>
// void printFields(T& x)
// {
//     visitEach(x, PrintVisitor());
// }

BOOST_AUTO_TEST_CASE( main_test )
{
	std::cout << "Hello world!\n";
	// Person p("John Doe", 42);
	// printFields(p);
}
