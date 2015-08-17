#include "gintonic.hpp"

namespace gt = gintonic;

int main(int argc, char** argv)
{
	#ifdef BOOST_MSVC
		boost::filesystem::current_path(gt::get_executable_path());
	#else
		boost::filesystem::current_path(gt::get_executable_path() / "..");
	#endif
	gt::init_all("driver_info");
	std::cout << "\nRenderer: " << gt::renderer::name() << '\n';
	std::cout << "Version: " << gt::renderer::version() << '\n';

	GLint num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

	std::cout << "\nExtensions: ";
	for (GLint i = 0; i < num_extensions; ++i)
	{
		std::cout << glGetStringi(GL_EXTENSIONS, i);
		if (i != num_extensions - 1) std::cout << ' ';
	}
	std::cout << "\n\n";

	return EXIT_SUCCESS;
}