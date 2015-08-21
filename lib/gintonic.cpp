#include "gintonic.hpp"

namespace
{
	gintonic::camera g_default_camera = gintonic::camera();
}

namespace gintonic {

void init_all(const char* window_title)
{
	#ifdef BOOST_MSVC
		boost::filesystem::current_path(get_executable_path());
	#else
		boost::filesystem::current_path(get_executable_path() / "..");
	#endif
	g_default_camera.projection_type = camera::e_projection_type::projective;
	g_default_camera.field_of_view = deg2rad(60.0f);
	g_default_camera.near_plane = 0.1f;
	g_default_camera.far_plane = 100.0f;
	g_default_camera.add_horizontal_and_vertical_angles(static_cast<float>(M_PI), 0.0f);
	g_default_camera.move_backward(4.0f);
	renderer::init(window_title, g_default_camera, true, 800, 640);
	opengl::texture2d::init();
	font::init();
}

camera& get_default_camera() BOOST_NOEXCEPT_OR_NOTHROW
{
	return g_default_camera;
}

} // namespace gintonic