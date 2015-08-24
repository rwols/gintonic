#include "gintonic.hpp"
#include "camera.hpp"

namespace
{
	// gintonic::camera g_default_camera = gintonic::camera();
	
	gintonic::octree g_root_octree = gintonic::octree(
		gintonic::vec3f(-1000.0f, -1000.0f, -1000.0f), 
		gintonic::vec3f( 1000.0f,  1000.0f,  1000.0f));
	
	gintonic::proj_info g_default_proj_info;
	
	gintonic::entity g_root_entity(
		gintonic::SQT(), 
		gintonic::box3f(), 
		g_root_octree);

	gintonic::entity g_default_camera_entity(
		gintonic::SQT(), 
		gintonic::box3f(), 
		g_root_octree, 
		&g_root_entity);

}

namespace gintonic {

void release()
{
	// nothing atm
}

void init_all(const char* window_title)
{
	#ifdef BOOST_MSVC
		boost::filesystem::current_path(get_executable_path());
	#else
		boost::filesystem::current_path(get_executable_path() / "..");
	#endif
	// g_default_camera.projection = camera::kPerspectiveProjection;
	// g_default_camera.field_of_view = deg2rad(60.0f);
	// g_default_camera.near_plane = 0.1f;
	// g_default_camera.far_plane = 100.0f;
	// g_default_camera.add_horizontal_and_vertical_angles(static_cast<float>(M_PI), 0.0f);
	// g_default_camera.move_backward(4.0f);

	g_default_camera_entity.proj_info_component = &g_default_proj_info;

	renderer::init(window_title, &g_default_camera_entity, true, 800, 640);
	opengl::texture2d::init();
	font::init();
	std::atexit(release);
}

// camera& get_default_camera() BOOST_NOEXCEPT_OR_NOTHROW
// {
// 	return g_default_camera;
// }

proj_info* get_default_proj_info() BOOST_NOEXCEPT_OR_NOTHROW
{
	return &g_default_proj_info;
}

entity& get_root_entity() BOOST_NOEXCEPT_OR_NOTHROW
{
	return g_root_entity;
}

entity& get_default_camera_entity() BOOST_NOEXCEPT_OR_NOTHROW
{
	return g_default_camera_entity;
}

octree& get_root_octree() BOOST_NOEXCEPT_OR_NOTHROW
{
	return g_root_octree;
}

} // namespace gintonic