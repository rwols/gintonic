#include "gintonic.hpp"
#include <fbxsdk.h>

namespace gt = gintonic;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <fbx-file> <output-directory>\n";
		return EXIT_FAILURE;
	}
	gt::init_all(argv[0]);
	return EXIT_SUCCESS;
}