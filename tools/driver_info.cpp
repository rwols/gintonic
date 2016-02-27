#include "gintonic.hpp"

namespace gt = gintonic;

int main(int argc, char** argv)
{
	bool list_extensions = false;
	if (argc > 1)
	{
		if (std::strcmp(argv[1], "--extensions") == 0
			|| std::strcmp(argv[1], "-e") == 0)
		{
			list_extensions = true;
		}
	}

	try
	{
		gt::renderer::init_dummy(false);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	
	std::cout << "\nRenderer: " << gt::renderer::name() << '\n';
	std::cout << "Version: " << gt::renderer::version() << '\n';

	GLint num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

	if (list_extensions)
	{
		std::cout << "\nExtensions: ";
		for (GLint i = 0; i < num_extensions; ++i)
		{
			std::cout << glGetStringi(GL_EXTENSIONS, i);
			if (i != num_extensions - 1) std::cout << ' ';
		}
		std::cout << "\n\n";
	}

	return EXIT_SUCCESS;
}