#include "gintonic.hpp"
#include "camera.hpp"

namespace gintonic {

void release()
{
	// nothing atm
}

void init_all(const char* window_title, entity& camera)
{
	#ifdef BOOST_MSVC
		boost::filesystem::current_path(get_executable_path());
	#else
		boost::filesystem::current_path(get_executable_path() / "..");
	#endif

	renderer::init(window_title, camera, true, 800, 640);
	opengl::texture2d::init();
	font::init();
	std::atexit(release);
}

} // namespace gintonic