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
	// Define a camera.
	gt::proj_info projection;
	gt::entity camera(gt::SQT{}, gt::box3f{});
	camera.set_translation({0.0f, 0.0f, -4.0f});
	camera.proj_info_component = &projection;

	try
	{
		gt::init_all("square", &camera);
		
		gt::material red(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f));

		// Define a square.
		gt::entity square(gt::SQT{}, gt::box3f{});
		square.mesh_component = &gt::renderer::get_unit_quad_P();
		square.material_component = &red;
		
		gt::renderer::show();
		
		while (!gt::renderer::should_close())
		{
			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
			{
				gt::renderer::close();
			}

			gt::renderer::begin_geometry_pass();

			square.set_rotation
			(
				gt::quatf::axis_angle
				(
					gt::vec3f(0.0f, 0.0f, 1.0f), 
					get_elapsed_time<float>()
				)
			);

			gt::renderer::set_model_matrix(square.global_transform());
			square.material_component->bind();
			square.mesh_component->draw();
			
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

// int main(int argc, char* argv[])
// {
// 	try
// 	{
// 		gt::init_all("square");

// 		gt::get_default_camera_entity().set_translation(gt::vec3f(0.0f, 0.0f, -4.0f));
		
// 		gt::material the_material(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f));
		
// 		gt::renderer::show();
		
// 		while (!gt::renderer::should_close())
// 		{
// 			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
// 			{
// 				gt::renderer::close();
// 			}
			
// 			gt::renderer::begin_geometry_pass();

// 			gt::renderer::set_model_matrix(gt::vec3f(0.0f, 0.0f, 1.0f), get_elapsed_time<float>());
			
// 			the_material.bind();
			
// 			gt::renderer::get_unit_quad_P().draw();
			
// 			gt::renderer::begin_light_pass();
			
// 			gt::renderer::null_light_pass();
			
// 			gt::renderer::update();
// 		}
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 		return EXIT_FAILURE;
// 	}
// 	return EXIT_SUCCESS;
// }

// int main(int argc, char* argv[])
// {
// 	try
// 	{
// 		gt::init_all("rotating_square");
// 		gt::opengl::unit_quad_P the_shape;
// 		gt::material the_material(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f));
// 		gt::renderer::show();
// 		while (!gt::renderer::should_close())
// 		{
// 			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
// 			{
// 				gt::renderer::close();
// 			}
// 			gt::renderer::begin_geometry_pass();

// 			// axis-angle constructor for mat4f
// 			gt::renderer::set_model_matrix(gt::vec3f(0.0f, 0.0f, 1.0f), get_elapsed_time<float>());

// 			the_material.bind();
// 			the_shape.draw();
// 			gt::renderer::begin_light_pass();
// 			gt::renderer::null_light_pass();
// 			gt::renderer::update();
// 		}
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 		return EXIT_FAILURE;
// 	}
// 	return EXIT_SUCCESS;
// }