#include "gintonic.hpp"

namespace gt = gintonic;

int main(int argc, char* argv[])
{
	try
	{
		#ifdef BOOST_MSVC
			boost::filesystem::current_path(gt::get_executable_path());
		#else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		#endif
		gt::init_all("square");
		gt::opengl::unit_quad_P the_shape;
		gt::material the_material(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f));
		gt::renderer::show();
		while (!gt::renderer::should_close())
		{
			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
			{
				gt::renderer::close();
			}
			gt::renderer::begin_geometry_pass();
			gt::renderer::set_model_matrix(1.0f);
			the_material.bind();
			the_shape.draw();
			gt::renderer::begin_light_pass();
			gt::renderer::null_light_pass();
			gt::renderer::update();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}