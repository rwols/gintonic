#include "gintonic.hpp"

namespace gt = gintonic;

template <class FloatType> FloatType get_elapsed_time()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::elapsed_time()).count()) / FloatType(1e9);
}

int main(int argc, char* argv[])
{
	try
	{
		gt::init_all("rotating_square");
		gt::opengl::unit_quad_P the_shape;
		gt::material the_material;
		gt::renderer::show();
		while (!gt::renderer::should_close())
		{
			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
			{
				gt::renderer::close();
			}
			gt::renderer::begin_geometry_pass();

			// angle-axis constructor for mat4f
			gt::renderer::set_model_matrix(get_elapsed_time<float>(), gt::vec3f(0.0f, 0.0f, 1.0f));

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