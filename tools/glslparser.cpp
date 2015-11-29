#include "gintonic.hpp"
#include <boost/spirit/include/qi.hpp>

#define INCLUDE_TOKEN "@include"
#define VERTEX_SHADER_TOKEN "@VertexShader"
#define GEOMETRY_SHADER_TOKEN "@GeometryShader"
#define FRAGMENT_SHADER_TOKEN "@FragmentShader"
#define ATTRIBUTE_TOKEN "@Attribute"

template <typename Iter>
struct quoted_string_grammar 
: public boost::spirit::qi::grammar
<
	Iter, 
	std::string, 
	boost::spirit::qi::ascii::space_type
>
{
	quoted_string_grammar() : quoted_string_grammar::base_type(quoted_string)
	{
		using namespace boost::spirit::qi;

		quoted_string %= lexeme[lit('"') >> +(char_ - '"')]
	}

	boost::spirit::qi::rule<Iter, std::string> quoted_string;
};

template <class Iter>
bool parse_numbers(Iter first, Iter last)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	using qi::double_;
	using qi::phrase_parse;
	using ascii::space;

	const bool r = phrase_parse(
		first,
		last,
		double_ >> *(',' >> double_),
		space
	);

	if (first != last) return false;
	else return r;
}

int main(int argc, char** argv)
{
	gintonic::luatest();

	std::cout << "Give me a comma separated list of numbers.\n";
	std::string line;
	while(std::getline(std::cin, line))
	{
		if (line.empty() || line[0] == 'q' || line[0] == 'Q')
		{
			break;
		}

		if (parse_numbers(line.begin(), line.end()))
		{
			std::cout << "I can parse this!\n";
		}
		else
		{
			std::cout << "Something is wrong in this line.. >:(\n";
		}
	}
	std::cout << "Goodbye!\n";
	return EXIT_SUCCESS;
}