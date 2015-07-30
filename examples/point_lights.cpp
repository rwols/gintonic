#include "gintonic.hpp"

namespace gt = gintonic;

#define MOVE_SPEED 2.77778f // 10 km/s

template <class FloatType> FloatType get_elapsed_time()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::elapsed_time()).count()) / FloatType(1e9);
}

template <class FloatType> FloatType get_dt()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::delta_time()).count()) / FloatType(1e9);
}

int main(int argc, char* argv[])
{
	try
	{
		gt::init_all("point_lights");
		
		gt::renderer::set_freeform_cursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;
		
		gt::opengl::unit_cube_PUN the_shape;
		
		std::unique_ptr<gt::light> red_light(
			new gt::point_light(
				gt::vec3f(1.0f, 0.2f, 0.2f), 
				gt::vec3f(0.0f, 0.0f, 1.0f)));

		gt::sqt_transformf red_light_transform;
		
		red_light_transform.scale = 0.2f;
		red_light_transform.rotation = gt::quatf::from_angle_axis(
			-M_PI / 2.0f, 
			gt::vec3f(1.0f, 0.0f, 0.0f));
		red_light_transform.translation = gt::vec3f(0.0f, 2.0f, 0.0f);

		std::unique_ptr<gt::material> the_material(
			new gt::material_cd(
				gt::vec4f(1.0f,1.0f,1.0f,0.9f), 
				"../examples/bricks.jpg"));

		std::unique_ptr<gt::material> red_light_material(
			new gt::material_c(
				gt::vec4f(1.0f, 0.0f, 0.0f, 1.0f)
				));

		assert(red_light);
		assert(the_material);

		gt::renderer::show();

		float curtime, dt;
		
		while (!gt::renderer::should_close())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			
			if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_Q))
			{
				gintonic::renderer::close();
			}
			if (gintonic::renderer::key(SDL_SCANCODE_W))
			{
				gt::get_default_camera().move_forward(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_A))
			{
				gt::get_default_camera().move_left(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_S))
			{
				gt::get_default_camera().move_backward(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_D))
			{
				gt::get_default_camera().move_right(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_SPACE))
			{
				gt::get_default_camera().move_up(MOVE_SPEED * dt);
			}

			auto mousedelta = gintonic::renderer::mouse_delta();
			mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
			
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);
			
			gt::renderer::bind_for_writing();
			
			const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			
			const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			
			const auto rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			
			gt::renderer::set_model_matrix(-curtime / 4.0f, rotation_axis);
			the_material->bind();
			the_shape.draw();

			gt::renderer::set_model_matrix(red_light_transform.get_matrix());
			red_light_material->bind();
			gt::renderer::get_unit_sphere_P().draw();

			gt::renderer::bind_for_reading();

			// gt::renderer::null_light_pass();

			red_light->shine(red_light_transform);

			gt::renderer::get_text_shader()->activate();
			
			gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
			
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			stream.open(font_inconsolata);
			
			stream << "Move around with WASD.\n"
				<< "Look around with the mouse.\n"
				<< "Go up by holding the spacebar.\n"
				<< "Press Q to quit.\n"
				<< "Camera position: " << std::fixed << std::setprecision(1) 
				<< gt::renderer::camera().position << '\n'
				<< "Light intensity: " << red_light->intensity << '\n'
				<< "Light position:  " << red_light_transform.translation 
				<< '\n';
			stream.close();
			
			glEnable(GL_CULL_FACE);
			
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