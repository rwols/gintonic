#include "Application.hpp"

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
		Renderer::cerr() << "Hello world!";
	}

};

DEFINE_MAIN(HelloWorldTextApplication);
