#include "gintonic.hpp"

namespace gt = gintonic;

int main(int argc, char** argv)
{
	bool lListExtensions = false;
	if (argc > 1)
	{
		if (std::strcmp(argv[1], "--extensions") == 0
			|| std::strcmp(argv[1], "-e") == 0)
		{
			lListExtensions = true;
		}
	}

	try
	{
		gt::Renderer::initDummy(false);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	if (!lListExtensions)
	{
		std::cout << gt::Renderer::name() << '\n';
		std::cout << gt::Renderer::version() << '\n';
	}
	else
	{
		GLint lNumExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &lNumExtensions);
		for (GLint i = 0; i < lNumExtensions; ++i)
		{
			std::cout << glGetStringi(GL_EXTENSIONS, i) << '\n';
		}
	}
	return EXIT_SUCCESS;
}