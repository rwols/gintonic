#include "gintonic.hpp"
#include "spatialtrees.hpp"
#include <algorithm>

namespace gt = gintonic;

#define DIM 2

template <std::size_t N, class T> 
struct dummy_object
{
	gt::box<float,N> m_bounds;
	T m_data;

	gt::quadtree<dummy_object<N,T>>* spatialtree;

	dummy_object(const gt::box<float,N>& bound, const T& data)
	: m_bounds(bound), m_data(data) {}

	inline const gt::box<float,N>& bounds() const { return m_bounds; }
};

std::string random_string( size_t length )
{
	auto randchar = []() -> char
	{
		const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[ rand() % max_index ];
	};
	std::string str(length,0);
	std::generate_n( str.begin(), length, randchar );
	return str;
}

typedef dummy_object<DIM,std::string> object_type;
typedef gt::quadtree<object_type> spatialtree_type;

void do_query_area(const spatialtree_type& tree)
{
	gt::vec<float,DIM> query_area_min;
	gt::vec<float,DIM> query_area_max;
	std::cout << "Enter the minimum corner of the query bounding area.\n>> ";
	std::cin >> query_area_min;
	std::cout << "You entered: " << query_area_min << '\n';
	std::cout << "Enter the maximum corner of the query bounding area.\n>> ";
	std::cin >> query_area_max;
	std::cout << "You entered: " << query_area_max << '\n';

	const gt::box<float,DIM> query_area(query_area_min, query_area_max);
	std::vector<const object_type*> query_result;
	std::cout << "Collecting objects...\n";
	tree.query(query_area, std::back_inserter(query_result));
	if (query_result.empty())
	{
		std::cout << "No objects!\n";
		return;
	}
	for (const auto* o : query_result)
	{
		std::cout << o->bounds() << ": " << o->m_data << '\n';
	}
}

struct object_name_identifier
{
	const std::string& name;
	object_name_identifier(const std::string& name) : name(name) {}
	bool operator()(const object_type& o) const
	{
		return o.m_data == name;
	}
};

void do_remove_object(std::vector<object_type>& objects, spatialtree_type& tree)
{
	std::cout << "Enter the name of the object that you would like to remove.\n>> ";
	std::string name;
	std::cin >> name;
	std::cout << "You entered: " << name << '\n';
	object_name_identifier identifier(name);
	auto r = std::find_if(objects.begin(), objects.end(), identifier);
	if (r == objects.end())
	{
		std::cout << "Could not find an object with the name " << name << '\n';
	}
	else
	{
		if (r->spatialtree)
		{
			r->spatialtree->erase(&(*r));
			std::cout << "Succesfully removed " << name << '\n';
		}
		else
		{
			std::cout << name << " does not appear to be part of a tree.\n";
		}
	}
}

struct print_object
{
	inline void operator()(const object_type* o) const
	{
		std::cout << o->bounds() << " : "<< o->m_data << '\n';
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <number-of-objects>\n";
		return EXIT_FAILURE;
	}
	const int num_objects = static_cast<int>(std::atoi(argv[1]));
	const int min_corner_value = -1000;
	const int max_corner_value = 1000;
	try
	{
		std::cout << "Populating quadtree with " << num_objects << " random objects... ";

		// Populate some objects
		std::srand((int)std::clock());
		std::vector<object_type> objects;
		objects.reserve(num_objects);
		for (int i = 0; i < num_objects; ++i)
		{
			gt::vec<float,DIM> min_corner(
				(float)((rand() % (2 * max_corner_value - 2)) - max_corner_value + 1), 
				(float)((rand() % (2 * max_corner_value - 2)) - max_corner_value + 1));
				// (float)((rand() % (2 * max_corner_value - 2)) - max_corner_value + 1));
			gt::vec<float,DIM> max_corner = min_corner 
			+ gt::vec<float,DIM>(
				(float)((rand() % 10) + 1), 
				(float)((rand() % 10) + 1));
				// (float)((rand() % 10) + 1));
			if (max_corner[0] >= max_corner_value)
			{
				min_corner[0] -= ((max_corner[0] - max_corner_value) + 1);
				max_corner[0] -= ((max_corner[0] - max_corner_value) + 1);
			}
			if (max_corner[1] >= max_corner_value)
			{
				min_corner[1] -= ((max_corner[1] - max_corner_value) + 1);
				max_corner[1] -= ((max_corner[1] - max_corner_value) + 1);
			}
			// if (max_corner[2] >= max_corner_value)
			// {
			// 	min_corner[2] -= ((max_corner[2] - max_corner_value) + 2);
			// 	max_corner[2] -= ((max_corner[2] - max_corner_value) + 2);
			// }
			const gt::box<float,DIM> bounds(min_corner, max_corner);
			const std::string name(random_string(6));
			objects.emplace_back(bounds, name);
		}

		std::cout << "Patience... ";

		// Initialize quadtree
		const gt::vec<float,DIM> min_corner((float)min_corner_value, (float)min_corner_value/*, (float)min_corner_value*/);
		const gt::vec<float,DIM> max_corner((float)max_corner_value, (float)max_corner_value/*, (float)max_corner_value*/);
		spatialtree_type tree(min_corner, max_corner, objects.begin(), objects.end());

		std::cout << "Done!\n";

		bool keep_running = true;
		while(keep_running)
		{
			char choice;
			std::cout << "Options:\n"
				<< " * [p]rint all objects\n"
				<< " * [q]uery an area for objects\n"
				<< " * [r]emove an object (by name)\n"
				<< " * [e]xit\n>> ";
			std::cin >> choice;
			switch (choice)
			{
				case 'p': 
					tree.foreach(print_object()); 
					break;
				case 'q': 
					do_query_area(tree); 
					break;
				case 'r': 
					do_remove_object(objects, tree); 
					break;
				case 'e': 
					keep_running = false; 
					break;
				default:
				{
					std::cout << "Unknown option. Type 'q' to query or 'e' to exit.\n";
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}