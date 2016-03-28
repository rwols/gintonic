#include "gintonic.hpp"

namespace gintonic {

void release()
{
	// nothing atm
}

void initializeEverything(const char* window_title, std::shared_ptr<Entity> camera)
{
	const auto lOldPath = boost::filesystem::current_path();
	#ifdef BOOST_MSVC
		boost::filesystem::current_path(get_executable_path());
	#else
		boost::filesystem::current_path(get_executable_path() / "..");
	#endif

	Renderer::initialize(window_title, std::move(camera), true, 800, 640);
	
	std::atexit(release);

	boost::filesystem::current_path(lOldPath);
}

void initializeEverything(const char* windowTitle)
{
	auto lCameraEntity = std::make_shared<Entity>("DefaultCamera");
	initializeEverything(windowTitle, std::move(lCameraEntity));
}

} // namespace gintonic