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
		gt::init_all("geometry_buffer_showcase");
		gt::renderer::set_freeform_cursor(true);
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		gt::fontstream stream;
		gt::opengl::unit_cube_PUN the_shape;
		gt::material the_material(
			gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f),
			gt::vec4f(0.2f, 0.2f, 0.2f, 0.2f),
			"../examples/RuralBrickWall.jpg",
			"../examples/RuralBrickWall_SPEC.png",
			"../examples/RuralBrickWall_NRM.png"
		);
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

			auto mousedelta = gt::renderer::mouse_delta();
			mousedelta = -gt::deg2rad(mousedelta) / 2.0f;
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta.x, mousedelta.y);
			
			gt::renderer::begin_geometry_pass();
			
			const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
			
			const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
			
			const auto rotation_axis = gt::vec3f(0.0f, yaxis, zaxis).normalize();
			
			gt::renderer::set_model_matrix(rotation_axis, -curtime / 4.0f);
			
			the_material.bind();
			
			the_shape.draw();

			stream.open(font_inconsolata);
			gt::renderer::blit_drawbuffers_to_screen(stream);
			stream.close();

			// gt::renderer::get_text_shader()->activate();
			
			// gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
			
			// glDisable(GL_CULL_FACE);
			// glEnable(GL_BLEND);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			// stream.open(font_inconsolata);

			// stream << "Move around with WASD.\n"
			// 	<< "Look around with the mouse.\n"
			// 	<< "Go up by holding the spacebar.\n"
			// 	<< "Press Q to quit." << std::endl;
			// stream.close();
			
			// glEnable(GL_CULL_FACE);
			
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