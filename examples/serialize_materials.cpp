#include "gintonic.hpp"
#include <boost/archive/text_oarchive.hpp>
#include "portable_iarchive.hpp"
#include "portable_oarchive.hpp"
#include <fstream>

namespace gt = gintonic;

int main(int argc, char* argv[])
{
	try
	{
		#ifdef BOOST_MSVC
			boost::filesystem::current_path(gt::get_executable_path());
		#else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		#endif
		const boost::filesystem::path filename("delete_me");
		gt::init_all("serialize_materials");
		{
			gt::material* ptr = nullptr;
			
			ptr = new gt::material_cd(gt::vec4f(0.25f, 1.0f, 1.0f, 1.0f), "../examples/bricks.jpg");

			ptr->save(filename);

			boost::archive::text_oarchive to_cout(std::cout);

			std::cout << "\n\n\tOriginal material:\n\n";
			
			to_cout << ptr;

			delete ptr;
			ptr = nullptr;
		}
		// End of scope; archives flush

		std::cout << "\n\n\tFilesize: "
			<< boost::filesystem::file_size(filename)
			<< " bytes";

		{
			gt::material* ptr = nullptr;

			ptr = gt::material::load(filename);

			assert(ptr);

			std::cout << "\n\n\tDeserialized material:\n\n";

			boost::archive::text_oarchive to_cout(std::cout);

			to_cout << ptr;

			delete ptr;
			ptr = nullptr;
		}
		boost::filesystem::remove(filename);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}