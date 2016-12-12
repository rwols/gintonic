#include "Common/Application.hpp"

#include <iomanip>

#define APPNAME "Hello World!"

class HelloWorldTextApplication : public Application
{
public:

	HelloWorldTextApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

	virtual ~HelloWorldTextApplication() noexcept = default;

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		Renderer::cerr() << "Hello world!\n";
		Renderer::cerr() << "Press Q to quit!\n";
		Renderer::cerr() << "This works across all examples.\n";
		Renderer::cerr() << "In most examples, you can also move around with WASD and the mouse.\n";
	}

};

DEFINE_MAIN(HelloWorldTextApplication);
