#include "gintonic.hpp"

namespace gt = gintonic;

int main(int argc, char* argv[])
{
	try
	{
		gt::init_all("square");
		gt::opengl::unit_quad_P the_shape;
		gt::material the_material;
		gt::renderer::show();
		while (!gt::renderer::should_close())
		{
			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
			{
				gt::renderer::close();
			}
			gt::renderer::bind_for_writing();
			gt::renderer::set_model_matrix(1);
			the_material.bind();
			the_shape.draw();
			gt::renderer::bind_for_reading();
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