#include "gintonic.hpp"
#include <iomanip>

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
	// Define a camera.
	gt::entity camera;
	gt::camera cam(camera);
	gt::proj_info projection;
	cam.position = {0.0f, 0.0f, -4.0f};
	camera.proj_info_component = &projection;
	try
	{
		gt::init_all("directional_light", &camera);
		gt::renderer::set_freeform_cursor(true);
		
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		gt::fontstream stream;
		gt::unit_cube_PUNTB the_shape;
		gt::directional_light the_light(gt::vec4f(1.0f, 0.8f, 0.8f, 1.0f));
		gt::material the_material(
			gt::vec4f(1.0f, 1.0f, 1.0f,  0.9f), // diffuse color. 4th component is diffuse contribution
			gt::vec4f(1.0f, 1.0f, 1.0f, 20.0f), // specular color. 4th component is shininess
			"../examples/bricks.jpg",
			"../examples/bricks_SPEC.png",
			"../examples/bricks_NRM.png"
		);

		gt::entity light_entity(
			gt::SQT(
				gt::vec3f(1.0f, 1.0f, 1.0f),
				gt::quatf::axis_angle(gt::vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(-M_PI) / 2.0f),
				gt::vec3f(0.0f, 0.0f, 0.0f)),
			gt::box3f{});

		light_entity.light_component = &the_light;

		gt::entity cube_entity(
			gt::SQT(
				gt::vec3f(1.0f, 1.0f, 1.0f),
				gt::quatf(1.0f, 0.0f, 0.0f, 0.0f),
				gt::vec3f(0.f, 0.0f, 0.0f)),
			gt::box3f(
				gt::vec3f(-2.0f, -2.0f, -2.0f),
				gt::vec3f( 2.0f,  2.0f,  2.0f)));

		cube_entity.material_component = &the_material;
		cube_entity.mesh_component = &the_shape;

		gt::renderer::show();
		float curtime, dt;
		gt::vec2f mousedelta;
		
		while (!gt::renderer::should_close())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_Q))
			{
				gt::renderer::close();
			}
			if (gt::renderer::key(SDL_SCANCODE_W))
			{
				cam.move_forward(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_A))
			{
				cam.move_left(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_S))
			{
				cam.move_backward(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_D))
			{
				cam.move_right(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_SPACE))
			{
				cam.move_up(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_C))
			{
				cam.move_down(dt);
			}

			mousedelta = -gt::deg2rad(gt::renderer::mouse_delta()) / 10.0f;
			cam.add_horizontal_and_vertical_angles(mousedelta.x, mousedelta.y);
			// camera.camera_component->update(mousedelta);
			// camera.set_rotation_from_camera();
			// camera.set_mousedelta(camera.camera_component->angles);
			// cam.update(mousedelta);
			// camera.set_mouserotation(cam.angles);
			
			gt::renderer::begin_geometry_pass();
			
			// const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			// const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			const auto rotation_axis = gt::vec3f(0.0f, 0.0f, 1.0f);

			cube_entity.set_rotation(gt::quatf::axis_angle(rotation_axis, -curtime / 4.0f));
			gt::renderer::set_model_matrix(&cube_entity);
			cube_entity.material_component->bind();
			cube_entity.mesh_component->draw();
			
			// gt::renderer::set_model_matrix(rotation_axis, -curtime / 4.0f);
			
			// the_material.bind();
			
			// the_shape.draw();

			gt::renderer::begin_light_pass();

			// the_light->shine(the_light_transform);

			light_entity.light_component->shine(light_entity.global_transform());

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
				<< the_light << '\n'
				<< std::setprecision(2) << std::fixed << '\n'
				<< "Camera RIGHT:    " << cam.right << '\n'
				<< "Camera UP:       " << cam.up << '\n'
				<< "Camera FORWARD:  " << cam.direction << "\n\n"
				<< "Camera POSITION: " << cam.position;
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