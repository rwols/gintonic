#include "gintonic.hpp"
#include "vec2f.hpp"

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
		gt::init_all("basic_shapes");
		
		gt::renderer::set_freeform_cursor(true);
		
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		
		gt::fontstream stream;
		
		gt::opengl::unit_cube_PUNTB cube;
		gt::opengl::unit_sphere_PUN sphere(16);
		gt::opengl::unit_cone_PN cone(32);
		
		std::unique_ptr<gt::light> the_light(new gt::directional_light(gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f)));
		gt::SQT the_light_transform;
		the_light_transform.rotation = gt::quatf::axis_angle(gt::vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(-M_PI) / 2.0f);
		
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
		gt::SQT shape_transform;
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

			mousedelta = gt::renderer::mouse_delta();
			mousedelta = -gt::deg2rad(mousedelta) / 4.0f;
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta.x, mousedelta.y);

			gt::renderer::begin_geometry_pass();
			
			// yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			// zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			// rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			yaxis = 1.0f;
			zaxis = 0.0f;
			rotation_axis = gt::vec3f(0.0f, yaxis, zaxis).normalize();

			shape_transform.rotation = gt::quatf::axis_angle(
				rotation_axis,
				-curtime / 8.0f);
			shape_transform.translation = { 0.0f, 0.0f, 0.0f };
			
			gt::renderer::set_model_matrix(shape_transform);
			brick_with_normal_material.bind();
			cube.draw();

			shape_transform.translation.x += 3.0f;
			gt::renderer::set_model_matrix(shape_transform);
			brick_material.bind();
			sphere.draw();

			shape_transform.translation.x -= 6.0f;
			gt::renderer::set_model_matrix(shape_transform);
			flat_material.bind();
			cone.draw();

			shape_transform.translation.x += 3.0f;
			shape_transform.translation.y += 4.0f;

			gt::mat4f frustummatrix;
			frustummatrix.set_inverse_perspective(fieldofview, aspectratio, 1.0f, 2.0f);
			gt::renderer::set_model_matrix(gt::mat4f(shape_transform) * frustummatrix);
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

				cursin = std::sin(curtime) / 2.0f;

				the_light_transform.rotation = gt::quatf::axis_angle(
					gt::vec3f(1.0f, 0.0f, 0.0f),
					cursin + static_cast<float>(-M_PI) / 2.0f);
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
					<< the_light << '\n'
					<< gt::renderer::camera().position << '\n';
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