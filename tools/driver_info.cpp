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

	if (!list_extensions)
	{
		std::cout << gt::renderer::name() << '\n';
		std::cout << gt::renderer::version() << '\n';
	}
	else
	{
		GLint num_extensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
		for (GLint i = 0; i < num_extensions; ++i)
		{
			std::cout << glGetStringi(GL_EXTENSIONS, i) << '\n';
		}
	}
	return EXIT_SUCCESS;
}