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
	try
	{
		gt::init_all("skybox");
		gt::renderer::set_freeform_cursor(true);
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		gt::fontstream stream;
		gt::opengl::unit_cube_PUN the_shape;
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
			"../resources/skybox/siege_up_flipped.tga", // positive Y
			"../resources/skybox/siege_dn_flipped.tga", // negative Y
			"../resources/skybox/siege_bk.tga", // positive Z
			"../resources/skybox/siege_ft.tga"  // negative Z
		));
		gt::renderer::show();
		float curtime, dt;
		gt::get_default_camera_entity().proj_info_component->update();
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
				gt::get_default_camera_entity().move_forward(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_A))
			{
				gt::get_default_camera_entity().move_left(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_S))
			{
				gt::get_default_camera_entity().move_backward(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_D))
			{
				gt::get_default_camera_entity().move_right(dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_SPACE))
			{
				gt::get_default_camera_entity().move_up(dt);
			}
			auto mousedelta = gintonic::renderer::mouse_delta();
			mousedelta = -gt::deg2rad(mousedelta) / 4.0f;
			gt::get_default_camera_entity().add_mousedelta(mousedelta);

			gt::get_default_camera_entity().add_translation(gt::vec3f(mousedelta.x, mousedelta.y, 0.0f));
			// gt::get_default_camera_entity().look_at(cube_entity);
			gt::get_default_camera_entity().proj_info_component->update();
			
			gt::renderer::begin_geometry_pass();

			const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			const auto rotation_axis = gt::vec3f(0.0f, yaxis, zaxis).normalize();
			gt::renderer::set_model_matrix(rotation_axis, -curtime / 4.0f);
			the_material.bind();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			the_shape.draw();

			gt::renderer::begin_light_pass();
			gt::renderer::null_light_pass();

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
				<< "Camera pos:   " << gt::renderer::camera()->global_transform().translation << '\n'
				<< "Camera up:    " << gt::renderer::camera()->global_transform().rotation.up_direction() << '\n'
				<< "Camera right: " << gt::renderer::camera()->global_transform().rotation.right_direction() << '\n';
				// << "Camera position: " << gt::get_default_camera_entity().position << '\n'
				// << "Camera up:       " << gt::get_default_camera_entity().up << '\n'
				// << "Camera right:    " << gt::get_default_camera_entity().right << std::endl;
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