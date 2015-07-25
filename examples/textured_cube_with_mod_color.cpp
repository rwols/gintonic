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
		gt::init_all("textured_cube_with_mod_color");
		gt::renderer::set_freeform_cursor(true);
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		gt::fontstream stream;
		gt::opengl::unit_cube_PUN the_shape;
		gt::material_cd the_material(gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f), "../examples/bricks.jpg");
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
			auto mousedelta = gintonic::renderer::mouse_delta();
			mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);
			gt::renderer::bind_for_writing();
			const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			const auto rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			gt::renderer::set_model_matrix(-curtime / 4.0f, rotation_axis);
			the_material.diffuse_color[1] = yaxis;
			the_material.diffuse_color[2] = zaxis;
			the_material.bind();
			the_shape.draw();
			gt::renderer::bind_for_reading();
			gt::renderer::null_light_pass();
			gt::renderer::get_text_shader()->activate();
			gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
			glDisable(GL_CULL_FACE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			stream.open(font_inconsolata);
			stream << "Move around with WASD.\n"
				<< "Look around with the mouse.\n"
				<< "Go up by holding the spacebar.\n"
				<< "Press Q to quit.\n"
				<< "Elapsed time: " << std::fixed << std::setprecision(1) << curtime << " seconds\n"
				<< "Frames per second: " << std::fixed << std::setprecision(1) << 1.0f / dt << '\n'
				<< "Camera position: " << gt::get_default_camera().position << '\n'
				<< "Camera up:       " << gt::get_default_camera().up << '\n'
				<< "Camera right:    " << gt::get_default_camera().right << std::endl;
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