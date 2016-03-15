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
	gt::proj_info projection_component;
	gt::camera cam_component;
	gt::entity cam_entity;
	projection_component.attach(cam_entity);
	cam_component.attach(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(-2.78f, 0.83f, 1.17f));
	cam_component.add_mouse(gt::vec2f(-gt::deg2rad(60.0f), -gt::deg2rad(9.0f)));
	try
	{
		gt::init_all("skybox", cam_entity);
		gt::renderer::set_freeform_cursor(true);
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		gt::fontstream stream;
		gt::unit_cube_PUN the_shape;
		gt::material the_material(
			gt::vec4f(1.0f, 1.0f, 1.0f, 0.0f), 
			gt::vec4f(0.0f, 0.0f, 0.0f, 1.0f), 
			"../examples/DaVinci.jpg"
		);

		// Remark: To get aligned textures, you need to
		// play around with ImageMagick. The commands
		// that worked for this particular example were:
		// $ convert -flip, for +X, -X, +Z, -Z
		// $ convert -flip -rotate 90, for +Y
		// $ convert -flop -rotate 90, for -Y

		gt::skybox the_skybox(gt::opengl::cube_texture(
			"../resources/skybox/siege_rt.tga", // positive X
			"../resources/skybox/siege_lf.tga", // negative X
			"../resources/skybox/siege_up.tga", // positive Y
			"../resources/skybox/siege_dn.tga", // negative Y
			"../resources/skybox/siege_bk.tga", // positive Z
			"../resources/skybox/siege_ft.tga"  // negative Z
		));

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

		gt::renderer::show();
		float curtime, dt;

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
				cam_entity.move_forward(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_A))
			{
				cam_entity.move_left(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_S))
			{
				cam_entity.move_backward(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_D))
			{
				cam_entity.move_right(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_SPACE))
			{
				cam_entity.move_up(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_C))
			{
				cam_entity.move_down(dt);
			}

			const auto mousedelta = -gt::deg2rad(gt::renderer::mouse_delta()) / 10.0f;
			cam_component.add_mouse(mousedelta);
			
			gt::renderer::begin_geometry_pass();

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			const auto rotation_axis = gt::vec3f(0.0f, yaxis, zaxis).normalize();
			cube_entity.set_rotation(gt::quatf::axis_angle(rotation_axis, -curtime / 4.0f));
			gt::renderer::set_model_matrix(cube_entity);
			cube_entity.material_component()->bind();
			cube_entity.mesh_component()->draw();

			gt::renderer::begin_light_pass();
			gt::renderer::ambient_light_pass();

			glCullFace(GL_FRONT);
			the_skybox.draw();

			gt::renderer::get_text_shader()->activate();
			gt::renderer::get_text_shader()->set_color(gt::vec3f(0.4f, 0.2f, 0.2f));

			glDisable(GL_CULL_FACE);			
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			stream.open(font_inconsolata);
			stream << "Move around with WASD.\n"
				<< "Look around with the mouse.\n"
				<< "Go up by holding the spacebar.\n"
				<< "Press Q to quit.\n"
				<< "Elapsed time: " << std::fixed << std::setprecision(1) << curtime << " seconds\n"
				<< "Frames per second: " << std::fixed << std::setprecision(1) << 1.0f / dt << '\n'
				<< "Camera pos:   " << gt::renderer::camera().local_transform().translation << '\n'
				<< "Camera up:    " << gt::renderer::camera().local_transform().rotation.up_direction() << '\n'
				<< "Camera right: " << gt::renderer::camera().local_transform().rotation.right_direction() << '\n';
			stream.close();
			
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