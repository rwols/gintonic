#include "gintonic.hpp"

#define M_PIf static_cast<float>(M_PI)

namespace gt = gintonic;

int main(int argc, char* argv[])
{
	try
	{
		gt::init_all("square");
		
		gt::get_default_camera_entity().set_scale(gt::vec3f(1.0f, 1.0f, 1.0f));
		gt::get_default_camera_entity().set_translation(gt::vec3f(0.0f, 0.0f, 4.0f));
		gt::get_default_camera_entity().set_rotation(gt::quatf::axis_angle(gt::vec3f(0.0f, 1.0f, 0.0f), M_PIf));

		gt::renderer::set_camera(&gt::get_default_camera_entity());

		std::cout << "Local camera SQT transform: " << gt::get_default_camera_entity().local_transform() << '\n'
			<< "Global camera SQT transform: " << gt::get_default_camera_entity().global_transform() << '\n';
		
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