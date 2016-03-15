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

int main(int argc, char* argv[])
{
	// Define a camera.
	gt::proj_info projection_component;
	gt::camera cam_component;
	gt::entity cam_entity;
	projection_component.attach(cam_entity);
	cam_component.attach(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(0.0f, 0.0f, 4.0f));
	cam_component.add_mouse(gt::vec2f(0.0f, 0.0f));
	try
	{
		gt::init_all("text", cam_entity);
		gt::font::flyweight font_scriptin60("../examples/SCRIPTIN.ttf", 60);
		gt::fontstream stream;
		gt::unit_quad_P the_shape;
		gt::material the_material;
		gt::Renderer::show();
		float curtime;
		while (!gt::Renderer::shouldClose())
		{
			curtime = get_elapsed_time<float>();
			if (gt::Renderer::key(SDL_SCANCODE_Q) || gt::Renderer::key(SDL_SCANCODE_ESCAPE))
			{
				gt::Renderer::close();
			}
			gt::Renderer::begin_geometry_pass();
			gt::Renderer::set_model_matrix(gt::vec3f(0.0f, 0.0f, 1.0f), -curtime);
			the_material.bind();
			the_shape.draw();
			gt::Renderer::begin_light_pass();
			gt::Renderer::ambient_light_pass();
			gt::Renderer::get_text_shader()->activate();
			const gt::vec3f text_color((1.0f + std::sin(curtime)) / 2.0f, 1.0f, 1.0f);
			gt::Renderer::get_text_shader()->set_color(text_color);
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