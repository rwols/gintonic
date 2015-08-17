#include "gintonic.hpp"

namespace gt = gintonic;

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
		// #ifdef BOOST_MSVC
		// 	boost::filesystem::current_path(gt::get_executable_path());
		// #else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		// #endif
		
		gt::init_all("basic_shapes");
		
		gt::renderer::set_freeform_cursor(true);
		
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		
		gt::fontstream stream;
		
		gt::opengl::unit_cube_PUNTB cube;
		gt::opengl::unit_sphere_PUN sphere(16);
		gt::opengl::unit_cone_PN cone(32);
		
		std::unique_ptr<gt::light> the_light(new gt::directional_light(gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f)));
		gt::sqt_transformf the_light_transform;
		the_light_transform.rotation = gt::quatf::from_angle_axis(static_cast<float>(-M_PI) / 2.0f, gt::vec3f(1.0f, 0.0f, 0.0f));
		
		gt::material brick_material(
			gt::vec4f(0.8f, 0.8f, 0.8f,  0.9f), // diffuse color. 4th component is diffuse contribution
			gt::vec4f(0.3f, 0.3f, 0.3f,  4.0f), // specular color. 4th component is shininess
			"../examples/bricks_COLOR.png",     // diffuse texture.
			"../examples/bricks_SPEC.png"       // specular texture.
		);

		gt::material brick_with_normal_material(
			gt::vec4f(1.0f, 1.0f, 1.0f,  0.9f), // diffuse color. 4th component is diffuse contribution
			gt::vec4f(0.3f, 0.3f, 0.3f, 20.0f), // specular color. 4th component is shininess
			"../examples/bricks_COLOR.png",     // diffuse texture.
			"../examples/bricks_SPEC.png",      // specular texture.
			"../examples/bricks_NRM.png"
		);

		gt::material flat_material(
			gt::vec4f(0.4f, 0.4f, 0.4f, 0.8f), // diffuse color
			gt::vec4f(0.3f, 0.3f, 0.3f, 20.0f) // specular color
		);

		float curtime, dt, yaxis, zaxis, cursin, fieldofview = M_PI / 2.0f, aspectratio = 1.0f;
		gt::vec3f rotation_axis;
		gt::vec2f mousedelta;
		gt::sqt_transformf shape_transform;
		shape_transform.scale = 1.0f;
		bool show_gbuffer = false;

		gt::renderer::show();
		
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
				gt::get_default_camera().move_forward(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_A))
			{
				gt::get_default_camera().move_left(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_S))
			{
				gt::get_default_camera().move_backward(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_D))
			{
				gt::get_default_camera().move_right(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_SPACE))
			{
				gt::get_default_camera().move_up(dt);
			}
			if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_G))
			{
				show_gbuffer = !show_gbuffer;
			}
			if (gintonic::renderer::key(SDL_SCANCODE_MINUS))
			{
				fieldofview -= dt;
			}
			if (gintonic::renderer::key(SDL_SCANCODE_EQUALS))
			{
				fieldofview += dt;
			}

			mousedelta = gintonic::renderer::mouse_delta();
			mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
			
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);

			gt::renderer::begin_geometry_pass();
			
			// yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			// zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			// rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			yaxis = 1.0f;
			zaxis = 0.0f;
			rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));

			shape_transform.rotation = gt::quatf::from_angle_axis(
				-curtime / 8.0f, // angle
				rotation_axis);  // rotation axis
			shape_transform.translation = { 0.0f, 0.0f, 0.0f };
			
			gt::renderer::set_model_matrix(shape_transform.get_matrix());
			brick_with_normal_material.bind();
			cube.draw();

			shape_transform.translation[0] += 3.0f;
			gt::renderer::set_model_matrix(shape_transform.get_matrix());
			brick_material.bind();
			sphere.draw();

			shape_transform.translation[0] -= 6.0f;
			gt::renderer::set_model_matrix(shape_transform.get_matrix());
			flat_material.bind();
			cone.draw();

			shape_transform.translation[0] += 3.0f;
			shape_transform.translation[1] += 4.0f;

			const auto frustummatrix = gt::mat4f::frustum(fieldofview, aspectratio, 1.0f, 2.0f);
			gt::renderer::set_model_matrix(shape_transform.get_matrix() * frustummatrix);
			brick_with_normal_material.bind();
			// glDisable(GL_CULL_FACE);
			cube.draw();
			// glEnable(GL_CULL_FACE);

			if (show_gbuffer)
			{
				stream.open(font_inconsolata);
				gt::renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::renderer::begin_light_pass();

				cursin = gt::sin(curtime) / 2.0f;

				the_light_transform.rotation = gt::quatf::from_angle_axis(
					cursin + static_cast<float>(-M_PI) / 2.0f, // angle
					gt::vec3f(1.0f, 0.0f, 0.0f));              // rotation axis
				the_light->shine(the_light_transform);

				gt::renderer::get_text_shader()->activate();
				gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
				glDisable(GL_CULL_FACE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				stream.open(font_inconsolata);
				stream << "Move around with WASD.\n"
					<< "Look around with the mouse.\n"
					<< "Go up by holding the spacebar.\n"
					<< "FOV for the frustum: " << fieldofview << '\n'
					<< "Press G to view the geometry buffer.\n"
					<< "Press Q to quit.\n"
					<< the_light << '\n';
				stream.close();
				
				glEnable(GL_CULL_FACE);
			}
			
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