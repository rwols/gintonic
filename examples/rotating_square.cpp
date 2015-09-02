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
	gt::proj_info projection_component;
	gt::camera cam_component;
	gt::entity cam_entity;
	projection_component.attach(cam_entity);
	cam_component.attach(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(0.0f, 0.0f, 4.0f));
	
	try
	{
		gt::init_all("square", cam_entity);
		
		gt::material red(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f));

		// Define a square.
		gt::entity square(gt::SQT{}, gt::box3f{});
		gt::renderer::get_unit_quad_P().attach(square);
		red.attach(square);
		
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

			gt::renderer::set_model_matrix(square);
			square.material_component()->bind();
			square.mesh_component()->draw();
			
			gt::renderer::begin_light_pass();
			gt::renderer::ambient_light_pass();
			
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