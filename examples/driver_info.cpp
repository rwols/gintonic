#include "gintonic.hpp"

namespace gt = gintonic;

int main(int argc, char** argv)
{
	gt::init_all("driver_info");
	std::cout << '\n' << gt::renderer::name() << '\n';
	std::cout << gt::renderer::version() << "\n\n";

	GLint num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

	std::cout << "OpenGL extension:\n";
	for (GLint i = 0; i < num_extensions; ++i)
	{
		std::cout << '\t' << glGetStringi(GL_EXTENSIONS, i) << '\n';
	}

	return EXIT_SUCCESS;
}