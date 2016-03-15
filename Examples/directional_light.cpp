#include "gintonic.hpp"
#include <iomanip>

namespace gt = gintonic;

template <class FloatType> FloatType get_elapsed_time()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(Renderer::elapsed_time()).count()) / FloatType(1e9);
}

template <class FloatType> FloatType get_dt()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(Renderer::deltaTime()).count()) / FloatType(1e9);
}

int main(int argc, char* argv[])
{
	// Define a camera.
	gt::proj_info projection_component;
	gt::camera cam_component;
	// std::shared_ptr<entity> cam_entity = std::make_shared<entity>();
	// cam_entity.add_component(new projection_component());
	// cam_entity.add_component(new camera_component());
	projection_component.attach(cam_entity);
	cam_component.attach(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(-2.78f, 0.83f, 1.17f));
	cam_component.add_mouse(gt::vec2f(-gt::deg2rad(60.0f), -gt::deg2rad(9.0f)));

	try
	{
		gt::init_all("directional_light", cam_entity);
		gt::Renderer::setFreeformCursor(true);

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

		gt::entity cube_entity(
			gt::SQT(
				gt::vec3f(1.0f, 1.0f, 1.0f),
				gt::quatf(1.0f, 0.0f, 0.0f, 0.0f),
				gt::vec3f(0.f, 0.0f, 0.0f)),
			gt::box3f(
				gt::vec3f(-2.0f, -2.0f, -2.0f),
				gt::vec3f( 2.0f,  2.0f,  2.0f)));

		the_material.attach(cube_entity);
		the_shape.attach(cube_entity);
		the_light.attach(light_entity);

		gt::Renderer::show();
		float curtime, dt;
		gt::vec2f mousedelta;

		while (!gt::Renderer::shouldClose())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_Q))
			{
				gt::Renderer::close();
			}
			if (gt::Renderer::key(SDL_SCANCODE_W))
			{
				cam_entity.move_forward(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_A))
			{
				cam_entity.move_left(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_S))
			{
				cam_entity.move_backward(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_D))
			{
				cam_entity.move_right(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_SPACE))
			{
				cam_entity.move_up(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_C))
			{
				cam_entity.move_down(dt);
			}

			mousedelta = -gt::deg2rad(gt::Renderer::mouse_delta()) / 10.0f;
			cam_component.add_mouse(mousedelta);

			gt::Renderer::begin_geometry_pass();

			const auto rotation_axis = gt::vec3f(0.0f, 0.0f, 1.0f);

			cube_entity.set_rotation(gt::quatf::axis_angle(rotation_axis, -curtime / 4.0f));
			gt::Renderer::set_model_matrix(cube_entity);
			cube_entity.material_component()->bind();
			cube_entity.mesh_component()->draw();

			gt::Renderer::begin_light_pass();

			light_entity.light_component()->shine(light_entity);

			gt::Renderer::get_text_shader()->activate();
			gt::Renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));

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
				<< "Camera RIGHT:    " << cam_entity.local_transform().rotation.right_direction() << '\n'
				<< "Camera UP:       " << cam_entity.local_transform().rotation.up_direction() << '\n'
				<< "Camera FORWARD:  " << cam_entity.local_transform().rotation.forward_direction() << '\n'
				<< "Camera QUAT:     " << cam_entity.local_transform().rotation << '\n'
				<< "Camera POSITION: " << cam_entity.local_transform().translation << "\n\n"
				<< "Cube RIGHT:      " << cube_entity.local_transform().rotation.right_direction() << '\n'
				<< "Cube UP:         " << cube_entity.local_transform().rotation.up_direction() << '\n'
				<< "Cube FORWARD:    " << cube_entity.local_transform().rotation.forward_direction() << '\n'
				<< "Cube QUAT:       " << cube_entity.local_transform().rotation << '\n'
				<< "Cube POSITION:   " << cube_entity.local_transform().translation;
			stream.close();

			glEnable(GL_CULL_FACE);

			gt::Renderer::update();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
