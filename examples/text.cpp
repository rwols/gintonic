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
	try
	{
		// #ifdef BOOST_MSVC
		// 	boost::filesystem::current_path(gt::get_executable_path());
		// #else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		// #endif
		gt::init_all("text");
		gt::font::flyweight font_scriptin60("../examples/SCRIPTIN.ttf", 60);
		gt::fontstream stream;
		gt::opengl::unit_quad_P the_shape;
		gt::material the_material;
		gt::renderer::show();
		float curtime;
		while (!gt::renderer::should_close())
		{
			curtime = get_elapsed_time<float>();
			if (gt::renderer::key(SDL_SCANCODE_Q) || gt::renderer::key(SDL_SCANCODE_ESCAPE))
			{
				gt::renderer::close();
			}
			gt::renderer::begin_geometry_pass();
			gt::renderer::set_model_matrix(gt::vec3f(0.0f, 0.0f, 1.0f), -curtime);
			the_material.bind();
			the_shape.draw();
			gt::renderer::begin_light_pass();
			gt::renderer::null_light_pass();
			gt::renderer::get_text_shader()->activate();
			const gt::vec3f text_color((1.0f + std::sin(curtime)) / 2.0f, 1.0f, 1.0f);
			gt::renderer::get_text_shader()->set_color(text_color);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			stream.open(font_scriptin60);
			stream->position.x = (std::cos(curtime / 4.0f) / 2.0f) - 0.25f;
			stream->position.y = std::sin(curtime / 4.0f) / 2.0f;
			stream << "Hello world!\n"
				<< "Elapsed time: " << std::fixed << std::setprecision(1) 
				<< curtime << " seconds" << std::endl;
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