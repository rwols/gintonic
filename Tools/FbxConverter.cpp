#include "gintonic.hpp"
#include "Foundation/portable_oarchive.hpp"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <fbx-file> <output-directory>\n";
		return EXIT_FAILURE;
	}
	try
	{
		gintonic::Renderer::initialize
		(
			argv[0], // Window title
			std::make_shared<gintonic::Entity>("DummyCamera"), // Dummy camera
			false,   // Don't go fullscreen
			1,       // viewport width
			1,       // viewport height
			false,   // don't initialize geometry buffer
			false,   // don't initialize shaders
			false    // don't initialize basic shapes
		);
	}
	catch (const std::exception& lException)
	{
		std::cout << "Caught exception during initialization of the renderer: "
			<< lException.what() << '\n';
		return EXIT_FAILURE;
	}

	gintonic::FbxImporter lImporter;
	std::cout << "Loading file " << argv[1] << '\n';
	const auto lRootEntity = lImporter.loadFromFile(argv[1]);
	std::cout << "Imported " << lRootEntity->name << '\n';

	boost::filesystem::path lOutputFilename(argv[2]);
	lOutputFilename /= (lRootEntity->name + ".entity");

	std::cout << "Opening file " << lOutputFilename << '\n';
	std::ofstream lOutput(lOutputFilename.string());
	if (!lOutput)
	{
		std::cout << "Could not open file " << lOutputFilename << '\n';
		return EXIT_FAILURE;
	}
	eos::portable_oarchive lOutputArchive(lOutput);
	std::cout << "Serializing " << lRootEntity->name << " to file " << lOutputFilename << '\n';
	try 
	{
		lOutputArchive << lRootEntity;
	}
	catch (const std::exception& lException)
	{
		std::cout << "Caught exception: " << lException.what() << '\n';
		return EXIT_FAILURE;
	}
	std::cout << "All done!\n";
	return EXIT_SUCCESS;
}